#include <Arduino.h>
#include <Commun.h>
#include <BluetoothSerial.h>
#include <CreateurTension.h>
#include <SelectionDeLaVoie.h>
#include <CommunicationARAL.h>
#include <CommunicationPC.h>
#include <IHM.h>
#include <General.h>

//On renseigne les 2 pins du bus 2 bits createur de tension. Si le bus est égale à 0x0, COURT_CIRCUIT, 0x1 ALARME, 0x2 NORMAL et 0x3 CONGRUENCE.
CreateurTensionBUS busA = (CreateurTensionBUS){4,   5};
CreateurTensionBUS busB = (CreateurTensionBUS){13, 14};
CreateurTension Alarme(busA, busB);//

//On renseigne les 3 pins du bus 3 bits qui permet de selectionner une voie parmi 8 d'un multiplexeur. 
SelectionDeLaVoieBUS bus = (SelectionDeLaVoieBUS){18, 19, 21};
//On renseigne les 6 pins enable connecter aux 12 multiplexeur et où chaque pin est connectée à 2 multiplexeur.
SelectionDeLaVoieENABLEMUX enable = (SelectionDeLaVoieENABLEMUX){22, 23, 25, 26, 27, 32};
SelectionDeLaVoie Voie(bus, enable);//

CommunicationARAL com;//

CommunicationPC pc;

General aral(&com, &pc, &Voie, &Alarme);

void setup() {
  // Serial.begin(921600);//Communication avec le PC
  pc.begin();
  //On initialise la communication série avec la carte ARAL à 2400 baud.
  //La fonction begin initialise ensuite une fonction d'interruption se declenchant à chaque réception d'un message.
  com.begin(&Serial2, 2400);

  aral.IHMBegin();
  TransitionDelay(3000);//On laisse la carte ARAL s'initialiser

  // com.sendMsg(ID_RESET);
  // while(!aral.ControleParMoniteurSerie());
}

void loop() {
  aral.run();
}