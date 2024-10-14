# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'historique.ui'
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
from PySide6.QtWidgets import (QAbstractButton, QApplication, QComboBox, QDialog,
    QDialogButtonBox, QHBoxLayout, QLabel, QPushButton,
    QSizePolicy, QTextEdit, QVBoxLayout, QWidget)

class Ui_Historique(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(337, 466)
        self.verticalLayout = QVBoxLayout(Dialog)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_num_serie = QLabel(Dialog)
        self.label_num_serie.setObjectName(u"label_num_serie")
        self.label_num_serie.setMaximumSize(QSize(70, 16777215))
        self.label_num_serie.setTextFormat(Qt.AutoText)

        self.horizontalLayout_2.addWidget(self.label_num_serie)

        self.comboBox_num_serie = QComboBox(Dialog)
        self.comboBox_num_serie.setObjectName(u"comboBox_num_serie")
        self.comboBox_num_serie.setEditable(True)

        self.horizontalLayout_2.addWidget(self.comboBox_num_serie)


        self.verticalLayout.addLayout(self.horizontalLayout_2)

        self.label_historique = QLabel(Dialog)
        self.label_historique.setObjectName(u"label_historique")

        self.verticalLayout.addWidget(self.label_historique)

        self.textEdit_historique = QTextEdit(Dialog)
        self.textEdit_historique.setObjectName(u"textEdit_historique")
        font = QFont()
        font.setFamilies([u"Arial"])
        font.setPointSize(12)
        font.setBold(False)
        self.textEdit_historique.setFont(font)
        self.textEdit_historique.setLineWidth(5)

        self.verticalLayout.addWidget(self.textEdit_historique)

        self.pushButton_nouvelle_panne = QPushButton(Dialog)
        self.pushButton_nouvelle_panne.setObjectName(u"pushButton_nouvelle_panne")

        self.verticalLayout.addWidget(self.pushButton_nouvelle_panne)

        self.buttonBox = QDialogButtonBox(Dialog)
        self.buttonBox.setObjectName(u"buttonBox")
        self.buttonBox.setOrientation(Qt.Horizontal)
        self.buttonBox.setStandardButtons(QDialogButtonBox.Cancel|QDialogButtonBox.Ok)

        self.verticalLayout.addWidget(self.buttonBox)


        self.retranslateUi(Dialog)
        self.buttonBox.accepted.connect(Dialog.accept)
        self.buttonBox.rejected.connect(Dialog.reject)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"Dialog", None))
        self.label_num_serie.setText(QCoreApplication.translate("Dialog", u"N\u00b0 s\u00e9rie :", None))
        self.label_historique.setText(QCoreApplication.translate("Dialog", u"Historique :", None))
        self.pushButton_nouvelle_panne.setText(QCoreApplication.translate("Dialog", u"Cr\u00e9er nouvelle Fiche de panne", None))
    # retranslateUi

