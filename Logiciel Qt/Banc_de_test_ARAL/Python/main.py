import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QTextEdit, QPushButton
from PyQt5.QtCore import QThread, pyqtSignal
import serial
import serial.tools.list_ports

class SerialThread(QThread):
    message_received = pyqtSignal(bytes)

    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial = serial.Serial(port, baudrate)
        self.running = True

    def run(self):
        while self.running:
            if self.serial.in_waiting > 0:
                data = self.serial.read(1)
                self.message_received.emit(data)
                self.serial.write(b'\xFF')

    def close(self):
        self.running = False
        self.serial.close()

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.serial_thread = None

    def initUI(self):
        self.layout = QVBoxLayout()

        self.text_display = QTextEdit()
        self.text_display.setReadOnly(True)
        self.layout.addWidget(self.text_display)

        self.start_button = QPushButton('Start')
        self.start_button.clicked.connect(self.start_serial)
        self.layout.addWidget(self.start_button)

        self.setLayout(self.layout)
        self.setWindowTitle('ESP32 UART Communication')

    def start_serial(self):
        available_ports = list_serial_ports()
        if available_ports:
            self.serial_thread = SerialThread(available_ports[0], 921600)
            self.serial_thread.message_received.connect(self.display_message)
            self.serial_thread.start()
            self.start_button.setEnabled(False)
        else:
            self.text_display.append("No serial ports available.")

    def display_message(self, message):
        self.text_display.append(f"Received: {message.hex()}")

    def closeEvent(self, event):
        if self.serial_thread:
            self.serial_thread.close()
        event.accept()

def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

if __name__ == '__main__':
    app = QApplication(sys.argv)
    mainWin = MainWindow()
    mainWin.show()
    sys.exit(app.exec_())
