# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'selectionneur_voies.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QHBoxLayout, QPushButton,
    QSizePolicy, QSlider, QVBoxLayout, QWidget)

class Ui_Selectionneur_Voies(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(1356, 809)
        Dialog.setMinimumSize(QSize(1356, 809))
        Dialog.setStyleSheet(u"background-image: url(:/images/selectionneurVoies.png);")
        self.pushButton = QPushButton(Dialog)
        self.pushButton.setObjectName(u"pushButton")
        self.pushButton.setGeometry(QRect(9, 9, 1341, 191))
        self.pushButton.setMinimumSize(QSize(1341, 191))
        self.pushButton.setMaximumSize(QSize(16777215, 16777215))
        font = QFont()
        font.setPointSize(11)
        font.setBold(True)
        font.setUnderline(False)
        self.pushButton.setFont(font)
        self.pushButton.setStyleSheet(u"")
        self.layoutWidget = QWidget(Dialog)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(9, 196, 1341, 601))
        self.verticalLayout = QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.verticalSlider_1 = QSlider(self.layoutWidget)
        self.verticalSlider_1.setObjectName(u"verticalSlider_1")
        self.verticalSlider_1.setMaximum(3)
        self.verticalSlider_1.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_1)

        self.verticalSlider_2 = QSlider(self.layoutWidget)
        self.verticalSlider_2.setObjectName(u"verticalSlider_2")
        self.verticalSlider_2.setMaximum(3)
        self.verticalSlider_2.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_2)

        self.verticalSlider_3 = QSlider(self.layoutWidget)
        self.verticalSlider_3.setObjectName(u"verticalSlider_3")
        self.verticalSlider_3.setMaximum(3)
        self.verticalSlider_3.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_3)

        self.verticalSlider_4 = QSlider(self.layoutWidget)
        self.verticalSlider_4.setObjectName(u"verticalSlider_4")
        self.verticalSlider_4.setMaximum(3)
        self.verticalSlider_4.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_4)

        self.verticalSlider_5 = QSlider(self.layoutWidget)
        self.verticalSlider_5.setObjectName(u"verticalSlider_5")
        self.verticalSlider_5.setMaximum(3)
        self.verticalSlider_5.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_5)

        self.verticalSlider_6 = QSlider(self.layoutWidget)
        self.verticalSlider_6.setObjectName(u"verticalSlider_6")
        self.verticalSlider_6.setMaximum(3)
        self.verticalSlider_6.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_6)

        self.verticalSlider_7 = QSlider(self.layoutWidget)
        self.verticalSlider_7.setObjectName(u"verticalSlider_7")
        self.verticalSlider_7.setMaximum(3)
        self.verticalSlider_7.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_7)

        self.verticalSlider_8 = QSlider(self.layoutWidget)
        self.verticalSlider_8.setObjectName(u"verticalSlider_8")
        self.verticalSlider_8.setMaximum(3)
        self.verticalSlider_8.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_8)

        self.verticalSlider_9 = QSlider(self.layoutWidget)
        self.verticalSlider_9.setObjectName(u"verticalSlider_9")
        self.verticalSlider_9.setMaximum(3)
        self.verticalSlider_9.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_9)

        self.verticalSlider_10 = QSlider(self.layoutWidget)
        self.verticalSlider_10.setObjectName(u"verticalSlider_10")
        self.verticalSlider_10.setMaximum(3)
        self.verticalSlider_10.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_10)

        self.verticalSlider_11 = QSlider(self.layoutWidget)
        self.verticalSlider_11.setObjectName(u"verticalSlider_11")
        self.verticalSlider_11.setMaximum(3)
        self.verticalSlider_11.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_11)

        self.verticalSlider_12 = QSlider(self.layoutWidget)
        self.verticalSlider_12.setObjectName(u"verticalSlider_12")
        self.verticalSlider_12.setMaximum(3)
        self.verticalSlider_12.setOrientation(Qt.Vertical)

        self.horizontalLayout.addWidget(self.verticalSlider_12)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.verticalSlider_13 = QSlider(self.layoutWidget)
        self.verticalSlider_13.setObjectName(u"verticalSlider_13")
        self.verticalSlider_13.setMaximum(3)
        self.verticalSlider_13.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_13)

        self.verticalSlider_14 = QSlider(self.layoutWidget)
        self.verticalSlider_14.setObjectName(u"verticalSlider_14")
        self.verticalSlider_14.setMaximum(3)
        self.verticalSlider_14.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_14)

        self.verticalSlider_15 = QSlider(self.layoutWidget)
        self.verticalSlider_15.setObjectName(u"verticalSlider_15")
        self.verticalSlider_15.setMaximum(3)
        self.verticalSlider_15.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_15)

        self.verticalSlider_16 = QSlider(self.layoutWidget)
        self.verticalSlider_16.setObjectName(u"verticalSlider_16")
        self.verticalSlider_16.setMaximum(3)
        self.verticalSlider_16.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_16)

        self.verticalSlider_17 = QSlider(self.layoutWidget)
        self.verticalSlider_17.setObjectName(u"verticalSlider_17")
        self.verticalSlider_17.setMaximum(3)
        self.verticalSlider_17.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_17)

        self.verticalSlider_18 = QSlider(self.layoutWidget)
        self.verticalSlider_18.setObjectName(u"verticalSlider_18")
        self.verticalSlider_18.setMaximum(3)
        self.verticalSlider_18.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_18)

        self.verticalSlider_19 = QSlider(self.layoutWidget)
        self.verticalSlider_19.setObjectName(u"verticalSlider_19")
        self.verticalSlider_19.setMaximum(3)
        self.verticalSlider_19.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_19)

        self.verticalSlider_20 = QSlider(self.layoutWidget)
        self.verticalSlider_20.setObjectName(u"verticalSlider_20")
        self.verticalSlider_20.setMaximum(3)
        self.verticalSlider_20.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_20)

        self.verticalSlider_21 = QSlider(self.layoutWidget)
        self.verticalSlider_21.setObjectName(u"verticalSlider_21")
        self.verticalSlider_21.setMaximum(3)
        self.verticalSlider_21.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_21)

        self.verticalSlider_22 = QSlider(self.layoutWidget)
        self.verticalSlider_22.setObjectName(u"verticalSlider_22")
        self.verticalSlider_22.setMaximum(3)
        self.verticalSlider_22.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_22)

        self.verticalSlider_23 = QSlider(self.layoutWidget)
        self.verticalSlider_23.setObjectName(u"verticalSlider_23")
        self.verticalSlider_23.setMaximum(3)
        self.verticalSlider_23.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_23)

        self.verticalSlider_24 = QSlider(self.layoutWidget)
        self.verticalSlider_24.setObjectName(u"verticalSlider_24")
        self.verticalSlider_24.setMaximum(3)
        self.verticalSlider_24.setOrientation(Qt.Vertical)

        self.horizontalLayout_2.addWidget(self.verticalSlider_24)


        self.verticalLayout.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.verticalSlider_25 = QSlider(self.layoutWidget)
        self.verticalSlider_25.setObjectName(u"verticalSlider_25")
        self.verticalSlider_25.setMaximum(3)
        self.verticalSlider_25.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_25)

        self.verticalSlider_26 = QSlider(self.layoutWidget)
        self.verticalSlider_26.setObjectName(u"verticalSlider_26")
        self.verticalSlider_26.setMaximum(3)
        self.verticalSlider_26.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_26)

        self.verticalSlider_27 = QSlider(self.layoutWidget)
        self.verticalSlider_27.setObjectName(u"verticalSlider_27")
        self.verticalSlider_27.setMaximum(3)
        self.verticalSlider_27.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_27)

        self.verticalSlider_28 = QSlider(self.layoutWidget)
        self.verticalSlider_28.setObjectName(u"verticalSlider_28")
        self.verticalSlider_28.setMaximum(3)
        self.verticalSlider_28.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_28)

        self.verticalSlider_29 = QSlider(self.layoutWidget)
        self.verticalSlider_29.setObjectName(u"verticalSlider_29")
        self.verticalSlider_29.setMaximum(3)
        self.verticalSlider_29.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_29)

        self.verticalSlider_30 = QSlider(self.layoutWidget)
        self.verticalSlider_30.setObjectName(u"verticalSlider_30")
        self.verticalSlider_30.setMaximum(3)
        self.verticalSlider_30.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_30)

        self.verticalSlider_31 = QSlider(self.layoutWidget)
        self.verticalSlider_31.setObjectName(u"verticalSlider_31")
        self.verticalSlider_31.setMaximum(3)
        self.verticalSlider_31.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_31)

        self.verticalSlider_32 = QSlider(self.layoutWidget)
        self.verticalSlider_32.setObjectName(u"verticalSlider_32")
        self.verticalSlider_32.setMaximum(3)
        self.verticalSlider_32.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_32)

        self.verticalSlider_33 = QSlider(self.layoutWidget)
        self.verticalSlider_33.setObjectName(u"verticalSlider_33")
        self.verticalSlider_33.setMaximum(3)
        self.verticalSlider_33.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_33)

        self.verticalSlider_34 = QSlider(self.layoutWidget)
        self.verticalSlider_34.setObjectName(u"verticalSlider_34")
        self.verticalSlider_34.setMaximum(3)
        self.verticalSlider_34.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_34)

        self.verticalSlider_35 = QSlider(self.layoutWidget)
        self.verticalSlider_35.setObjectName(u"verticalSlider_35")
        self.verticalSlider_35.setMaximum(3)
        self.verticalSlider_35.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_35)

        self.verticalSlider_36 = QSlider(self.layoutWidget)
        self.verticalSlider_36.setObjectName(u"verticalSlider_36")
        self.verticalSlider_36.setMaximum(3)
        self.verticalSlider_36.setOrientation(Qt.Vertical)

        self.horizontalLayout_3.addWidget(self.verticalSlider_36)


        self.verticalLayout.addLayout(self.horizontalLayout_3)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.verticalSlider_37 = QSlider(self.layoutWidget)
        self.verticalSlider_37.setObjectName(u"verticalSlider_37")
        self.verticalSlider_37.setMaximum(3)
        self.verticalSlider_37.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_37)

        self.verticalSlider_38 = QSlider(self.layoutWidget)
        self.verticalSlider_38.setObjectName(u"verticalSlider_38")
        self.verticalSlider_38.setMaximum(3)
        self.verticalSlider_38.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_38)

        self.verticalSlider_39 = QSlider(self.layoutWidget)
        self.verticalSlider_39.setObjectName(u"verticalSlider_39")
        self.verticalSlider_39.setMaximum(3)
        self.verticalSlider_39.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_39)

        self.verticalSlider_40 = QSlider(self.layoutWidget)
        self.verticalSlider_40.setObjectName(u"verticalSlider_40")
        self.verticalSlider_40.setMaximum(3)
        self.verticalSlider_40.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_40)

        self.verticalSlider_41 = QSlider(self.layoutWidget)
        self.verticalSlider_41.setObjectName(u"verticalSlider_41")
        self.verticalSlider_41.setMaximum(3)
        self.verticalSlider_41.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_41)

        self.verticalSlider_42 = QSlider(self.layoutWidget)
        self.verticalSlider_42.setObjectName(u"verticalSlider_42")
        self.verticalSlider_42.setMaximum(3)
        self.verticalSlider_42.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_42)

        self.verticalSlider_43 = QSlider(self.layoutWidget)
        self.verticalSlider_43.setObjectName(u"verticalSlider_43")
        self.verticalSlider_43.setMaximum(3)
        self.verticalSlider_43.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_43)

        self.verticalSlider_44 = QSlider(self.layoutWidget)
        self.verticalSlider_44.setObjectName(u"verticalSlider_44")
        self.verticalSlider_44.setMaximum(3)
        self.verticalSlider_44.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_44)

        self.verticalSlider_45 = QSlider(self.layoutWidget)
        self.verticalSlider_45.setObjectName(u"verticalSlider_45")
        self.verticalSlider_45.setMaximum(3)
        self.verticalSlider_45.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_45)

        self.verticalSlider_46 = QSlider(self.layoutWidget)
        self.verticalSlider_46.setObjectName(u"verticalSlider_46")
        self.verticalSlider_46.setMaximum(3)
        self.verticalSlider_46.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_46)

        self.verticalSlider_47 = QSlider(self.layoutWidget)
        self.verticalSlider_47.setObjectName(u"verticalSlider_47")
        self.verticalSlider_47.setMaximum(3)
        self.verticalSlider_47.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_47)

        self.verticalSlider_48 = QSlider(self.layoutWidget)
        self.verticalSlider_48.setObjectName(u"verticalSlider_48")
        self.verticalSlider_48.setMaximum(3)
        self.verticalSlider_48.setOrientation(Qt.Vertical)

        self.horizontalLayout_4.addWidget(self.verticalSlider_48)


        self.verticalLayout.addLayout(self.horizontalLayout_4)


        self.retranslateUi(Dialog)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"Dialog", None))
        self.pushButton.setText(QCoreApplication.translate("Dialog", u"Appuyer sur ce bouton pour pouvoir selectionner les voies 49-96", None))
    # retranslateUi

