#include <Arduino.h>
#include <CreateurTension.h>
#include <SelectionDeLaVoie.h>
#include <CommunicationARAL.h>
#include <TIMEMS.h>

CreateurTensionBUS busA = (CreateurTensionBUS){4,   5};
CreateurTensionBUS busB = (CreateurTensionBUS){13, 14};
CreateurTension Alarme(busA, busB);

SelectionDeLaVoieBUS bus = (SelectionDeLaVoieBUS){18, 19, 21};
SelectionDeLaVoieENABLEMUX enable = (SelectionDeLaVoieENABLEMUX){22, 23, 25, 26, 27, 32};
SelectionDeLaVoie Voie(bus, enable);

CommunicationARAL com;

TIMEMS timer;

void selectionVoie(Tension alarme, uint8_t voie = 1);
bool initialisationARAL();
void ControleParMoniteurSerie();
void setup() {
  Serial.begin(115200);
  selectionVoie(BOUCLE_OUVERTE);
  timer.begin();
  timer.start();
  
  com.begin(&Serial2, 9600);
  delay(100);

  com.sendMsg(ID_RESET);
}

void loop() {
  com.RxManage();
  ControleParMoniteurSerie();
  initialisationARAL();
}

void selectionVoie(Tension alarme, uint8_t voie){//de 1 à 96
  if(voie > 96 || voie==0){
    return;
  }
  
  Voie.disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
  Alarme.creationTensionVoie_1_48(COURT_CIRCUIT);
  Alarme.creationTensionVoie_49_96(COURT_CIRCUIT);

  if(alarme == BOUCLE_OUVERTE){
    return;
  }

  if(voie>48){
    Alarme.creationTensionVoie_49_96(alarme);
  }else{
    Alarme.creationTensionVoie_1_48(alarme);
  }

  Voie.selectionVoie(voie);
}

bool initialisationARAL(){
  static const char RESET = 0, PREMIERE_SCRUTATION = 1, CHECK_CAPTEURS = 2, DIFINITIVE_SCRUTATION = 3, ATT_ACK = 4, FINISH = 5;
  static const char etat_init[4] = {RESET, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION}, nbInstructions = 4;
  static char etat = etat_init[0], etape = 1, etat_suiv = 0, nb_repetition = 0;

  switch (etat)
  {
    case RESET:{
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_RESET;
      com.sendMsg(ID_RESET);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      timer.start();
    }
    break;
    case PREMIERE_SCRUTATION:{
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_PREMIERE_SCRUTATION;
      com.sendMsg(ID_PREMIERE_SCRUTATION);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      timer.start();
    }
    break;
    case CHECK_CAPTEURS:{
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_CHECK_CAPTEURS;
      com.sendMsg(ID_CHECK_CAPTEURS);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      timer.start();
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION;
      com.sendMsg(ID_DIFINITIVE_SCRUTATION);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      timer.start();
    }
    break;
    case ATT_ACK:{
      if(com.checkACK()){
        Serial.println("ACK reçu ! ");
        etat = etat_suiv;
        nb_repetition = 0;
        etape ++;
        if(etape > nbInstructions){
          etat = FINISH;
          return true;
        }
      }
      else{
        if(nb_repetition>=2){
          Serial.println("TIMEOUT ACK REPETITION 3 FOIS");
          etat = 55;
          // com.ACK.AckFrom_FLAG = true;//Pour les tests sans la carte ARAL
        }
        else if(timer.read()>=TIMEOUT_ACK){
          etat = etat_init[etape-1];
          nb_repetition++;
        }
      }
      
    }
    break;
    case FINISH:{
      return true;
    }
    break;
    default:
      break;
  }

  return false;
}

void ControleParMoniteurSerie(){
  static char instructionBuffer[20]; 
  static int etat = 0, nb_car = 0, init = true;
  static uint8_t voie = 0;
  
  switch (etat)
  {
    case 0:
    {
      if(init){
        init = false;
        Voie.disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
        Alarme.creationTensionVoie_1_48(COURT_CIRCUIT);
        Alarme.creationTensionVoie_49_96(COURT_CIRCUIT);
        Serial.println(".");
        Serial.println("Ecrire la voie voulue puis tapez sur entrée");
        Serial.println("Deux chiffres significatif !!");
        Serial.println("En chiffre, par exemple : 48 ou bien 07");
        Serial.println(".");
      }
      if(Serial.available()){
        char c = Serial.read(); Serial.print(c);
        if (c == '\n'){
          voie = (instructionBuffer[0] - '0')*10 + (instructionBuffer[1] - '0');
          init = true;
          etat = 1;
          nb_car = 0;
        }
        else{
          // Ajouter le caractère au tampon
          instructionBuffer[nb_car] = c;
          nb_car++;
        }
      }
    }
      break;
    case 1:
    {
      if(init){
        init = false;
        Serial.println(".");
        Serial.println("Ecrire l'alarme voulue puis tapez sur entrée");
        Serial.println("CC = COURT CIRCUIT");
        Serial.println("AL = ALARME");
        Serial.println("NO = NORMAL");
        Serial.println("CG = COONGRUENCE");
        Serial.println(".");
      }
      if(Serial.available()){
        char c = Serial.read(); Serial.print(c);
        if (c == '\n'){
          Tension tension = COURT_CIRCUIT;

          if(instructionBuffer[0]=='C' && instructionBuffer[1]=='C'){
            tension = COURT_CIRCUIT;
          }
          else if(instructionBuffer[0]=='A' && instructionBuffer[1]=='L'){
            tension = ALARME;
          }
          else if(instructionBuffer[0]=='N' && instructionBuffer[1]=='O'){
            tension = NORMAL;
          }
          else if(instructionBuffer[0]=='C' && instructionBuffer[1]=='G'){
            tension = CONGRUENCE;
          }
          Serial.printf("selectionVoie(alarme = %d, voie = %d); \n", tension, voie);
          selectionVoie(tension, voie);
          
          init = true;
          etat = 2;
          nb_car = 0;
        }
        else{
          // Ajouter le caractère au tampon
          instructionBuffer[nb_car] = c;
          nb_car++;
        }
      }
    }
      break;
    case 2:
    {
      if(init){
        init = false;
        Serial.println(".");
        Serial.println("Pour recommencer tapez sur Entrée");
        Serial.println(".");
      }
      if(Serial.available()){
        char c = Serial.read(); Serial.print(c);
        if (c == '\n'){
          init = true;
          etat = 0;
        }
      }
    }
      break;
  
  default:
    break;
  }
}

