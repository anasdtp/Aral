import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QTextEdit, QDialog, QVBoxLayout, QTableWidgetItem, QPushButton, QTableWidget
from PySide6.QtCore import QThread, Signal, Slot, QTimer
from PySide6.QtGui import QColor, QBrush
import serial
import serial.tools.list_ports
import struct
import json
import time
from ui_mainwindow import Ui_MainWindow
from ui_dialog import Ui_Dialog
from ui_tableauVoies import Ui_tableauVoies
import generatePDF
from ui_ficheValidation import Ui_FicheValidation
from ui_ajoutControleur import Ui_AjoutControleur
#Pour actualiser : 
#   pyside6-rcc -o Ressources_rc.py Ressources.qrc
#   pyside6-uic mainwindow.ui -o ui_mainwindow.py
#   pyside6-uic dialog.ui -o ui_dialog.py
#   pyside6-uic tableauBilan.ui -o ui_tableauVoies.py
#   pyside6-uic ficheValidation.ui -o ui_ficheValidation.py
#   pyside6-uic ajoutControleur.ui -o ui_ajoutControleur.py 

SERIAL_BAUDRATE = 921600

ID_NB_TOURS     = 0xA0 #On envoi le nombre de tours à faire sur 2 octets
ID_ACK_NB_TOURS = 0xA1 #RIEN

ID_INITIALISATION_ARAL_EN_COURS = 0xB0 #Reception du nombre de tentative de com
ID_INITIALISATION_ARAL_FAITE    = 0xB1 #RIEN
ID_TEST_EN_COURS = 0xB2 #Reception du bilan de test du tours effectué
ID_TEST_TERMINEE = 0xB3 #Reception du bilan de test
ID_ETAT_VOIES = 0xB4 #Reception de l'etat des voies directement de la carte ARAL
ID_ETAT_UNE_VOIE = 0xB5 #Reception de l'etat d'une voie en defaut ou ok avec data[0]->(Num de la voie) et data[1]->(Etat, OK=0x30, Defaut = 0x10, non testée = 0)
ID_CARTE_ARAL_NE_REPOND_PLUS = 0xB6
ID_CARTE_ARAL_REPEAT_REQUEST = 0xB7

ID_ACK_GENERAL      = 0xC0
ID_RELANCER_TEST    = 0xC1
ID_ARRET_TEST       = 0xC2
ID_REPEAT_REQUEST   = 0xD0
ID_REQUEST_NB_TOURS_FAIT = 0xD1
ID_ACK_REQUEST_NB_TOURS_FAIT = 0xD2
ID_REQUEST_BILAN = 0xD3

class Message():
    def __init__(self, id=0, length=0, data=None, checksum=0):
        self.id = id
        self.len = length
        self.data = data if data else []
        self.checksum = checksum

    def build_packet(self):
        # Calculate checksum as a simple example
        self.checksum = (self.id ^ self.len) & 0xFF
        for i in range(self.len):
            self.checksum ^= self.data[i]
        length = self.len if(self.len) else 1
        # Construct the packet with start marker, ID, length, data, checksum, and end marker
        packet_format = f'<B B B {length}s B B'
        packet_data = bytes(self.data)
        return struct.pack(packet_format, 0xFF, self.id, self.len, packet_data, self.checksum, 0xFF)

SIZE_FIFO = 32
class COMMUNICATION():
    def __init__(self):
        self.rxMsg = [Message() for _ in range(SIZE_FIFO)]
        self.FIFO_Ecriture = 0
        self.serial_thread = None
        self.ecritureEnCours = False #Flag pour faire savoir qu'on a lancé une ecriture
        self.problemeEnEcriture = False #Flag pour dire que le serial.write n'a pas fonctionné
com = COMMUNICATION()

NOMBRE_VOIES = 96

etatBilan = {
    "test non fait" : 0,
    "OK" : 0x30,
    "DEFAUT" : 0x10,
}
etatVoies = {
    "COURT_CIRCUIT" : 0,
    "ALARME" : 1,
    "NORMAL" : 2,
    "CONGRUENCE" : 3,
}
class VOIE():
    def __init__(self):
        self.voies = [etatVoies["CONGRUENCE"] for _ in range(NOMBRE_VOIES)]
        self.bilan = [etatBilan["test non fait"] for _ in range(NOMBRE_VOIES)]
        self.perteDeCom = 0
voies = VOIE()

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
        byte = int.from_bytes(message)
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
        self.ui.sendButton_nbTours.clicked.connect(self.sendNbTours)
        self.ui.sendButton_repriseTest.clicked.connect(self.sendRelancerTest)
        self.ui.sendButton_arret.clicked.connect(self.sendArretTest)

        self.dialog = Dialog()
        self.dialog.ui.buttonBox.accepted.connect(self.start_serial)

        self.bilan_window = BilanWindow()
        self.state_window = StateWindow()
        self.fiche_validation = FicheValidation()
        self.ui.actionTableau_Voies_Bilan.triggered.connect(self.openBilanWindow)
        self.ui.actionTableau_Voies_en_Cours.triggered.connect(self.openStateWindow)
        self.ui.actionFicheValidation.triggered.connect(self.openFicheValidation)

        self.ui.actionQuit.triggered.connect(self.QuitWindows)
        self.ui.actionClearLog.triggered.connect(self.textPanel_Clear)
        self.ui.actionConnect.triggered.connect(self.openDialogWindow)
        self.ui.actionDisconnect.triggered.connect(self.closeSerial)
        self.ui.actionReset.triggered.connect(self.resetStruct)

        self.ui.sendButton_lancementTestNuit.clicked.connect(self.send128Tours)
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
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_ACK_NB_TOURS : Banc de test à bien reçu le nombre de tours à faire")
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
                        self.ui.textEdit_panel.append(f"")
                        self.ui.textEdit_panel.append(f"ID_ETAT_UNE_VOIE : Voie n°"+ str(numVoie) +" en DEFAUT")
            case 0xB6:#ID_CARTE_ARAL_NE_REPOND_PLUS
                voies.perteDeCom += 1
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_CARTE_ARAL_NE_REPOND_PLUS : carte ARAL ne répond plus !! Defaut COM!")
            case 0xB7:#ID_CARTE_ARAL_REPEAT_REQUEST
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_CARTE_ARAL_REPEAT_REQUEST : la carte aral demande de repeter le message")
            case 0xC0:#ID_ACK_GENERAL
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_ACK_GENERAL : reponse gen")
            case 0xD0:#ID_REPEAT_REQUEST
                self.ui.textEdit_panel.append(f"")
                self.ui.textEdit_panel.append(f"ID_REPEAT_REQUEST : le banc de test n'a pas compris, message incohérent")
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
    
    def send128Tours(self):
        print("send128Tours called")
        self.sendRelancerTest()
        nbTours = 128
        self.sendTwoBytes(ID_NB_TOURS, nbTours)

    def openBilanWindow(self):
        self.sendEmpty(ID_REQUEST_BILAN)
        self.bilan_window.show()
        self.bilan_window.raise_()
        self.bilan_window.activateWindow()
    def openStateWindow(self):
        self.state_window.show()
        self.state_window.raise_()
        self.state_window.activateWindow()
    def openDialogWindow(self):
        self.dialog.show()
        self.dialog.raise_()
        self.dialog.activateWindow()
    def openFicheValidation(self):
        self.fiche_validation.show()
        self.fiche_validation.raise_()
        self.fiche_validation.activateWindow()
    
    def textPanel_Clear(self):
        self.ui.textEdit_panel.clear()
    
    def closeSerial(self):
        self.ui.textEdit_panel.append(f"")
        self.ui.textEdit_panel.append(f"-----------Deconnexion du PORT COM...")
        com.serial_thread.close()
    
    def resetStruct(self):
        voies.voies = [etatVoies["CONGRUENCE"] for _ in range(NOMBRE_VOIES)]
        voies.bilan = [etatBilan["test non fait"] for _ in range(NOMBRE_VOIES)]
        voies.perteDeCom = 0

    def QuitWindows(self):
        self.close()
        QApplication.quit()
        
    def closeEvent(self, event):
        print("Au revoir")
        self.dialog.close()
        self.state_window.close()
        self.bilan_window.close()
        self.fiche_validation.close()
        super().closeEvent(event)
#end MainWindow

class Dialog(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_Dialog()
        self.ui.setupUi(self)
        self.setWindowTitle("CHOIX PORT COM")
        # self.ui.buttonBox.accepted.connect(self.start_serial) #Fait dans la mainwindow
        self.ui.buttonBox.rejected.connect(self.reject)
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
    def __init__(self):
        super().__init__()
        self.ui = Ui_tableauVoies()
        self.ui.setupUi(self)
        self.update_states()
        self.setWindowTitle("Bilan de test")

    # @Slot(list)
    def update_states(self):
        for i in range(10):
            for j in range(10):
                index = i * 10 + j
                # print("j : " + str(j) + " i : " + str(i) + " index : " + str(index))
                if(index>=96):
                    return
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

class StateWindow(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_tableauVoies()
        self.ui.setupUi(self)
        self.update_states()
        self.setWindowTitle("Etat Voies en Cours, envoyé par la carte ARAL")

    # @Slot(list)
    def update_states(self):
        for i in range(10):
            for j in range(10):
                index = i * 10 + j
                # print("j : " + str(j) + " i : " + str(i) + " index : " + str(index))
                if(index>=96):
                    return
                item = QTableWidgetItem()
                
                if voies.voies[index] == etatVoies["COURT_CIRCUIT"]:
                    item.setText('Court-Circuit')
                    item.setBackground(QColor("darkCyan"))
                elif voies.voies[index] == etatVoies["ALARME"]:
                    item.setText('Alarme')
                    item.setBackground(QColor("darkMagenta"))
                elif voies.voies[index] == etatVoies["NORMAL"]:
                    item.setText('Normal')
                    item.setBackground(QColor("green"))
                elif voies.voies[index] == etatVoies["CONGRUENCE"]:
                    item.setText('Congruence')
                    item.setBackground(QColor("gray"))
                


                self.ui.tableWidget.setItem(i, j, item)

class donneesFiche():
    def __init__(self):
        self.numSerie = ""
        self.controleurTechnique = ""
        self.controleurExterne = ""
        self.commentaires = generatePDF.CommentaireCarteFonctionnelle
donnees = donneesFiche()

class FicheValidation(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_FicheValidation()
        self.ui.setupUi(self)
        self.setWindowTitle("Etat Voies en Cours, envoyé par la carte ARAL")
        print("Fiche Validation")
        self.ui.buttonBox.accepted.connect(self.createPDFFicheValidation)
        self.ui.buttonBox.rejected.connect(self.reject)
        self.ui.pushButton_controleur_technique_ajout.clicked.connect(self.addControleurTechnique)
        self.ui.pushButton_controleur_externe_ajout.clicked.connect(self.addControleurExterne)
        self.ui.pushButton_num_serie_generer_auto.clicked.connect(self.genererNumSerie)
        self.init_controleurs_comboBox()

        self.ui.lineEdit_num_serie.setText(donnees.numSerie)

    def init_controleurs_comboBox(self):
        self.ui.comboBox_controleur_technique.clear()
        self.ui.comboBox_controleur_externe.clear()
        try:
            with open('PDF/controleur_technique.json', "r") as file:
                items = json.load(file)
                print("Loading controleur technique ", items)
                self.ui.comboBox_controleur_technique.addItems(items)
        except (FileNotFoundError, json.JSONDecodeError):
            print("PDF/controleur_technique.json error or not found")
            # pass  # No items to load or file is empty
        try:
            with open('PDF/controleur_externe.json', "r") as file:
                items = json.load(file)
                print("Loading controleur externe ", items)
                self.ui.comboBox_controleur_externe.addItems(items)
        except (FileNotFoundError, json.JSONDecodeError):
            print("PDF/controleur_externe.json error or not found")
            # pass  # No items to load or file is empty

    def genererNumSerie(self):
        donnees.numSerie = generatePDF.generer_numero_serie("172")
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
                        problemes.append(f"Voie {debut + 1}: {etat_str}")
                    else:
                        problemes.append(f"Voies {debut + 1} à {fin + 1}: {etat_str}")
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
        donnees.commentaires = self.getCommentaires()
        fiche = generatePDF.FicheValidation()
        output = fiche.generateFicheValidation(donnees.numSerie, donnees.controleurTechnique, donnees.controleurExterne, donnees.commentaires)
        fiche.writePDF(output) 
        self.accept()
    
    def addControleurTechnique(self):
        donnees.numSerie = self.ui.lineEdit_num_serie.text()
        self.ajoutTech = AjoutControleurTechnique()
        self.ajoutTech.show()
        self.reject()

    def addControleurExterne(self):
        donnees.numSerie = self.ui.lineEdit_num_serie.text()
        self.ajoutExt = AjoutControleurExterne()
        self.ajoutExt.show()
        self.reject()


class AjoutControleurTechnique(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_AjoutControleur()
        self.ui.setupUi(self)
        self.setWindowTitle("Ajout Controleur Technique")
        self.ui.buttonBox.accepted.connect(self.controleurAccept)
        self.ui.buttonBox.rejected.connect(self.controleurRejected)
    
    def controleurAccept(self):
        controleur = self.ui.lineEdit.text()
        generatePDF.add_items_to_json('PDF/controleur_technique.json', controleur)
        self.fiche = FicheValidation()
        self.fiche.show()
        self.accept()

    def controleurRejected(self):
        self.fiche = FicheValidation()
        self.fiche.show()
        self.reject()

class AjoutControleurExterne(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_AjoutControleur()
        self.ui.setupUi(self)
        self.setWindowTitle("Ajout Controleur Externe")
        self.ui.buttonBox.accepted.connect(self.controleurAccept)
        self.ui.buttonBox.rejected.connect(self.controleurRejected)
    
    def controleurAccept(self):
        controleur = self.ui.lineEdit.text()
        generatePDF.add_items_to_json('PDF/controleur_externe.json', controleur)
        self.fiche = FicheValidation()
        self.fiche.show()
        self.accept()

    def controleurRejected(self):
        self.fiche = FicheValidation()
        self.fiche.show()
        self.reject()

def main():
    app = QApplication([]) 
    main_window = MainWindow()
    main_window.show()

    main_window.dialog.show()


    sys.exit(app.exec())

if __name__ == '__main__':
    main()