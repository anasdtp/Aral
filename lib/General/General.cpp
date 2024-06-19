#include "General.h"

General::General(CommunicationARAL *com, CommunicationPC *pc, SelectionDeLaVoie *voie, CreateurTension *alarme)
{
    _alarme = alarme;
    _com = com;
    _pc = pc;
    _voie = voie;

    etat_gen = INIT_COM;
    
    _pc->setNombreTours(1);
    nbToursFait = 0;
}

void General::IHMBegin()
{
    IHM_begin();
    setLedColor(NUM_PIXELS, WHITE);
    printMidOLED("Banc de\ntest\nARAL", 2, 1);
}

void General::run(){
    _pc->RxManage();
    _com->RxManage();//Fonction qui gére en parrallele les données triées prealablement 

    txLoop();
}

void General::selectionVoie(Tension alarme, uint8_t voie){//de 1 à 96
  if(voie > 96 || voie==0){
    return;
  }
  
  // _voie->disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
  // _alarme->creationTensionVoie_1_48(COURT_CIRCUIT);
  // _alarme->creationTensionVoie_49_96(COURT_CIRCUIT);

  if(alarme == BOUCLE_OUVERTE){
    _voie->disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
    _alarme->creationTensionVoie_1_48(COURT_CIRCUIT);
    _alarme->creationTensionVoie_49_96(COURT_CIRCUIT);
    return;
  }

  if(voie>48){
    _alarme->creationTensionVoie_49_96(alarme);
  }else{
    _alarme->creationTensionVoie_1_48(alarme);
  }

  _voie->selectionVoie(voie);
}

void General::getTension(EtatVoies &voies, bool affichage){
  static String alarmeText[4]={"COURT_CIRCUIT", "ALARME_______", "NORMAL_______", "CONGRUENCE___"};

  if(affichage){Serial.println("get Tension des 96 voies : Voies[96] = ");}
  
  for (int numVoie = 0; numVoie < 96; numVoie++)
  {
    if(_com->etatVoies.voies[numVoie] == ETAT_COURT_CIRCUIT){
      voies.voies[numVoie] = COURT_CIRCUIT;
    }
    else if(_com->etatVoies.voies[numVoie] == ETAT_ALARME){
      voies.voies[numVoie] = ALARME;
    }
    else if(_com->etatVoies.voies[numVoie] == ETAT_NORMAL){
      voies.voies[numVoie] = NORMAL;
    }
    else if(_com->etatVoies.voies[numVoie] == ETAT_CONGRUENCE){
      voies.voies[numVoie] = CONGRUENCE;
    }
    if(affichage){Serial.printf("%2d [%s]\n", numVoie+1, alarmeText[int(voies.voies[numVoie])]);}
  }
  if(affichage){Serial.println("");}
}

void General::txLoop(){
  static BilanTest bilan;
  switch (etat_gen)
  {
  case INIT_COM:
  {
    static bool initDone = false;
    if(initialisationARAL() && !initDone){
      etat_gen = TEST_VOIES;
      initDone = true;
      //Serial.println("Communication carte ARAL initialisée !");
      _pc->sendMsg(ID_INITIALISATION_ARAL_FAITE);

    }

    //Test Bilan a commenté: 
    // for (int i = 0; i < 96; i++)
    // {
    //   bilan.voies[i] = VOIE_OK;
    // }
    // bilan.voies[33] = VOIE_EN_DEFAUT;
    // bilan.voies[11] = VOIE_NONE;
    // bilan.voies[22] = VOIE_EN_DEFAUT;
    // bilan.voies[88] = VOIE_EN_DEFAUT;
    // bilan.voies[77] = VOIE_EN_DEFAUT;
    // etat = BILAN; 
  }
    break;
  case TEST_VOIES:
  {
    if(TestComCarteARAL(bilan)){
      LedBilanTest(bilan);
      etat_gen = BILAN;
      LedEtatProgramme(2);
    }
    _pc->getRestartTestRequest();//Mis ici pour baisser le flag si jamais l'utilisateur envoit un reset alors qu'on est deja entrain de faire un test
    if(_pc->getStopTestRequest()){
      etat_gen = BILAN;
    }
  }
  break;
  case BILAN:
  {
    displayBilanTest(bilan);
    rainbow(20);
    if(_pc->getRestartTestRequest()){
      etat_gen = TEST_VOIES;
    }
    _pc->getStopTestRequest();//Mis ici pour baisser le flag si jamais l'utilisateur envoit un stop alors qu'on est deja au bilan
  }
  break;
  default:
    etat_gen = INIT_COM;
    break;
  }
}

bool General::initialisationARAL(){
  static const AralState etat_init[4] = {RESET, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION};
  static AralState etat = etat_init[0], etat_suiv = etat;
  static char etape = 1, nbInstructions = 4;
  static uint32_t startTimeVoie = 0;
  static uint8_t *color[3] = {GREEN};
  static String msgErreur = "";

  switch (etat)
  {
    case RESET:{
      static int nb_essais = 0;
      //Serial.println("INITIALISATION CARTE ARAL...");
      printMidOLED("INITIALISATION\nCARTE ARAL...\n" + msgErreur , 1, 1);
      _com->ACK.id = ID_RESET;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_RESET;
      _com->sendMsg(_com->ACK.id);

      setLedColor(etat+1 + (nb_essais++), color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
      _pc->sendMsg(ID_INITIALISATION_ARAL_EN_COURS, (uint8_t)nb_essais);//Envoi de l'info au pc qu'on est entrain d'initialiser l'aral et le nombre de tentative
      {
        static EtatVoies voies; static Tension ten[4] = {COURT_CIRCUIT, ALARME, NORMAL, CONGRUENCE}; static int i = 0;
        voies.voies[0] = ten[(i++)%4];  voies.voies[1] = ten[(i++)%4]; voies.voies[2] = ten[(i++)%4]; voies.voies[3] = ten[(i++)%4];
        _pc->sendMsg(ID_ETAT_VOIES, voies); 
      }//Test
    }
    break;
    case PREMIERE_SCRUTATION:{
      printMidOLED("INITIALISATION\nCARTE ARAL...\n" + msgErreur , 1, 1);
      _com->ACK.id = ID_PREMIERE_SCRUTATION;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_PREMIERE_SCRUTATION;
      _com->sendMsg(_com->ACK.id);

      setLedColor(etat+1, color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
    }
    break;
    case CHECK_CAPTEURS:{
      _com->ACK.id = ID_CHECK_CAPTEURS;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_CHECK_CAPTEURS;
      _com->sendMsg(_com->ACK.id);

      setLedColor(etat+1, color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      _com->ACK.id = ID_DIFINITIVE_SCRUTATION;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION;
      _com->sendMsg(_com->ACK.id);

      setLedColor(etat+1, color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
    }
    break;
    case ATT_ACK:{
      if(_com->checkACK()){
        //Serial.println("ACK reçu ! ");
        msgErreur = "\nACK recu !";
        
        color[0] = GREEN;
        etat = etat_suiv;
        etape ++;
        if(etape > nbInstructions){
          etat = FINISH;
        }
      }
      else if(_com->checkRepeatRequest()){
        //Serial.println("La carte ARAL demande une repetition du dernier message...");
        printMidOLED("La carte ARAL demande une repetition du dernier message...", 1, 1);
        _com->sendMsg(_com->ACK.id);
        color[0] = BLUE;
      }
      else if((millis() - startTimeVoie)> TIMEOUT_ACK){
        //Serial.println("La carte ARAL ne repond pas !");
        //Serial.println("Commande RESET ARAL...");
        msgErreur = "La carte ARAL \n ne repond pas ! \n Verifier switch 8";
        color[0] = RED;
        // _com->sendMsg(ID_RESET);
        etat = RESET;
      }
    }
    break;
    case FINISH:{
      //Serial.println("Communication carte ARAL Fonctionnelle !");
      EtatVoies voies;
      getTension(voies);
      _pc->sendMsg(ID_ETAT_VOIES, voies);
      setLedColor(1);//NUM_PIXELS);
      LedEtatProgramme(0);
      etat = NONE;
      return true;
    }
    break;
    case NONE:{
      return true;
    }break;
    default:
      break;
  }

  return false;
}

bool General::TestComCarteARAL(BilanTest &bilan){

  static const AralState etat_init[5] = {SELECTION_DE_VOIE, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION, VERIFICATION_BONNE_VOIE};
  static AralState etat = etat_init[0], etat_suiv = etat;
  static char etape = 1, nbInstructions = 5;
  static String alarmeText[4]={"COURT_CIRCUIT", "___ALARME____", "___NORMAL____", "_CONGRUENCE__"};

  static int voieActuelle = 1; 
  static uint32_t startTimeVoie = 0, TempsSwitchVoie = 0;//750/3; 
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
        // //Serial.println("SELECTION_DE_VOIE");
        startTimeVoie = millis();
        alarmeActuelle = _TabTension[_numAlarme];
        selectionVoie(alarmeActuelle, voieActuelle);
        _numAlarme = (_numAlarme + 1)%4;
        if(!_numAlarme){
          voieActuelle = (voieActuelle + 1)%97;
          if(!voieActuelle){
            voieActuelle = 1;
            nbToursFait++;
            _pc->sendMsg(ID_TEST_EN_COURS, bilan);
          }
        }
        etape = 1;
        etat_suiv = etat_init[etape];
        etat = etat_suiv;
        etape++;
        if(nbToursFait>=_pc->getNombreTours()){
          etat = FINISH;
        }

        LedEtatProgramme(1);

      }
    }
    break;
    case PREMIERE_SCRUTATION:{
      // //Serial.println("PREMIERE_SCRUTATION");
      _com->ACK.id = ID_PREMIERE_SCRUTATION;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_PREMIERE_SCRUTATION;
      _com->sendMsg(_com->ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case CHECK_CAPTEURS:{
      // //Serial.println("CHECK_CAPTEURS");
      _com->ACK.id = ID_CHECK_CAPTEURS;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_CHECK_CAPTEURS;
      _com->sendMsg(_com->ACK.id);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      // //Serial.println("DIFINITIVE_SCRUTATION");
      _com->ACK.id = ID_DIFINITIVE_SCRUTATION;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION;
      _com->sendMsg(_com->ACK.id);
      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case VERIFICATION_BONNE_VOIE:{
      // //Serial.println("VERIFICATION_BONNE_VOIE");
      EtatVoies voies;
      getTension(voies);
      _pc->sendMsg(ID_ETAT_VOIES, voies);
      //Serial.printf("Test de la voie %2d, alarme %s", voieActuelle, alarmeText[alarmeActuelle]);
      if(voies.voies[voieActuelle-1] == alarmeActuelle){
        if(bilan.voies[voieActuelle-1] != VOIE_EN_DEFAUT){
          bilan.voies[voieActuelle-1] = VOIE_OK;
          //Serial.println(" OKAY!");
        }
        else{
          // bilan.voies[voieActuelle-1] = false;
          //Serial.println(" Okay sur cette alarme mais sur d'autres alarmes non !");
        }
      }else{
        //Serial.println(" LA VOIE N'EST PAS BONNE!!");
        bilan.voies[voieActuelle-1] = VOIE_EN_DEFAUT;
      }
      displayEtatVoie(voieActuelle, alarmeText[alarmeActuelle], bilan.voies[voieActuelle-1]);

      LedAlarmeActuelle(alarmeActuelle);
      LedEtatVoieActuelle(bilan.voies[voieActuelle-1]);
      etat = SELECTION_DE_VOIE;
      etape = 1;
    }
    break;
    case ATT_ACK:{
      if(_com->checkACK()){
        // //Serial.println("ACK reçu ! ");
        LedACK();
        etat = etat_suiv;
        etape ++;
        if(etape > nbInstructions){
          etat = SELECTION_DE_VOIE;
          etape = 1;
        }
      }
      else if(_com->checkRepeatRequest()){
        LedACK();
        //Serial.println("La carte ARAL demande une repetition du dernier message...");
        _com->sendMsg(_com->ACK.id);
        setLedColor(NUM_PIXELS, BLUE);
        printMidOLED("La carte ARAL demande une repetition du dernier message...", 1, 1);
      }
      else if((millis() - startTimeVoie)> TIMEOUT_ACK){
        startTimeVoie = millis();
        //Serial.println("La carte ARAL ne repond pas !");
        //Serial.println("Commande RESET ARAL...");
        printMidOLED("La carte ARAL \n ne repond pas ! \n Verifier les connexions", 1, 1);
        setLedColor(NUM_PIXELS, BLUE);
        _com->sendMsg(_com->ACK.id);
      }
    }
    break;
    case FINISH:{
      //Serial.println("*************");
      //Serial.println("BILAN DE TEST");
      //Serial.printf("Nombre de tours durant le test : %d\n", nbToursFait);
      //Serial.println("Affichage du bilan de test pour chaque voie : ");
      //Serial.println("");
      // for (int numVoie = 0; numVoie < 96; numVoie++)
      // {
      //   //Serial.printf("Voie (%2d) bilan : ", numVoie+1);
      //   if(bilan.voies[numVoie] == VOIE_EN_DEFAUT){
      //     //Serial.println("DEFAUT!! Voie non fonctionnelle !");
      //   }else if(bilan.voies[numVoie] == VOIE_OK){
      //     //Serial.println("OKAY!");
      //   }
      //   else{
      //     //Serial.println("Voie non testée ou bien bug");
      //   }
      // }
      //Serial.println("");
      //Serial.println("*************");

      _pc->sendMsg(ID_TEST_TERMINEE, bilan);
      nbToursFait = 0;
      _numAlarme = 0;
      voieActuelle = 0;
      etat = SELECTION_DE_VOIE;
      return true;
    }
    break;
    case NONE:{
      return true;
    }break;
    default:
      break;
  }
  return false;
}

void General::TestContinueVoiesBoucleOuverte(){
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

bool General::ControleParMoniteurSerie(){
  static char instructionBuffer[20]; 
  static int etat = 0, nb_car = 0, init = true;
  static uint8_t voie = 0;
  
  switch (etat)
  {
    case 0:
    {
      if(init){
        init = false;
        _voie->disableMUX();//Pour eviter de mettre une alarme dans une voie indésirer pendant le traitement
        _alarme->creationTensionVoie_1_48(COURT_CIRCUIT);
        _alarme->creationTensionVoie_49_96(COURT_CIRCUIT);
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