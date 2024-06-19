import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QTextEdit, QDialog, QVBoxLayout, QTableWidgetItem, QPushButton, QTableWidget
from PySide6.QtCore import QThread, Signal, Slot, QTimer
from PySide6.QtGui import QColor
import serial
import serial.tools.list_ports
import struct
# import time
from ui_dialog import Ui_Dialog
from ui_mainwindow import Ui_MainWindow
from ui_tableauVoies import Ui_tableauVoies

SERIAL_BAUDRATE = 921600


ID_NB_TOURS     = 0xA0 #On envoi le nombre de tours à faire sur 2 octets
ID_ACK_NB_TOURS = 0xA1 #RIEN

ID_INITIALISATION_ARAL_EN_COURS = 0xB0 #Reception du nombre de tentative de com
ID_INITIALISATION_ARAL_FAITE    = 0xB1 #RIEN
ID_TEST_EN_COURS = 0xB2 #Reception du bilan de test du tours effectué
ID_TEST_TERMINEE = 0xB3 #Reception du bilan de test
ID_ETAT_VOIES = 0xB4 #Reception de l'etat des voies directement de la carte ARAL

ID_ACK_GENERAL      = 0xC0
ID_RELANCER_TEST    = 0xC1
ID_ARRET_TEST       = 0xC2
ID_REPEAT_REQUEST   = 0xD0

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
        # Construct the packet with start marker, ID, length, data, checksum, and end marker
        packet_format = f'<B B B {self.len}s B B'
        packet_data = bytes(self.data)
        return struct.pack(packet_format, 0xFF, self.id, self.len, packet_data, self.checksum, 0xFF)

SIZE_FIFO = 32
class COMMUNICATION():
    def __init__(self):
        self.rxMsg = [Message() for _ in range(SIZE_FIFO)]
        self.FIFO_Ecriture = 0
        self.serial_thread = None
com = COMMUNICATION()

NOMBRE_VOIES = 96

etatBilan = {
    "none" : 0,
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
        self.bilan = [etatBilan["none"] for _ in range(NOMBRE_VOIES)]
voies = VOIE()

class SerialThread(QThread):
    # message_received = Signal(bytes)
    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial = serial.Serial(port, baudrate)
        self.running = True

        self.stateRx = 0
        self.compteurData = 0

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

    def close(self):
        self.running = False
        self.serial.close()

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

        self.state_window = StateWindow()
        self.ui.actionTableau_Voies_Bilan.triggered.connect(self.openStateWindow)
        print("Initialized MainWindow")

        # Configuration du QTimer
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.RxManage)
        self.timer.start(1)  # Déclenchement toutes les 1 ms
        self.FIFO_lecture = 0
        self.FIFO_occupation = 0
        self.FIFO_max_occupation = 0
        print("End Initialization MainWindow")

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
                self.ui.textEdit_panel.append(f"Received message from ID_NB_TOURS??? Bizarre")
                pass
            case 0xA1:#ID_ACK_NB_TOURS
                self.ui.textEdit_panel.append(f"ID_ACK_NB_TOURS : Banc de test à bien reçu le nombre de tours à faire")
            case 0xB0:#ID_INITIALISATION_ARAL_EN_COURS
                nbEssai = com.rxMsg[self.FIFO_lecture].data[0]
                self.ui.textEdit_panel.append(f"ID_INITIALISATION_ARAL_EN_COURS : initialisation carte aral... " + str(nbEssai))
            case 0xB1:#ID_INITIALISATION_ARAL_FAITE
                self.ui.textEdit_panel.append(f"ID_INITIALISATION_ARAL_FAITE : carte aral initialisée!!")
            case 0xB2:#ID_TEST_EN_COURS
                voies.bilan = com.rxMsg[self.FIFO_lecture].data
                self.ui.textEdit_panel.append(f"ID_TEST_EN_COURS : bilan : " + str(voies.bilan))
                self.state_window.update_states()
            case 0xB3:#ID_TEST_TERMINEE
                voies.bilan = com.rxMsg[self.FIFO_lecture].data
                self.ui.textEdit_panel.append(f"ID_TEST_TERMINEE : bilan : " + str(voies.bilan))
                self.state_window.update_states()
                self.state_window.show()
            case 0xB4:#ID_ETAT_VOIES
                voies.voies = com.rxMsg[self.FIFO_lecture].data
                self.ui.textEdit_panel.append(f"ID_ETAT_VOIES : etat voies : " + str(voies.bilan))
            case 0xC0:#ID_ACK_GENERAL
                self.ui.textEdit_panel.append(f"ID_ACK_GENERAL : reponse gen")
            case 0xD0:#ID_REPEAT_REQUEST
                self.ui.textEdit_panel.append(f"ID_REPEAT_REQUEST : le banc de test n'a pas compris, message incohérent")
            case _:
                self.ui.textEdit_panel.append(f"Received message from an unknown ID")

        self.FIFO_lecture = (self.FIFO_lecture + 1) % SIZE_FIFO

    def sendMsg(self, msg = Message()):
        # sample_message = Message(id=1, length=3, data=[0x01, 0x02, 0x03])
        packet = msg.build_packet()
        print(packet)
        com.serial_thread.serial.write(packet)

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

    def openStateWindow(self):
        self.state_window.show()
#end MainWindow

class Dialog(QDialog):
    def __init__(self):
        super().__init__()
        self.ui = Ui_Dialog()
        self.ui.setupUi(self)
        self.ui.buttonBox.accepted.connect(self.start_serial)
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

class StateWindow(QDialog):
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
                print("j : " + str(j) + " i : " + str(i) + " index : " + str(index))
                if(index>=96):
                    return
                item = QTableWidgetItem()
                if voies.voies[index] == etatBilan["OK"]:
                    item.setBackground(QColor("green"))
                else:
                    item.setBackground(QColor("red"))
                self.ui.tableWidget.setItem(i, j, item)

def main():
    app = QApplication([]) 
    main_window = MainWindow()
    main_window.show()

    dialog = Dialog()
    dialog.show()

    sys.exit(app.exec())

if __name__ == '__main__':
    main()