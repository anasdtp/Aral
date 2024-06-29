# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'ficheValidation.ui'
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
from PySide6.QtWidgets import (QAbstractButton, QApplication, QCheckBox, QComboBox,
    QDialog, QDialogButtonBox, QHBoxLayout, QLabel,
    QLineEdit, QPushButton, QSizePolicy, QTextEdit,
    QVBoxLayout, QWidget)

class Ui_FicheValidation(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(492, 394)
        self.verticalLayout = QVBoxLayout(Dialog)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.checkBox_prise_en_compte_test = QCheckBox(Dialog)
        self.checkBox_prise_en_compte_test.setObjectName(u"checkBox_prise_en_compte_test")
        self.checkBox_prise_en_compte_test.setEnabled(True)
        self.checkBox_prise_en_compte_test.setCursor(QCursor(Qt.PointingHandCursor))
        self.checkBox_prise_en_compte_test.setChecked(True)

        self.verticalLayout.addWidget(self.checkBox_prise_en_compte_test)

        self.textEdit = QTextEdit(Dialog)
        self.textEdit.setObjectName(u"textEdit")

        self.verticalLayout.addWidget(self.textEdit)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.label = QLabel(Dialog)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.lineEdit_num_serie = QLineEdit(Dialog)
        self.lineEdit_num_serie.setObjectName(u"lineEdit_num_serie")

        self.horizontalLayout.addWidget(self.lineEdit_num_serie)

        self.pushButton_num_serie_generer_auto = QPushButton(Dialog)
        self.pushButton_num_serie_generer_auto.setObjectName(u"pushButton_num_serie_generer_auto")

        self.horizontalLayout.addWidget(self.pushButton_num_serie_generer_auto)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.label_2 = QLabel(Dialog)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout_2.addWidget(self.label_2)

        self.comboBox_controleur_technique = QComboBox(Dialog)
        self.comboBox_controleur_technique.setObjectName(u"comboBox_controleur_technique")

        self.horizontalLayout_2.addWidget(self.comboBox_controleur_technique)


        self.verticalLayout.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.label_3 = QLabel(Dialog)
        self.label_3.setObjectName(u"label_3")

        self.horizontalLayout_3.addWidget(self.label_3)

        self.comboBox_controleur_externe = QComboBox(Dialog)
        self.comboBox_controleur_externe.setObjectName(u"comboBox_controleur_externe")

        self.horizontalLayout_3.addWidget(self.comboBox_controleur_externe)


        self.verticalLayout.addLayout(self.horizontalLayout_3)

        self.horizontalLayout_5 = QHBoxLayout()
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.pushButton_controleur_technique_ajout = QPushButton(Dialog)
        self.pushButton_controleur_technique_ajout.setObjectName(u"pushButton_controleur_technique_ajout")

        self.horizontalLayout_5.addWidget(self.pushButton_controleur_technique_ajout)

        self.pushButton_controleur_externe_ajout = QPushButton(Dialog)
        self.pushButton_controleur_externe_ajout.setObjectName(u"pushButton_controleur_externe_ajout")

        self.horizontalLayout_5.addWidget(self.pushButton_controleur_externe_ajout)


        self.verticalLayout.addLayout(self.horizontalLayout_5)

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
        self.checkBox_prise_en_compte_test.setText(QCoreApplication.translate("Dialog", u"Prendre en compte le test fait", None))
        self.label.setText(QCoreApplication.translate("Dialog", u"N\u00b0 de s\u00e9rie :", None))
        self.pushButton_num_serie_generer_auto.setText(QCoreApplication.translate("Dialog", u"G\u00e9n\u00e9rer automatiquement", None))
        self.label_2.setText(QCoreApplication.translate("Dialog", u"Controleur Technique :", None))
        self.label_3.setText(QCoreApplication.translate("Dialog", u"Controleur Externe :", None))
        self.pushButton_controleur_technique_ajout.setText(QCoreApplication.translate("Dialog", u"Ajout d'un controleur technique", None))
        self.pushButton_controleur_externe_ajout.setText(QCoreApplication.translate("Dialog", u"Ajout d'un controleur externe", None))
    # retranslateUi

