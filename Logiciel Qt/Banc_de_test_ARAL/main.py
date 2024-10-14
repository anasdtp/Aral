import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QTextEdit, QDialog, QVBoxLayout, QTableWidgetItem, QPushButton, QTableWidget, QLabel, QComboBox, QTextEdit, QLineEdit, QHBoxLayout
from PySide6.QtCore import QThread, Signal, Slot, QTimer
from PySide6.QtGui import QColor, QBrush, QIcon
import serial
import serial.tools.list_ports
import json
import time
from ui_mainwindow import Ui_MainWindow
from ui_dialog import Ui_Dialog
from ui_tableauVoies import Ui_tableauVoies
import generatePDF
from ui_ficheValidation import Ui_FicheValidation
# from ui_switch import Ui_Switch
from ui_historique import Ui_Historique
from ui_selectionneur_voies import Ui_Selectionneur_Voies

# from CTKPopubKeyboarb
#Pour actualiser : 
#   pyside6-rcc -o Ressources_rc.py Ressources.qrc
#   pyside6-uic mainwindow.ui -o ui_mainwindow.py
#   pyside6-uic dialog.ui -o ui_dialog.py
#   pyside6-uic tableauBilan.ui -o ui_tableauVoies.py
#   pyside6-uic ficheValidation.ui -o ui_ficheValidation.py
#   pyside6-uic ajoutControleur.ui -o ui_ajoutControleur.py 
#   pyside6-uic selectionneur_voies.ui -o ui_selectionneur_voies.py

from donnees import *


# Modification à faire : 
#    Ajouter une fenetre pour selectionner une voie en particulier avec une alarme. Pour cela du coté de l'esp32 il faut que le troisieme etat renvoit les etats de chaque voie du point de vue de la carte aral, mais sans verification
#    Ajouter une fenetre de recherche des references d'un composants (par ex: on recherche U5 et ça nous donne la ref 74HC240 et à quoi il sert)
#    Quand on arrete un test et qu'on le relance, il faut reprendre de la voie 1
#    Afficher sur le compte rendu final si jamais il y a eu une perte de com
#    Quand on test une voie avec une alarme, on verifie si l'alarme qu'on veut est bien celle qui est affichée. Mais il faut ajouter une verification que toutes les autres voies n'est pas changé d'alarme(qu'elles soient restées en congruence)


class SerialThread(QThread):
    # message_received = Signal(bytes)
    def __init__(self, port = None, baudrate = None):
        super().__init__()
        if port is not None:
            self.port = port
            self.baudrate = baudrate
            self.serial = serial.Serial(port, baudrate)
            self.running = True
        else:
            self.port = None
            self.baudrate = None
            self.serial = None
            self.running = False

        self.stateRx = 0
        self.compteurData = 0

        self.lastTime = time.time()

        self.msgError = ""

    def run(self):
        while self.running:
            if self.serial.in_waiting > 0:
                data = self.serial.read(1)
                # print(data)
                # self.message_received.emit(data)
                self.RxReceive(data)
                # self.serial.write(b'\xFF')
                # sample_message = Message(id=1, length=3, data=[0x01, 0x02, 0x03])
                # packet = sample_message.build_packet()
                # self.serial.write(packet)
            if((time.time()-self.lastTime) > 3):
                self.lastTime = time.time()
                if(com.ecritureEnCours):
                    com.ecritureEnCours = False
                    self.serial.cancel_write() #Pour debloquer toutes les 3 secondes si jamais il y a un probléme
                    com.problemeEnEcriture = True
            

    def close(self):
        if self.running:
            self.running = False
            self.serial.close()
    
    # def send_data(self, data):
    #     if self.running:
    #         try:
    #             self.serial.write(data)
    #         except serial.SerialException as e:
    #             print(f"Failed to send data: {e}")
    #             self.close()
    #             self.running = False

    # @Slot(bytes)
    def RxReceive(self, message):
        byte = int.from_bytes(message, byteorder='big', signed=False)
        # self.ui.textEdit_panel.append(f"Received: {byte}")
        # print(f"Received: {message}")
        match self.stateRx:
            case 0:
                if byte == 0xff:
                    self.msgError = ""
                    self.msgError += " Header"
                    # print("Header")
                    self.stateRx = 1
                    com.rxMsg[com.FIFO_Ecriture].checksum = 0
            case 1:
                # print("ID")
                self.msgError += " ID" + str(byte.to_bytes())
                com.rxMsg[com.FIFO_Ecriture].id = int(byte)
                com.rxMsg[com.FIFO_Ecriture].checksum ^= byte
                self.stateRx = 2
            case 2:
                # print("len")
                self.msgError += " len" + str(byte.to_bytes())
                com.rxMsg[com.FIFO_Ecriture].len = int(byte)
                com.rxMsg[com.FIFO_Ecriture].checksum ^= byte
                com.rxMsg[com.FIFO_Ecriture].data = []
                self.compteurData = 0
                self.stateRx = 3
            case 3:
                # print("data n°", self.compteurData)
                self.msgError += " dt[" + str(self.compteurData) + "]= " + str(byte.to_bytes()) +"."
                com.rxMsg[com.FIFO_Ecriture].data.append(int(byte)) 
                com.rxMsg[com.FIFO_Ecriture].checksum ^= byte
                self.compteurData += 1
                if(self.compteurData >= com.rxMsg[com.FIFO_Ecriture].len):
                    self.compteurData = 0
                    self.stateRx = 4
            case 4:
                # print("checksum %d", byte)
                self.msgError += " checksum" + str(byte.to_bytes())
                if(com.rxMsg[com.FIFO_Ecriture].checksum == int(byte)):
                    self.stateRx = 5
                else :
                    self.stateRx = 0
                    print(self.msgError)
                    print(" ERROR Checksum mismatch msg n°"+ str(com.FIFO_Ecriture) +", "+ str(com.rxMsg[com.FIFO_Ecriture].checksum) + " != " + str(byte))
            case 5:
                # print("Header Fin")
                if byte == 0xFF:
                    self.msgError += " Header Fin"
                    print("Received new msg n°"+ str(com.FIFO_Ecriture) +" from id : ", com.rxMsg[com.FIFO_Ecriture].id.to_bytes())
                    com.FIFO_Ecriture = (com.FIFO_Ecriture + 1)%SIZE_FIFO
                print(self.msgError)
                self.stateRx = 0

com.serial_thread = SerialThread()
#end SerialThread

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.setWindowTitle("Banc de test carte ARAL")
        self.setWindowIcon(QIcon('logo.ico'))

        self.ui.sendButton_nbTours.clicked.connect(self.sendNbTours)
        self.ui.sendButton_repriseTest.clicked.connect(self.sendRelancerTest)
        self.ui.sendButton_arret.clicked.connect(self.sendArretTest)
        self.ui.sendButton_lancementTestNuit.clicked.connect(self.sendNbTours8Heures)

        self.ui.sendButton_activer_filtrage.clicked.connect(self.sendFiltrage)
        self.ui.sendButton_reglage_nb_etat_en_test.clicked.connect(self.sendModeTension)

        self.dialog = Dialog(self)
        self.bilan_window = BilanWindow(self)
        self.state_window = StateWindow(self)
        self.fiche_validation = FicheValidation(self)

        self.historique_des_tests = Historique(self)
        self.selectionneur_voies = SelectionneurVoies(self)
        # self.switchARAL = SwitchAral()
        self.ui.actionTableau_Voies_Bilan.triggered.connect(self.openBilanWindow)
        self.ui.actionTableau_Voies_en_Cours.triggered.connect(self.openStateWindow)
        self.ui.actionFicheValidation.triggered.connect(self.openFicheValidation)
        self.ui.actionHistorique.triggered.connect(self.openHistoriqueDesTests)
        self.ui.Selectionneur_Voies.triggered.connect(self.openSelectionneurVoies)

        self.ui.actionQuit.triggered.connect(self.QuitWindows)
        self.ui.actionClearLog.triggered.connect(self.textPanel_Clear)
        self.ui.actionConnect.triggered.connect(self.openDialogWindow)
        self.ui.actionDisconnect.triggered.connect(self.closeSerial)
        self.ui.actionReset.triggered.connect(self.resetStruct)

        print("Initialized MainWindow")

        # Configuration du QTimer
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.RxManage)
        self.timer.start(1)  # Déclenchement toutes les 1 ms
        self.FIFO_lecture = 0
        self.FIFO_occupation = 0
        self.FIFO_max_occupation = 0
        print("End Initialization MainWindow")

    
    def start_serial(self):
        selected_port = self.dialog.ui.comboBox.currentText()
        print(selected_port)
        if selected_port:
            try:
                com.serial_thread = SerialThread(selected_port, SERIAL_BAUDRATE)
                com.serial_thread.start()
                print("Starting serial com")
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"-----------Starting serial com")
                self.ui.textEdit_panel.append(f"-----------Port: {selected_port}")
                self.dialog.accept()
            except serial.SerialException as e:
                print("Serial Error", f"Failed to open port {selected_port}: {e}")
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"-----------Serial Error !!")
                self.ui.textEdit_panel.append(f"-----------Failed to open port {selected_port}: {e}")
        

    def RxManage(self):
        # print("RxManage")
        self.FIFO_occupation = com.FIFO_Ecriture - self.FIFO_lecture
        if(self.FIFO_occupation<0):
            self.FIFO_occupation = self.FIFO_occupation + SIZE_FIFO
        if(self.FIFO_max_occupation < self.FIFO_occupation):
            self.FIFO_max_occupation = self.FIFO_occupation
        if(self.FIFO_occupation == 0):
            return

        match com.rxMsg[self.FIFO_lecture].id:
            case 0xA0:#ID_NB_TOURS
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"Received message from ID_NB_TOURS??? Bizarre")
                pass
            case 0xA1:#ID_ACK_NB_TOURS
                message = f"ID_ACK_NB_TOURS : Banc de test à bien reçu le nombre de tours à faire"
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel_ACK.append(f"")
                self.ui.textEdit_panel.append(message)
                self.ui.textEdit_panel_ACK.append(message)
            case 0xB0:#ID_INITIALISATION_ARAL_EN_COURS
                nbEssai = com.rxMsg[self.FIFO_lecture].data[0]
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_INITIALISATION_ARAL_EN_COURS : initialisation carte aral... " + str(nbEssai))
            case 0xB1:#ID_INITIALISATION_ARAL_FAITE
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_INITIALISATION_ARAL_FAITE : carte aral initialisée!!")
            case 0xB2:#ID_TEST_EN_COURS
                voies.bilan = com.rxMsg[self.FIFO_lecture].data
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_TEST_EN_COURS : bilan : " + str(voies.bilan))
                self.bilan_window.update_states()
            case 0xB3:#ID_TEST_TERMINEE
                voies.bilan = com.rxMsg[self.FIFO_lecture].data
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_TEST_TERMINEE : bilan : " + str(voies.bilan))
                self.bilan_window.update_states()
                self.bilan_window.show()
            case 0xB4:#ID_ETAT_VOIES 
                voies.voies = com.rxMsg[self.FIFO_lecture].data
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_ETAT_VOIES : etat voies : " + str(voies.voies))
                self.state_window.update_states()
            case 0xB5:#ID_ETAT_UNE_VOIE
                numVoie = com.rxMsg[self.FIFO_lecture].data[0]
                etatVoie = com.rxMsg[self.FIFO_lecture].data[1]
                if(numVoie>0 and numVoie<=96):
                    voies.bilan[numVoie-1] = etatVoie
                    self.bilan_window.update_states()
                    if(etatVoies == etatBilan["DEFAUT"]):
                        message = f"ID_ETAT_UNE_VOIE : Voie n°"+ str(numVoie) +" en DEFAUT"
                        self.ui.textEdit_panel.append(f"")
                        self.ui.textEdit_panel_ACK.append(f"")
                        self.ui.textEdit_panel.append(message)
                        self.ui.textEdit_panel_ACK.append(message)
            case 0xB6:#ID_CARTE_ARAL_NE_REPOND_PLUS
                voies.perteDeCom += 1
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_CARTE_ARAL_NE_REPOND_PLUS : carte ARAL ne répond plus !! Defaut COM! " + str(voies.perteDeCom))
            case 0xB7:#ID_CARTE_ARAL_REPEAT_REQUEST
                message = f"ID_CARTE_ARAL_REPEAT_REQUEST : la carte aral demande de repeter le message"
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel_ACK.append(f"")
                self.ui.textEdit_panel.append(message)
                self.ui.textEdit_panel_ACK.append(message)
            case 0xB8:#ID_TEST_TEMPS_DE_REPONSE_FILTRAGE
                # voies.tempsDeReponse = com.rxMsg[self.FIFO_lecture].data / 10.0 #reçu en diziéme de secondes, convertit lors du stockage ici en secondes
                for i in range(96):
                    voies.tempsDeReponse[i] = com.rxMsg[self.FIFO_lecture].data[i] / 10.0
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_TEST_TEMPS_DE_REPONSE_FILTRAGE : Reception des temps de reponse de chaque voies")
            case 0xB9:#ID_TEST_TEMPS_DE_REPONSE_FILTRAGE_UNE_VOIE
                numVoie = com.rxMsg[self.FIFO_lecture].data[0]
                tempsDeReponse = com.rxMsg[self.FIFO_lecture].data[1] / 10.0 #reçu en diziéme de secondes, convertit lors du stockage ici en secondes
                if(numVoie>0 and numVoie<=96):
                    voies.tempsDeReponse[numVoie-1] = tempsDeReponse
                    self.ui.textEdit_panel.append(f"")
                    self.ui.textEdit_panel.append(f"ID_TEST_TEMPS_DE_REPONSE_FILTRAGE_UNE_VOIE : temps de reponse de la voie n°"+ str(numVoie) +" : " + str(tempsDeReponse) +" secondes")
            case 0xC0:#ID_ACK_GENERAL
                message = f"ID_ACK_GENERAL : reponse gen"#, " + idComEnText[com.rxMsg[self.FIFO_lecture].data[0]] + " : " + str(com.rxMsg[self.FIFO_lecture].data[0])
                if(com.rxMsg[self.FIFO_lecture].len > 0):
                    message += ", " + idComEnText[com.rxMsg[self.FIFO_lecture].data[0]] + " : "
                    for i in range(1, com.rxMsg[self.FIFO_lecture].len):
                        message += str(com.rxMsg[self.FIFO_lecture].data[i]) + ", "
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel_ACK.append(f"")
                self.ui.textEdit_panel.append(message)
                self.ui.textEdit_panel_ACK.append(message)

            case 0xD0:#ID_REPEAT_REQUEST
                message = f"ID_REPEAT_REQUEST : le banc de test n'a pas compris, message incohérent"
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel_ACK.append(f"")
                self.ui.textEdit_panel.append(message)
                self.ui.textEdit_panel_ACK.append(message)
            case 0xD2:#ID_ACK_REQUEST_NB_TOURS_FAIT
                nbToursFait = com.rxMsg[self.FIFO_lecture].data[0] #+ com.rxMsg[self.FIFO_lecture].data[1]<<8
                voies.nombreDeTourFait = nbToursFait
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel_ACK.append(f"")
                self.ui.textEdit_panel.append(f"ID_ACK_REQUEST_NB_TOURS_FAIT : Nombre de tours fait : " + str(voies.nombreDeTourFait))
                self.ui.textEdit_panel_ACK.append(f"ID_ACK_REQUEST_NB_TOURS_FAIT : Nombre de tours fait : " + str(voies.nombreDeTourFait))
            case _:
                self.ui.textEdit_panel.append(f"Received message from an unknown ID")
        self.FIFO_lecture = (self.FIFO_lecture + 1) % SIZE_FIFO

    def sendMsg(self, msg = Message()):
        # sample_message = Message(id=1, length=3, data=[0x01, 0x02, 0x03])
        packet = msg.build_packet()
        print(packet)
        if com.serial_thread.running:
            try:
                com.ecritureEnCours = True
                com.serial_thread.serial.write(packet) #Fonction bloquante, qui se debloque toutes les 3 secondes si l'envoi à echouer
                com.ecritureEnCours = False
                if(com.problemeEnEcriture):
                    com.problemeEnEcriture = False
                    self.ui.textEdit_panel.append(f"")
                    self.ui.textEdit_panel.append(f"-----------Problème rencontré lors de l'envoi de données")
                    self.ui.textEdit_panel.append(f"-----------Deconnexion du PORT COM...")
                    com.serial_thread.close()
                    self.ui.textEdit_panel.append(f"-----------Essayer de vous reconnectez svp")
            except (serial.SerialException) as e:
                error_message = f"Failed to send data: {e.__class__.__name__}: {e}"
                print(error_message)
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"-----------{error_message}")
                com.serial_thread.close()
                self.ui.textEdit_panel.append(f"-----------Essayer de vous reconnectez svp")
        else:
            self.ui.textEdit_panel.append(f"")
            self.ui.textEdit_panel.append(f"-----------Aucun PORT COM de connecté! Veuillez-vous connectez.")
                

    def sendEmpty(self, id):
        sample_message = Message(id, length=0, data=[0])
        self.sendMsg(sample_message)

    def sendByte(self, id, byte):
        sample_message = Message(id, length=1, data=[byte & 0xFF])
        self.sendMsg(sample_message)

    def sendTwoBytes(self, id, bytes):
        sample_message = Message(id, length=2, data=[(bytes&0xFF), (bytes>>8 & 0xFF)])
        self.sendMsg(sample_message)

    def sendNbTours(self):
        print("sendNbTours called")
        nbTours = int(self.ui.comboBox_nbTours.currentText())
        self.sendTwoBytes(ID_NB_TOURS, nbTours)
    
    def sendRelancerTest(self):
        print("sendRelancerTest called")
        self.sendEmpty(ID_RELANCER_TEST)

    def sendArretTest(self):
        print("sendArretTest called")
        self.sendEmpty(ID_ARRET_TEST)
    
    def sendFiltrage(self): #  ----  FILTRAGE ---- Filtrage = etat stable pendant un temps desiré (etat stable = ne prends pas en compte les changements)
        print("sendFiltrage called")
        activer = 1
        if(self.ui.sendButton_activer_filtrage.text() == "Activer Filtrage"):
            activer = 1
            self.ui.sendButton_activer_filtrage.setText("Désactiver Filtrage")
        else:
            activer = 0
            self.ui.sendButton_activer_filtrage.setText("Activer Filtrage")
        self.sendByte(ID_SET_FILTRAGE, activer)

    def sendModeTension(self):
        print("sendModeTension called")
        mode = 1
        if(self.ui.sendButton_reglage_nb_etat_en_test.text() == "Mettre le mode avec 2 états, Court-circuit et Alarme"):
            mode = 1
            self.ui.sendButton_reglage_nb_etat_en_test.setText("Mettre le mode 4 états")
        else:
            mode = 2
            self.ui.sendButton_reglage_nb_etat_en_test.setText("Mettre le mode avec 2 états, Court-circuit et Alarme")
        self.sendByte(ID_SET_MODE_TENSION, mode)
    
    def sendNbTours8Heures(self):#1 tours avec 4 états = 3 min30, avec 2 etats = 1min45. Soit pour atteindre 8 Heures...
        nbTours = 128
        print("sendNbTours8Heures called")
        if(self.ui.sendButton_reglage_nb_etat_en_test.text() == "Mettre le mode avec 2 états, Court-circuit et Alarme"):
            nbTours = 128
        else:
            nbTours = 267
        self.sendRelancerTest()
        self.sendTwoBytes(ID_NB_TOURS, nbTours)

    def sendCommandeVoie(self, voies = {"numVoie": 0, "etatVoie": 0}):
        print("sendCommandeVoie called")
        sample_message = Message(ID_COMMANDE_VOIE, length=2, data=[voies["numVoie"], voies["etatVoie"]])
        self.sendMsg(sample_message)

    def openBilanWindow(self):
        self.sendEmpty(ID_REQUEST_BILAN)
        self.sendEmpty(ID_REQUEST_NB_TOURS_FAIT)
        self.bilan_window.show()
        self.bilan_window.raise_()
        self.bilan_window.activateWindow()
        self.bilan_window.update_states()
    def openStateWindow(self):
        self.sendEmpty(ID_REQUEST_NB_TOURS_FAIT)
        self.state_window.show()
        self.state_window.raise_()
        self.state_window.activateWindow()
        self.state_window.update_states()
    def openDialogWindow(self):
        self.dialog.show()
        self.dialog.raise_()
        self.dialog.activateWindow()
        self.dialog.populate_com_ports() #Permet d'aller chercher et actualiser les ports COM de disponible. Ce qui permet de ne pas evoir fermer le logiciel et le reouvrir
    def openFicheValidation(self):
        self.sendEmpty(ID_REQUEST_BILAN)
        self.sendEmpty(ID_REQUEST_NB_TOURS_FAIT)
        self.fiche_validation.show()
        self.fiche_validation.raise_()
        self.fiche_validation.activateWindow()
        self.fiche_validation.init_controleurs_comboBox()
    def openHistoriqueDesTests(self):
        self.historique_des_tests.show()
        self.historique_des_tests.raise_()
        self.historique_des_tests.activateWindow()
        self.historique_des_tests.init()
    def openSelectionneurVoies(self):
        self.selectionneur_voies.show()
        self.selectionneur_voies.raise_()
        self.selectionneur_voies.activateWindow()

        self.sendArretTest()

    def closeEvent(self, event):
        print("Au revoir")
        self.dialog.close()
        self.state_window.close()
        self.bilan_window.close()
        self.fiche_validation.close()
        self.historique_des_tests.close()
        self.selectionneur_voies.close()
        # self.switchARAL.close()
        super().closeEvent(event)
    
    # def openSwitchARAL(self):
    #     self.switchARAL.show()
    #     self.switchARAL.raise_()
    #     self.switchARAL.activateWindow()
    
    def textPanel_Clear(self):
        self.ui.textEdit_panel.clear()
        self.ui.textEdit_panel_ACK.clear()
    
    def closeSerial(self):
        self.ui.textEdit_panel.append(f"")
        self.ui.textEdit_panel.append(f"-----------Deconnexion du PORT COM...")
        com.serial_thread.close()
    
    def resetStruct(self):
        self.ui.textEdit_panel.append(f"")
        self.ui.textEdit_panel.append(f"-----------Refraichissement...")
        voies.voies = [etatVoies["NONE"] for _ in range(NOMBRE_VOIES)]
        voies.bilan = [etatBilan["test non fait"] for _ in range(NOMBRE_VOIES)]
        voies.perteDeCom = 0
        voies.nombreDeTourFait = 0

        self.bilan_window.update_states()
        self.state_window.update_states()
        
    def QuitWindows(self):
        self.close()
        QApplication.quit()
        
    
#end MainWindow

class Dialog(QDialog):
    def __init__(self, main_window : MainWindow):
        super().__init__()
        self.main_window = main_window  # Stocker la référence à MainWindow
        self.ui = Ui_Dialog()
        self.ui.setupUi(self)
        self.setWindowTitle("CHOIX PORT COM")
        self.setWindowIcon(QIcon('logo.ico'))
        # self.ui.buttonBox.accepted.connect(self.start_serial) #Cas si jamais on n'appuie sur OK. Fonction dans la mainwindow pour afficher à l'utilisateur ce qu'on a fait
        self.ui.buttonBox.accepted.connect(self.main_window.start_serial)
        self.ui.buttonBox.rejected.connect(self.reject) #Cas si jamais on n'appuie sur annuler

        self.populate_com_ports()

    def populate_com_ports(self):
        self.ui.comboBox.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.ui.comboBox.addItem(port.device)

    def start_serial(self):
        selected_port = self.ui.comboBox.currentText()
        print(selected_port)
        if selected_port:
            try:
                com.serial_thread = SerialThread(selected_port, SERIAL_BAUDRATE)
                com.serial_thread.start()
                print("Starting serial com")
                self.accept()
            except serial.SerialException as e:
                print("Serial Error", f"Failed to open port {selected_port}: {e}")
#end Dialog

class BilanWindow(QDialog):
    def __init__(self, main_window = None):
        super().__init__()
        self.main_window = main_window  # Stocker la référence à MainWindow
        self.ui = Ui_tableauVoies()
        self.ui.setupUi(self)
        self.update_states()
        self.setWindowTitle("Bilan de test")
        self.setWindowIcon(QIcon('logo.ico'))

    # @Slot(list)
    def update_states(self):
        for i in range(10):
            for j in range(10):
                index = i * 10 + j
                # print("j : " + str(j) + " i : " + str(i) + " index : " + str(index))
                if(index<96):
                    
                    item = QTableWidgetItem()
                    if voies.bilan[index] == etatBilan["OK"]:
                        item.setBackground(QColor("darkGreen"))
                        item.setText('OK')
                    elif voies.bilan[index] == etatBilan["DEFAUT"]:
                        item.setBackground(QColor("red"))
                        item.setText('DEFAUT')
                    else:
                        item.setBackground(QColor("gray"))
                        item.setText('Non-testée')

                    self.ui.tableWidget.setItem(i, j, item)
                else:
                    item = QTableWidgetItem()
                    item.setText('Tour n°' + str(voies.nombreDeTourFait))
                    self.ui.tableWidget.setItem(i, j, item)
                    return


class StateWindow(QDialog):
    def __init__(self, main_window = None):
        super().__init__()
        self.main_window = main_window  # Stocker la référence à MainWindow
        self.ui = Ui_tableauVoies()
        self.ui.setupUi(self)
        self.update_states()
        self.setWindowTitle("Etat Voies en Cours, envoyé par la carte ARAL")
        self.setWindowIcon(QIcon('logo.ico'))

    # @Slot(list)
    def update_states(self):
        for i in range(10):
            for j in range(10):
                index = i * 10 + j
                # print("j : " + str(j) + " i : " + str(i) + " index : " + str(index))
                if(index<96):
                    item = QTableWidgetItem()
                    
                    if voies.voies[index] == etatVoies["COURT_CIRCUIT"]:
                        text = 'Court-Circuit, ' + str(voies.tempsDeReponse[index]) + ' s'
                        color = QColor("darkCyan")
                    elif voies.voies[index] == etatVoies["ALARME"]:
                        text = 'Alarme, ' + str(voies.tempsDeReponse[index]) + ' s'
                        color = QColor("darkMagenta")
                    elif voies.voies[index] == etatVoies["NORMAL"]:
                        text = 'Normal, ' + str(voies.tempsDeReponse[index]) + ' s'
                        color = QColor("green")
                    elif voies.voies[index] == etatVoies["CONGRUENCE"]:
                        text = 'Congruence, ' + str(voies.tempsDeReponse[index]) + ' s'
                        color = QColor("gray")
                    else:
                        text = ''
                        color = QColor("gray")
                    
                    item.setText(text)
                    item.setBackground(color)

                    self.ui.tableWidget.setItem(i, j, item)
                else:
                    item = QTableWidgetItem()
                    item.setText('Tour n°' + str(voies.nombreDeTourFait))
                    self.ui.tableWidget.setItem(i, j, item)
                    return

        

class donneesFiche():
    def __init__(self):
        self.numSerie = ""
        self.controleurTechnique = ""
        self.controleurExterne = ""
        self.commentaires = generatePDF.CommentaireCarteFonctionnelle
donnees = donneesFiche()

class FicheValidation(QDialog):
    def __init__(self, main_window : MainWindow):
        super().__init__()
        self.main_window = main_window  # Stocker la référence à MainWindow
        self.ui = Ui_FicheValidation()
        self.ui.setupUi(self)
        self.setWindowTitle("Etat Voies en Cours, envoyé par la carte ARAL")
        self.setWindowIcon(QIcon('logo.ico'))
        print("Fiche Validation")
        self.ui.buttonBox.accepted.connect(self.createPDFFicheValidation)
        self.ui.buttonBox.rejected.connect(self.reject)
        
        self.ui.pushButton_num_serie_generer_auto.clicked.connect(self.genererNumSerie)
        self.init_controleurs_comboBox()

        self.ui.lineEdit_num_serie.setText(donnees.numSerie)

    def init_controleurs_comboBox(self):
        self.ui.comboBox_controleur_technique.clear()
        self.ui.comboBox_controleur_externe.clear()
        try:
            with open(generatePDF.output_directory + '/controleur_technique.json', "r") as file:
                items = json.load(file)
                print("Loading controleur technique ", items)
                self.ui.comboBox_controleur_technique.addItems(items)
        except (FileNotFoundError, json.JSONDecodeError):
            print(generatePDF.output_directory + "/controleur_technique.json error or not found")
            # pass  # No items to load or file is empty
        try:
            with open(generatePDF.output_directory + '/controleur_externe.json', "r") as file:
                items = json.load(file)
                print("Loading controleur externe ", items)
                self.ui.comboBox_controleur_externe.addItems(items)
        except (FileNotFoundError, json.JSONDecodeError):
            print(generatePDF.output_directory + "/controleur_externe.json error or not found")
            # pass  # No items to load or file is empty

    def genererNumSerie(self):
        donnees.numSerie = generatePDF.generer_numero_serie("17")
        self.ui.lineEdit_num_serie.setText(donnees.numSerie)

    def regrouper_voies_par_etat(self, bilan):
        groupes = []
        debut = 0
        etat_actuel = bilan[0]
        for i in range(1, len(bilan)):
            if bilan[i] != etat_actuel:
                groupes.append((debut, i - 1, etat_actuel))
                debut = i
                etat_actuel = bilan[i]
        groupes.append((debut, len(bilan) - 1, etat_actuel))
        return groupes

    def getCommentaires(self):
        commentaires = ""        
        if self.ui.checkBox_prise_en_compte_test.isChecked():
            if all(bilan == etatBilan["OK"] for bilan in voies.bilan):
                commentaires = generatePDF.CommentaireCarteFonctionnelle
            else:
                groupes = self.regrouper_voies_par_etat(voies.bilan)
                problemes = []
                for debut, fin, etat in groupes:
                    etat_str = [key for key, value in etatBilan.items() if value == etat][0]
                    if debut == fin:
                        problemes.append(f"Voie {debut + 1} : {etat_str}")
                    else:
                        problemes.append(f"Voies {debut + 1} à {fin + 1} : {etat_str}")
                commentaires = generatePDF.CommentaireCarteMinimal + "Des problèmes ont été détectés dans le bilan des tests:\n" + "\n".join(problemes)
            if(voies.perteDeCom>0):
                commentaires += "Perte(s) de communication durant le test ("+ str(voies.perteDeCom)+" fois)\n"
        else:
            commentaires = generatePDF.CommentaireCarteFonctionnelle
        
        return commentaires

    def createPDFFicheValidation(self):
        donnees.numSerie = self.ui.lineEdit_num_serie.text()
        donnees.controleurTechnique = self.ui.comboBox_controleur_technique.currentText()
        donnees.controleurExterne = self.ui.comboBox_controleur_externe.currentText()
        generatePDF.add_items_to_json(generatePDF.output_directory + '/controleur_technique.json', donnees.controleurTechnique)
        generatePDF.add_items_to_json(generatePDF.output_directory + 'controleur_externe.json', donnees.controleurExterne)

        donnees.commentaires = self.getCommentaires()
        fiche = generatePDF.FicheValidation()
        output = fiche.generateFicheValidation(donnees.numSerie, donnees.controleurTechnique, donnees.controleurExterne, donnees.commentaires)
        fiche.writePDF(output) 
        self.accept()


class DonneesHistorique():
    def __init__(self):
        self.dic_historique = {}
        self.add_historique("0", "", "", "")

    def add_historique(self, num_serie, date, en_panne, commentaires):
        if num_serie in self.dic_historique:
            self.dic_historique[num_serie].append({"date": date, "en_panne": en_panne, "commentaires": commentaires})
        else:
            self.dic_historique[num_serie] = [{"date": date, "en_panne": en_panne, "commentaires": commentaires}]

    def get_ALL_data(self):
        return self.dic_historique
    
    def get_data(self, num_serie):
        return self.dic_historique.get(num_serie, [])
    
    def reset_all_data(self):
        self.dic_historique = {}

    def load_historique_from_json(self, filename):
        try:
            with open(filename, 'r') as file:
                self.dic_historique = json.load(file)
        except (FileNotFoundError, json.JSONDecodeError):
            # self.dic_historique = {}
            print("Error loading historique or json file not found")

    def save_historique_to_json(self, filename):
        with open(filename, 'w') as file:
            json.dump(self.dic_historique, file, indent=4)

historiqueData = DonneesHistorique()

class Historique(QDialog):
    def __init__(self, main_window = None):
        super().__init__()
        self.main_window = main_window  # Stocker la référence à MainWindow
        self.ui = Ui_Historique()
        self.ui.setupUi(self)
        self.setWindowTitle("Historique des cartes ARAL")
        self.setWindowIcon(QIcon('logo.ico'))

        self.ui.buttonBox.accepted.connect(self.update)
        self.ui.buttonBox.rejected.connect(self.reject)

        self.ui.pushButton_nouvelle_panne.clicked.connect(self.AfficherSaisieHistorique)
        self.ui.comboBox_num_serie.currentIndexChanged.connect(self.AfficherHistorique)

        self.output_historique_directory = generatePDF.output_directory + '/Historique'
        generatePDF.ensure_directory_exists(self.output_historique_directory)
        self.output_JSON_num_serie = self.output_historique_directory + '/num_serie.json'
        self.output_JSON_historique = self.output_historique_directory + '/historique.json'

        self.populate_combo_box_num_serie()
        historiqueData.load_historique_from_json(self.output_JSON_historique)

        self.label_date = QLabel("Date :")
        self.line_edit_date = QLineEdit()
        self.label_en_panne = QLabel("En Panne?")
        self.combo_box_panne = QComboBox()
        self.combo_box_panne.addItems(["Oui", "Non", "Peut-être"])
        self.label_commentaires = QLabel("Commentaires :")
        self.text_edit_commentaires = QTextEdit()

        

        # self.layout().insertWidget(self.layout().count() - 1, self.label_date)
        # self.layout().insertWidget(self.layout().count() - 1, self.line_edit_date)
        layout_date = QHBoxLayout()
        layout_date.addWidget(self.label_date)
        layout_date.addWidget(self.line_edit_date)
        self.layout().insertLayout(self.layout().count() - 1, layout_date)

        # self.layout().insertWidget(self.layout().count() - 1, self.label_en_panne)
        # self.layout().insertWidget(self.layout().count() - 1, self.combo_box_panne)
        layout_panne = QHBoxLayout()
        layout_panne.addWidget(self.label_en_panne)
        layout_panne.addWidget(self.combo_box_panne)
        self.layout().insertLayout(self.layout().count() - 1, layout_panne)

        self.layout().insertWidget(self.layout().count() - 1, self.label_commentaires)
        self.layout().insertWidget(self.layout().count() - 1, self.text_edit_commentaires)

        self.label_date.hide()
        self.line_edit_date.hide()
        self.label_en_panne.hide()
        self.combo_box_panne.hide()
        self.label_commentaires.hide()
        self.text_edit_commentaires.hide()

    def populate_combo_box_num_serie(self):
        try:
            with open(self.output_JSON_num_serie, "r") as file:
                num_series = json.load(file)
                self.ui.comboBox_num_serie.addItems(num_series)
                print("Loading num_serie", num_series)
        except (FileNotFoundError, json.JSONDecodeError):
            print("num_serie.json error or not found")
            # pass  # No items to load or file is empty

    def init(self):
        self.ui.comboBox_num_serie.clear()
        self.populate_combo_box_num_serie()

        self.ui.label_historique.show()
        self.ui.textEdit_historique.show()
        self.ui.textEdit_historique.clear()
        historiqueData.load_historique_from_json(self.output_JSON_historique)
        self.AfficherHistorique()

        self.ui.pushButton_nouvelle_panne.show()

        self.label_date.hide()
        self.line_edit_date.hide()
        self.line_edit_date.setText("")
        self.label_en_panne.hide()
        self.combo_box_panne.hide()
        self.label_commentaires.hide()
        self.text_edit_commentaires.hide()

    def AfficherHistorique(self):
        self.ui.textEdit_historique.clear()
        historiqueSpecifique = historiqueData.get_data(self.ui.comboBox_num_serie.currentText())
        # historiqueSpecifique = historique_list[0]  # Accède au premier élément de la liste
        print(historiqueSpecifique)
        for historique in historiqueSpecifique:
            # text = (
            #     historique["date"] + 
            #     ":\nEn panne ? " + 
            #     historique["en_panne"] + 
            #     ".\nCommentaires :\n" + 
            #     historique["commentaires"] + " \n"
            # )
            self.ui.textEdit_historique.append('\n' + historique["date"] + '--------------------------- ')
            
            self.ui.textEdit_historique.append("En panne? ")
            self.ui.textEdit_historique.append(historique["en_panne"])

            self.ui.textEdit_historique.append("\nCommentaires : ")
            self.ui.textEdit_historique.append(historique["commentaires"] + '\n')
    
    def AfficherSaisieHistorique(self):
        self.ui.label_historique.hide()
        self.ui.textEdit_historique.hide()
        self.ui.pushButton_nouvelle_panne.hide()
        self.label_date.show()
        self.line_edit_date.show()
        self.label_en_panne.show()
        self.combo_box_panne.show()
        self.label_commentaires.show()
        self.text_edit_commentaires.show()
        self.text_edit_commentaires.clear()

        self.line_edit_date.setText(generatePDF.get_current_date_string())
        
    
    def update(self):#Actualisation des JSON 
        generatePDF.add_items_to_json(self.output_JSON_num_serie, self.ui.comboBox_num_serie.currentText())
        if(self.line_edit_date.text() != ""):
            historiqueData.add_historique(self.ui.comboBox_num_serie.currentText(), 
                                          self.line_edit_date.text(), 
                                          self.combo_box_panne.currentText(), 
                                          self.text_edit_commentaires.toPlainText())
        historiqueData.save_historique_to_json(self.output_JSON_historique)
        print(historiqueData.get_ALL_data())


class SelectionneurVoies(QDialog):
    def __init__(self, main_window : MainWindow):
        super().__init__()
        self.main_window = main_window  # Stocker la référence à MainWindow
        self.ui = Ui_Selectionneur_Voies()
        self.ui.setupUi(self)
        self.setWindowTitle("Selectionneur Voies")
        self.setWindowIcon(QIcon('logo.ico'))

        self.mode = 0
        self.ui.pushButton.clicked.connect(self.basculerSurAutrePartieVoies)
        
        self.connect_sliders()

        self.consigneVoies = {
            "numVoie": 0,
            "etatVoie": 0
        }

    def basculerSurAutrePartieVoies(self):
        if(self.ui.pushButton.text() == "Appuyer sur ce bouton pour pouvoir selectionner les voies 49-96"):
            self.mode = 1
            self.ui.pushButton.setText("Appuyer sur ce bouton pour pouvoir selectionner les voies 01-48")
        else:
            self.mode = 0
            self.ui.pushButton.setText("Appuyer sur ce bouton pour pouvoir selectionner les voies 49-96")
        print("Mode : " + str(self.mode))
        self.reset_sliders()

    def connect_sliders(self):
        for voie in range(1, 49):
            slider = getattr(self.ui, f'verticalSlider_{voie}')
            slider.valueChanged.connect(lambda value, name=f'verticalSlider_{voie}': self.update_voies(name, value))
    
    def reset_sliders(self, withoutOne = None):
        for voie in range(1, 49):
            if(withoutOne != voie):
                slider = getattr(self.ui, f'verticalSlider_{voie}')
                slider.setValue(0)

    def update_voies(self, numVoie, etatVoie):
        numVoie = int(numVoie.split('_')[1])
        etatVoie = 3 - int(etatVoie) 
        self.reset_sliders(numVoie)
        if(self.mode == 1):
            numVoie += 48
        print("Voie n°" + str(numVoie) + " : " + str(etatVoie))

        self.consigneVoies["numVoie"] = numVoie
        self.consigneVoies["etatVoie"] = etatVoie if etatVoie >= 0 else 3
        self.main_window.sendCommandeVoie(self.consigneVoies)
        

def main():
    app = QApplication([]) 
    main_window = MainWindow()
    main_window.show()

    main_window.dialog.show()

    sys.exit(app.exec())

if __name__ == '__main__':
    main()



# class SwitchAral(QDialog):
#     def __init__(self):
#         super().__init__()
#         self.ui = Ui_Switch()
#         self.ui.setupUi(self)
#         self.setWindowTitle("Switch ARAL")
#         self.setWindowIcon(QIcon('switch.png'))

#         self.ui.buttonBox.accepted.connect(self.switch_aral)
#         self.ui.buttonBox.rejected.connect(self.reject)
    
#     def switch_aral(self):
#         pass