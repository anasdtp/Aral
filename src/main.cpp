#include <Arduino.h>
#include <CreateurTension.h>
#include <SelectionDeLaVoie.h>
#include <CommunicationARAL.h>

CreateurTensionBUS busA = (CreateurTensionBUS){4,   5};
CreateurTensionBUS busB = (CreateurTensionBUS){13, 14};
CreateurTension Alarme(busA, busB);

SelectionDeLaVoieBUS bus = (SelectionDeLaVoieBUS){18, 19, 21};
SelectionDeLaVoieENABLEMUX enable = (SelectionDeLaVoieENABLEMUX){22, 23, 25, 26, 27, 32};
SelectionDeLaVoie Voie(bus, enable);

CommunicationARAL com(&Serial2, 115200);

void selectionVoie(Tension alarme, uint8_t voie);
void ControleParMoniteurSerie();
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Hello World");
  
  selectionVoie(ALARME, 1);
}

void loop() {
  ControleParMoniteurSerie();
}

void selectionVoie(Tension alarme, uint8_t voie){//de 1 à 96
  if(voie > 96 || voie==0){
    return;
  }
  Alarme.creationTensionVoie_1_48(COURT_CIRCUIT);//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
  Alarme.creationTensionVoie_49_96(COURT_CIRCUIT);

  Voie.selectionVoie(voie);

  if(voie>48){
    Alarme.creationTensionVoie_49_96(alarme);
  }else{
    Alarme.creationTensionVoie_1_48(alarme);
  }
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
        Alarme.creationTensionVoie_1_48(COURT_CIRCUIT);//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
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