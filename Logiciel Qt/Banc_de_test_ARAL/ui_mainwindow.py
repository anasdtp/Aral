# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainwindow.ui'
##
## Created by: Qt User Interface Compiler version 6.7.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QLabel,
    QMainWindow, QMenuBar, QPushButton, QSizePolicy,
    QStatusBar, QTextEdit, QVBoxLayout, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(800, 384)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.label_4 = QLabel(self.centralwidget)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.label_4)

        self.textEdit_panel = QTextEdit(self.centralwidget)
        self.textEdit_panel.setObjectName(u"textEdit_panel")

        self.verticalLayout.addWidget(self.textEdit_panel)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_2 = QLabel(self.centralwidget)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout_2.addWidget(self.label_2)

        self.sendButton_nbTours_2 = QPushButton(self.centralwidget)
        self.sendButton_nbTours_2.setObjectName(u"sendButton_nbTours_2")

        self.horizontalLayout_2.addWidget(self.sendButton_nbTours_2)


        self.horizontalLayout_4.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.label_3 = QLabel(self.centralwidget)
        self.label_3.setObjectName(u"label_3")

        self.horizontalLayout_3.addWidget(self.label_3)

        self.sendButton_nbTours_3 = QPushButton(self.centralwidget)
        self.sendButton_nbTours_3.setObjectName(u"sendButton_nbTours_3")

        self.horizontalLayout_3.addWidget(self.sendButton_nbTours_3)


        self.horizontalLayout_4.addLayout(self.horizontalLayout_3)


        self.verticalLayout.addLayout(self.horizontalLayout_4)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label = QLabel(self.centralwidget)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.comboBox_nbTours = QComboBox(self.centralwidget)
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.addItem("")
        self.comboBox_nbTours.setObjectName(u"comboBox_nbTours")

        self.horizontalLayout.addWidget(self.comboBox_nbTours)

        self.sendButton_nbTours = QPushButton(self.centralwidget)
        self.sendButton_nbTours.setObjectName(u"sendButton_nbTours")

        self.horizontalLayout.addWidget(self.sendButton_nbTours)


        self.verticalLayout.addLayout(self.horizontalLayout)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 800, 25))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"Banc de Test Carte ARAL", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"ARRET / Affichage du Bilan", None))
        self.sendButton_nbTours_2.setText(QCoreApplication.translate("MainWindow", u"Send", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Commencer / Reprise du test en boucle", None))
        self.sendButton_nbTours_3.setText(QCoreApplication.translate("MainWindow", u"Send", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"Nombre de tours de tests", None))
        self.comboBox_nbTours.setItemText(0, QCoreApplication.translate("MainWindow", u"1", None))
        self.comboBox_nbTours.setItemText(1, QCoreApplication.translate("MainWindow", u"2", None))
        self.comboBox_nbTours.setItemText(2, QCoreApplication.translate("MainWindow", u"3", None))
        self.comboBox_nbTours.setItemText(3, QCoreApplication.translate("MainWindow", u"4", None))
        self.comboBox_nbTours.setItemText(4, QCoreApplication.translate("MainWindow", u"5", None))
        self.comboBox_nbTours.setItemText(5, QCoreApplication.translate("MainWindow", u"10", None))
        self.comboBox_nbTours.setItemText(6, QCoreApplication.translate("MainWindow", u"20", None))
        self.comboBox_nbTours.setItemText(7, QCoreApplication.translate("MainWindow", u"30", None))
        self.comboBox_nbTours.setItemText(8, QCoreApplication.translate("MainWindow", u"50", None))
        self.comboBox_nbTours.setItemText(9, QCoreApplication.translate("MainWindow", u"100", None))
        self.comboBox_nbTours.setItemText(10, QCoreApplication.translate("MainWindow", u"200", None))
        self.comboBox_nbTours.setItemText(11, QCoreApplication.translate("MainWindow", u"300", None))
        self.comboBox_nbTours.setItemText(12, QCoreApplication.translate("MainWindow", u"400", None))
        self.comboBox_nbTours.setItemText(13, QCoreApplication.translate("MainWindow", u"500", None))

        self.sendButton_nbTours.setText(QCoreApplication.translate("MainWindow", u"Send", None))
    # retranslateUi

