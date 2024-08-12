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
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QLabel,
    QMainWindow, QMenu, QMenuBar, QPushButton,
    QSizePolicy, QSpacerItem, QStatusBar, QTextEdit,
    QToolBar, QVBoxLayout, QWidget)
import Ressources_rc

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1215, 751)
        self.actionTableau_Voies_Bilan = QAction(MainWindow)
        self.actionTableau_Voies_Bilan.setObjectName(u"actionTableau_Voies_Bilan")
        self.actionTableau_Voies_en_Cours = QAction(MainWindow)
        self.actionTableau_Voies_en_Cours.setObjectName(u"actionTableau_Voies_en_Cours")
        self.actionFicheValidation = QAction(MainWindow)
        self.actionFicheValidation.setObjectName(u"actionFicheValidation")
        self.actionConnect = QAction(MainWindow)
        self.actionConnect.setObjectName(u"actionConnect")
        icon = QIcon()
        icon.addFile(u":/images/connect.png", QSize(), QIcon.Normal, QIcon.Off)
        self.actionConnect.setIcon(icon)
        self.actionDisconnect = QAction(MainWindow)
        self.actionDisconnect.setObjectName(u"actionDisconnect")
        icon1 = QIcon()
        icon1.addFile(u":/images/disconnect.png", QSize(), QIcon.Normal, QIcon.Off)
        self.actionDisconnect.setIcon(icon1)
        self.actionClearLog = QAction(MainWindow)
        self.actionClearLog.setObjectName(u"actionClearLog")
        icon2 = QIcon()
        icon2.addFile(u":/images/clear.png", QSize(), QIcon.Normal, QIcon.Off)
        self.actionClearLog.setIcon(icon2)
        self.actionQuit = QAction(MainWindow)
        self.actionQuit.setObjectName(u"actionQuit")
        icon3 = QIcon()
        icon3.addFile(u":/images/application-exit.png", QSize(), QIcon.Normal, QIcon.Off)
        self.actionQuit.setIcon(icon3)
        self.actionReset = QAction(MainWindow)
        self.actionReset.setObjectName(u"actionReset")
        icon4 = QIcon()
        icon4.addFile(u":/images/refresh.png", QSize(), QIcon.Normal, QIcon.Off)
        self.actionReset.setIcon(icon4)
        self.actionactionSetMainWindowIcon = QAction(MainWindow)
        self.actionactionSetMainWindowIcon.setObjectName(u"actionactionSetMainWindowIcon")
        icon5 = QIcon()
        icon5.addFile(u":/images/logo.ico", QSize(), QIcon.Normal, QIcon.Off)
        self.actionactionSetMainWindowIcon.setIcon(icon5)
        self.action_reglage_du_switch_SW2 = QAction(MainWindow)
        self.action_reglage_du_switch_SW2.setObjectName(u"action_reglage_du_switch_SW2")
        icon6 = QIcon()
        icon6.addFile(u":/images/switch.png", QSize(), QIcon.Normal, QIcon.Off)
        self.action_reglage_du_switch_SW2.setIcon(icon6)
        self.actionHistorique = QAction(MainWindow)
        self.actionHistorique.setObjectName(u"actionHistorique")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.label_4 = QLabel(self.centralwidget)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setMaximumSize(QSize(16777215, 20))
        self.label_4.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.label_4)

        self.textEdit_panel = QTextEdit(self.centralwidget)
        self.textEdit_panel.setObjectName(u"textEdit_panel")
        self.textEdit_panel.setMinimumSize(QSize(0, 300))
        self.textEdit_panel.setMaximumSize(QSize(16777215, 400))

        self.verticalLayout.addWidget(self.textEdit_panel)

        self.verticalSpacer_2 = QSpacerItem(20, 5, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Minimum)

        self.verticalLayout.addItem(self.verticalSpacer_2)

        self.textEdit_panel_ACK = QTextEdit(self.centralwidget)
        self.textEdit_panel_ACK.setObjectName(u"textEdit_panel_ACK")

        self.verticalLayout.addWidget(self.textEdit_panel_ACK)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_2 = QLabel(self.centralwidget)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout_2.addWidget(self.label_2)

        self.sendButton_arret = QPushButton(self.centralwidget)
        self.sendButton_arret.setObjectName(u"sendButton_arret")
        self.sendButton_arret.setMinimumSize(QSize(0, 30))

        self.horizontalLayout_2.addWidget(self.sendButton_arret)


        self.horizontalLayout_4.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.label_3 = QLabel(self.centralwidget)
        self.label_3.setObjectName(u"label_3")

        self.horizontalLayout_3.addWidget(self.label_3)

        self.sendButton_repriseTest = QPushButton(self.centralwidget)
        self.sendButton_repriseTest.setObjectName(u"sendButton_repriseTest")
        self.sendButton_repriseTest.setMinimumSize(QSize(0, 30))

        self.horizontalLayout_3.addWidget(self.sendButton_repriseTest)


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

        self.sendButton_lancementTestNuit = QPushButton(self.centralwidget)
        self.sendButton_lancementTestNuit.setObjectName(u"sendButton_lancementTestNuit")
        self.sendButton_lancementTestNuit.setMinimumSize(QSize(0, 40))
        font = QFont()
        font.setBold(True)
        font.setUnderline(False)
        font.setStrikeOut(False)
        self.sendButton_lancementTestNuit.setFont(font)
        self.sendButton_lancementTestNuit.setCursor(QCursor(Qt.PointingHandCursor))
        self.sendButton_lancementTestNuit.setFocusPolicy(Qt.StrongFocus)
        self.sendButton_lancementTestNuit.setCheckable(False)

        self.verticalLayout.addWidget(self.sendButton_lancementTestNuit)

        self.verticalSpacer_3 = QSpacerItem(20, 5, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Minimum)

        self.verticalLayout.addItem(self.verticalSpacer_3)

        self.sendButton_activer_filtrage = QPushButton(self.centralwidget)
        self.sendButton_activer_filtrage.setObjectName(u"sendButton_activer_filtrage")
        self.sendButton_activer_filtrage.setMinimumSize(QSize(0, 30))
        font1 = QFont()
        font1.setBold(True)
        font1.setUnderline(False)
        font1.setStrikeOut(False)
        font1.setKerning(True)
        self.sendButton_activer_filtrage.setFont(font1)
        self.sendButton_activer_filtrage.setCursor(QCursor(Qt.CrossCursor))
        self.sendButton_activer_filtrage.setMouseTracking(False)
        self.sendButton_activer_filtrage.setStyleSheet(u"")

        self.verticalLayout.addWidget(self.sendButton_activer_filtrage)

        self.sendButton_reglage_nb_etat_en_test = QPushButton(self.centralwidget)
        self.sendButton_reglage_nb_etat_en_test.setObjectName(u"sendButton_reglage_nb_etat_en_test")
        self.sendButton_reglage_nb_etat_en_test.setMinimumSize(QSize(0, 30))
        self.sendButton_reglage_nb_etat_en_test.setFont(font1)
        self.sendButton_reglage_nb_etat_en_test.setCursor(QCursor(Qt.CrossCursor))
        self.sendButton_reglage_nb_etat_en_test.setMouseTracking(False)
        self.sendButton_reglage_nb_etat_en_test.setStyleSheet(u"")

        self.verticalLayout.addWidget(self.sendButton_reglage_nb_etat_en_test)

        self.verticalSpacer = QSpacerItem(10, 5, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.mainToolBar = QToolBar(MainWindow)
        self.mainToolBar.setObjectName(u"mainToolBar")
        self.mainToolBar.setMinimumSize(QSize(0, 40))
        MainWindow.addToolBar(Qt.ToolBarArea.TopToolBarArea, self.mainToolBar)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1215, 22))
        self.menuFichiers = QMenu(self.menubar)
        self.menuFichiers.setObjectName(u"menuFichiers")
        self.menuCreation_PDF = QMenu(self.menubar)
        self.menuCreation_PDF.setObjectName(u"menuCreation_PDF")
        self.menuPort_COM = QMenu(self.menubar)
        self.menuPort_COM.setObjectName(u"menuPort_COM")
        MainWindow.setMenuBar(self.menubar)

        self.mainToolBar.addAction(self.actionConnect)
        self.mainToolBar.addSeparator()
        self.mainToolBar.addAction(self.actionDisconnect)
        self.mainToolBar.addAction(self.actionClearLog)
        self.mainToolBar.addSeparator()
        self.mainToolBar.addAction(self.actionReset)
        self.mainToolBar.addSeparator()
        self.mainToolBar.addAction(self.actionTableau_Voies_en_Cours)
        self.mainToolBar.addSeparator()
        self.mainToolBar.addAction(self.actionFicheValidation)
        self.mainToolBar.addSeparator()
        self.mainToolBar.addAction(self.actionHistorique)
        self.menubar.addAction(self.menuFichiers.menuAction())
        self.menubar.addAction(self.menuPort_COM.menuAction())
        self.menubar.addAction(self.menuCreation_PDF.menuAction())
        self.menuFichiers.addAction(self.actionTableau_Voies_en_Cours)
        self.menuFichiers.addAction(self.actionTableau_Voies_Bilan)
        self.menuFichiers.addSeparator()
        self.menuFichiers.addAction(self.actionQuit)
        self.menuCreation_PDF.addAction(self.actionFicheValidation)
        self.menuPort_COM.addAction(self.actionConnect)
        self.menuPort_COM.addAction(self.actionDisconnect)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.actionTableau_Voies_Bilan.setText(QCoreApplication.translate("MainWindow", u"Tableau Voies Bilan", None))
        self.actionTableau_Voies_en_Cours.setText(QCoreApplication.translate("MainWindow", u"Tableau Voies en Cours", None))
        self.actionFicheValidation.setText(QCoreApplication.translate("MainWindow", u"Fiche de Validation", None))
        self.actionConnect.setText(QCoreApplication.translate("MainWindow", u"&Connect", None))
        self.actionDisconnect.setText(QCoreApplication.translate("MainWindow", u"&Disconnect", None))
        self.actionClearLog.setText(QCoreApplication.translate("MainWindow", u"Clear &Log", None))
        self.actionQuit.setText(QCoreApplication.translate("MainWindow", u"&Quit", None))
        self.actionReset.setText(QCoreApplication.translate("MainWindow", u"Reset", None))
        self.actionactionSetMainWindowIcon.setText(QCoreApplication.translate("MainWindow", u"actionSetMainWindowIcon", None))
        self.action_reglage_du_switch_SW2.setText(QCoreApplication.translate("MainWindow", u"R\u00e9glage du switch SW2", None))
#if QT_CONFIG(tooltip)
        self.action_reglage_du_switch_SW2.setToolTip(QCoreApplication.translate("MainWindow", u"switch_SW2", None))
#endif // QT_CONFIG(tooltip)
        self.actionHistorique.setText(QCoreApplication.translate("MainWindow", u"Historique de test", None))
#if QT_CONFIG(tooltip)
        self.actionHistorique.setToolTip(QCoreApplication.translate("MainWindow", u"Historique de test", None))
#endif // QT_CONFIG(tooltip)
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"Banc de Test Carte ARAL", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"ARRET / Affichage du Bilan", None))
        self.sendButton_arret.setText(QCoreApplication.translate("MainWindow", u"Send", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Commencer / Reprise du test en boucle", None))
        self.sendButton_repriseTest.setText(QCoreApplication.translate("MainWindow", u"Send", None))
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
        self.sendButton_lancementTestNuit.setText(QCoreApplication.translate("MainWindow", u"LANCER UN TEST DE 8H", None))
        self.sendButton_activer_filtrage.setText(QCoreApplication.translate("MainWindow", u"Activer Filtrage", None))
        self.sendButton_reglage_nb_etat_en_test.setText(QCoreApplication.translate("MainWindow", u"Mettre le mode avec 2 \u00e9tats, Court-circuit et Alarme", None))
        self.mainToolBar.setWindowTitle(QCoreApplication.translate("MainWindow", u"toolBar", None))
        self.menuFichiers.setTitle(QCoreApplication.translate("MainWindow", u"Fichiers", None))
        self.menuCreation_PDF.setTitle(QCoreApplication.translate("MainWindow", u"Creation PDF", None))
        self.menuPort_COM.setTitle(QCoreApplication.translate("MainWindow", u"Port COM", None))
    # retranslateUi

