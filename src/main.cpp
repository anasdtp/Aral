#include <Arduino.h>
#include <CreateurTension.h>
#include <SelectionDeLaVoie.h>
#include <CommunicationARAL.h>

typedef struct EtatVoies{
    Tension voies[96];
}EtatVoies;

//On renseigne les 2 pins du bus 2 bits createur de tension. Si le bus est égale à 0x0, COURT_CIRCUIT, 0x1 ALARME, 0x2 NORMAL et 0x3 CONGRUENCE.
CreateurTensionBUS busA = (CreateurTensionBUS){4,   5};
CreateurTensionBUS busB = (CreateurTensionBUS){13, 14};
CreateurTension Alarme(busA, busB);

//On renseigne les 3 pins du bus 3 bits qui permet de selectionner une voie parmi 8 d'un multiplexeur. 
SelectionDeLaVoieBUS bus = (SelectionDeLaVoieBUS){18, 19, 21};
//On renseigne les 6 pins enable connecter aux 12 multiplexeur et où chaque pin est connectée à 2 multiplexeur.
SelectionDeLaVoieENABLEMUX enable = (SelectionDeLaVoieENABLEMUX){22, 23, 25, 26, 27, 32};
SelectionDeLaVoie Voie(bus, enable);

CommunicationARAL com;


void selectionVoie(Tension alarme, uint8_t voie = 1);
void txLoop();
bool initialisationARAL();
void TestComCarteARAL();
void TestContinueVoiesBoucleOuverte();
bool ControleParMoniteurSerie();
void setup() {
  Serial.begin(921600);

  //On initialise la communication série avec la carte ARAL à 2400 baud.
  //La fonction begin initialise ensuite une fonction d'interruption se declenchant à chaque réception d'un message.
  com.begin(&Serial2, 2400);
  delay(500);

  // com.sendMsg(ID_RESET);
  // while(!ControleParMoniteurSerie());
}

void loop() {
  //Fonction qui gére en parrallele les données triées prealablement 
  com.RxManage();
  // initialisationARAL();
  // txLoop();

  TestContinueVoiesBoucleOuverte();
  
}

void selectionVoie(Tension alarme, uint8_t voie){//de 1 à 96
  if(voie > 96 || voie==0){
    return;
  }
  
  // Voie.disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
  // Alarme.creationTensionVoie_1_48(COURT_CIRCUIT);
  // Alarme.creationTensionVoie_49_96(COURT_CIRCUIT);

  if(alarme == BOUCLE_OUVERTE){
    Voie.disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
    Alarme.creationTensionVoie_1_48(COURT_CIRCUIT);
    Alarme.creationTensionVoie_49_96(COURT_CIRCUIT);
    return;
  }

  if(voie>48){
    Alarme.creationTensionVoie_49_96(alarme);
  }else{
    Alarme.creationTensionVoie_1_48(alarme);
  }

  Voie.selectionVoie(voie);
}

void getTension(EtatVoies &voies){
  Serial.print("get Tension des 96 voies : Voies[96] = ");
  for (int numVoie = 0; numVoie < 96; numVoie++)
  {
    if(com.etatVoies.voies[numVoie] == ETAT_COURT_CIRCUIT){
      voies.voies[numVoie] = COURT_CIRCUIT;
    }
    else if(com.etatVoies.voies[numVoie] == ETAT_ALARME){
      voies.voies[numVoie] = ALARME;
    }
    else if(com.etatVoies.voies[numVoie] == ETAT_NORMAL){
      voies.voies[numVoie] = NORMAL;
    }
    else if(com.etatVoies.voies[numVoie] == ETAT_CONGRUENCE){
      voies.voies[numVoie] = CONGRUENCE;
    }
    Serial.printf(" [%d]", voies.voies[numVoie]);
  }
  Serial.println("");
}

void txLoop(){
  bool initDone = false;
  if(initialisationARAL() && !initDone){
    initDone = true;
  }
  if(initDone){
    TestComCarteARAL();
  }
}

bool initialisationARAL(){
  static const char RESET = 0, PREMIERE_SCRUTATION = 1, CHECK_CAPTEURS = 2, DIFINITIVE_SCRUTATION = 3, ATT_ACK = 4, FINISH = 5;
  static const char etat_init[4] = {RESET, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION}, nbInstructions = 4;
  static char etat = etat_init[0], etape = 1, etat_suiv = 0;

  switch (etat)
  {
    case RESET:{
      Serial.println("INITIALISATION CARTE ARAL...");
      com.ACK.id = ID_RESET;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_RESET;
      com.sendMsg(com.ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case PREMIERE_SCRUTATION:{
      com.ACK.id = ID_PREMIERE_SCRUTATION;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_PREMIERE_SCRUTATION;
      com.sendMsg(com.ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case CHECK_CAPTEURS:{
      com.ACK.id = ID_CHECK_CAPTEURS;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_CHECK_CAPTEURS;
      com.sendMsg(com.ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      com.ACK.id = ID_DIFINITIVE_SCRUTATION;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION;
      com.sendMsg(com.ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case ATT_ACK:{
      if(com.checkACK()){
        Serial.println("ACK reçu ! ");
        etat = etat_suiv;
        etape ++;
        if(etape > nbInstructions){
          etat = FINISH;
          return true;
        }
      }
      else if(com.checkRepeatRequest()){
        com.sendMsg(com.ACK.id);
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

void TestComCarteARAL(){
  static const char SELECTION_DE_VOIE = 0, PREMIERE_SCRUTATION = 1, CHECK_CAPTEURS = 2, DIFINITIVE_SCRUTATION = 3, VERIFICATION_BONNE_VOIE = 4, ATT_ACK = 5;
  static const char etat_init[5] = {SELECTION_DE_VOIE, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION, VERIFICATION_BONNE_VOIE}, nbInstructions = 5;
  static char etat = etat_init[0], etape = 1, etat_suiv = 0;

  static int voieActuelle = 1; static uint32_t startTimeVoie = 0, TempsSwitchVoie = 750/3; 
  static int _numAlarme = 0; static Tension _TabTension[4]={COURT_CIRCUIT, ALARME, NORMAL, CONGRUENCE}, alarmeActuelle;

  switch (etat)
  {
    case SELECTION_DE_VOIE:{
      // static bool init = true;
      // if(init){
      //   init = false; 
      //   etape = 1;
      //   etat_suiv = etat_init[etape];
      //   etat = etat_suiv;
      //   etape++;
      // }
      if((millis() - startTimeVoie)>TempsSwitchVoie){
        // Serial.println("SELECTION_DE_VOIE");
        startTimeVoie = millis();
        alarmeActuelle = _TabTension[_numAlarme];
        selectionVoie(alarmeActuelle, voieActuelle);
        _numAlarme = (_numAlarme + 1)%4;
        if(!_numAlarme){
          voieActuelle = (voieActuelle + 1)%97;
          if(!voieActuelle){voieActuelle = 1;}
        }
        etape = 1;
        etat_suiv = etat_init[etape];
        etat = etat_suiv;
        etape++;
      }
    }
    break;
    case PREMIERE_SCRUTATION:{
      // Serial.println("PREMIERE_SCRUTATION");
      com.ACK.id = ID_PREMIERE_SCRUTATION;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_PREMIERE_SCRUTATION;
      com.sendMsg(com.ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case CHECK_CAPTEURS:{
      // Serial.println("CHECK_CAPTEURS");
      com.ACK.id = ID_CHECK_CAPTEURS;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_CHECK_CAPTEURS;
      com.sendMsg(com.ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      // Serial.println("DIFINITIVE_SCRUTATION");
      com.ACK.id = ID_DIFINITIVE_SCRUTATION;
      com.ACK.waitingAckFrom = ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION;
      com.sendMsg(com.ACK.id);
      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case VERIFICATION_BONNE_VOIE:{
      // Serial.println("VERIFICATION_BONNE_VOIE");
      EtatVoies voies;
      getTension(voies);
      Serial.printf("Test de la voie %d, alarme %d", voieActuelle, alarmeActuelle);
      if(voies.voies[voieActuelle-1] == alarmeActuelle){
        Serial.println(" OKAY!");
      }else{
        Serial.println(" LA VOIE N'EST PAS BONNE!!");
      }
      etat = SELECTION_DE_VOIE;
      etape = 1;
    }
    break;
    case ATT_ACK:{
      if(com.checkACK()){
        // Serial.println("ACK reçu ! ");
        etat = etat_suiv;
        etape ++;
        if(etape > nbInstructions){
          etat = SELECTION_DE_VOIE;
          etape = 1;
        }
      }
      else if(com.checkRepeatRequest()){
        Serial.println("La carte ARAL demande une repetition du dernier message...");
        com.sendMsg(com.ACK.id);
      }  
    }
    break;
    default:
      break;
  }
}

void TestContinueVoiesBoucleOuverte(){
  static int voie = 1; static uint32_t startTimeVoie = 0, TempsSwitchVoie = 250; 
  static int numAlarme = 0; static Tension TabTension[4]={COURT_CIRCUIT, ALARME, NORMAL, CONGRUENCE};

  if((millis() - startTimeVoie)>TempsSwitchVoie){
    startTimeVoie = millis();
    selectionVoie(TabTension[numAlarme], voie);
    numAlarme = (numAlarme + 1)%4;
    if(!numAlarme){
      voie = (voie + 1)%97;
      if(!voie){voie = 1;}
    }
  }
}

bool ControleParMoniteurSerie(){
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
      return true;

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
  return false;
}

