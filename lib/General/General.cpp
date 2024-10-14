#include "General.h"

General::General(CommunicationARAL *com, CommunicationPC *pc, SelectionDeLaVoie *voie, CreateurTension *alarme)
{
    _alarme = alarme;
    _com = com;
    _pc = pc;
    _voie = voie;

    etat_gen = INIT_COM;
    
    _pc->setNombreTours(1);
}

void General::IHMBegin()
{
    IHM_begin();
    setLedColor(NUM_PIXELS, WHITE);
    printMidOLED("Banc de\ntest ARAL", 2);
}

void General::run(){
    _pc ->RxManage();
    _com->RxManage();//Fonction qui gére en parrallele les données triées prealablement 

    txManage();
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
    _alarme->creationTensionVoie_1_48(BOUCLE_OUVERTE);
    _alarme->creationTensionVoie_49_96(BOUCLE_OUVERTE);
    return;
  }

  if(voie>48){
    _alarme->creationTensionVoie_1_48(BOUCLE_OUVERTE);//Pour pas qu'elle est une autre valeur "au hasard"
    _alarme->creationTensionVoie_49_96(alarme);
  }else{
    _alarme->creationTensionVoie_1_48(alarme);
    _alarme->creationTensionVoie_49_96(BOUCLE_OUVERTE);//Pour pas qu'elle est une autre valeur "au hasard"
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

void General::txManage(){
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
      nbToursFait = 0;
      for (uint8_t i = 0; i < 96; i++)
      {
        bilan.tempsReponse[i] = 0;
      }
    }
    else if(_pc->getStopTestRequest()){
      etat_gen = BILAN;//Pour que si la personne ne veut pas faire de test automatique, mais plutot utiliser l'ancien logiciel TeraTerm
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
    if(TestCarteARAL(bilan)){
      // LedBilanTest(bilan);
      etat_gen = BILAN;
      // LedEtatProgramme(2);
    }
    else if(_pc->getStopTestRequest(false)){
      etat_gen = BILAN;
      _pc->sendMsg(ID_ACK_REQUEST_NB_TOURS_FAIT, (uint16_t)nbToursFait);
      _pc->sendMsg(ID_TEST_TERMINEE, bilan);
    }else if (_pc->getBilanRequest()){
      _pc->sendMsg(ID_TEST_EN_COURS, bilan);
    }
    else if(_pc->getNbToursFaitRequest()){
      _pc->sendMsg(ID_ACK_REQUEST_NB_TOURS_FAIT, (uint16_t)nbToursFait);
    }
    _pc->getRestartTestRequest();//Mis ici pour baisser le flag si jamais l'utilisateur envoit un reset alors qu'on est deja entrain de faire un test
  }
  break;
  case BILAN:
  {
    displayBilanTest(bilan);
    rainbow(20);
    _pc->getStopTestRequest();//Mis ici pour baisser le flag si jamais l'utilisateur envoit un stop alors qu'on est deja au bilan
    if(_pc->getRestartTestRequest()){
      etat_gen = TEST_VOIES;
      nbToursFait = 0;
      for (int i = 0; i < 96; i++)
      {
        bilan.voies[i] = VOIE_NONE;//Remise à 0 du bilan
      }
    }
    else if (_pc->getBilanRequest()){
      _pc->sendMsg(ID_TEST_TERMINEE, bilan);
    }
    else if(_pc->getNbToursFaitRequest()){
      _pc->sendMsg(ID_ACK_REQUEST_NB_TOURS_FAIT, (uint16_t)nbToursFait);
    }

    _pc->getCmdVoie(_setUneVoie);
    // TestContinueEnBoucleOuverte();//Pour faire tourner les voies si jamais sans verification
    TestCarteARALSansVerification(_setUneVoie, _pc->getCmdRandomSelectionVoie());
  }
  break;
  default:
    etat_gen = INIT_COM;
    break;
  }
}

void General::ComBasicARAL(AralState etat){
  switch (etat)
  {
    case RESET:{
      _com->ACK.id = ID_RESET;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_RESET;
      _com->sendMsg(_com->ACK.id);
    }
    break;
    case PREMIERE_SCRUTATION:{
      _com->ACK.id = ID_PREMIERE_SCRUTATION;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_PREMIERE_SCRUTATION;
      _com->sendMsg(_com->ACK.id);
    }
    break;
    case CHECK_CAPTEURS:{
      _com->ACK.id = ID_CHECK_CAPTEURS;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_CHECK_CAPTEURS;
      _com->sendMsg(_com->ACK.id);
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      _com->ACK.id = ID_DIFINITIVE_SCRUTATION;
      _com->ACK.waitingAckFrom = ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION;
      _com->sendMsg(_com->ACK.id);
    }
    break;
    default:
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
  static int nb_essais = 0;

  switch (etat)
  {
    case RESET:{
      
      //Serial.println("INITIALISATION CARTE ARAL...");
      printMidOLED("INITIALISATION\nCARTE ARAL...\n" + msgErreur, 1);
      
      ComBasicARAL(etat);

      setLedColor(etat+1 + (nb_essais++), color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
      _pc->sendMsg(ID_INITIALISATION_ARAL_EN_COURS, (uint8_t)nb_essais);//Envoi de l'info au pc qu'on est entrain d'initialiser l'aral et le nombre de tentative
      // {
      //   static EtatVoies voies; static Tension ten[4] = {COURT_CIRCUIT, ALARME, NORMAL, CONGRUENCE}; static int i = 0;
      //   voies.voies[0] = ten[(i++)%4];  voies.voies[1] = ten[(i++)%4]; voies.voies[2] = ten[(i++)%4]; voies.voies[3] = ten[(i++)%4];
      //   _pc->sendMsg(ID_ETAT_VOIES, voies); 
      // }//Test
    }
    break;
    case PREMIERE_SCRUTATION:{
      printMidOLED("INITIALISATION\nCARTE ARAL...\n" + msgErreur, 1);
      
      ComBasicARAL(etat);

      setLedColor(etat+1, color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
    }
    break;
    case CHECK_CAPTEURS:{
      ComBasicARAL(etat);

      setLedColor(etat+1, color[0]);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
      startTimeVoie = millis();
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      ComBasicARAL(etat);

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
        printMidOLED("La carte ARAL demande une repetition du dernier message...", 1);
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
      getTension(_voies);
      _pc->sendMsg(ID_ETAT_VOIES, _voies);
      setLedColor(1);//NUM_PIXELS);
      LedEtatProgramme(0);
      etat = etat_init[0]; etat_suiv = etat;
      nbInstructions = 4;
      etape = 1;

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

bool General::TestCarteARAL(BilanTest &bilan){
  static const AralState etat_init[5] = {SELECTION_DE_VOIE, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION, VERIFICATION_BONNE_VOIE};
  static AralState etat = etat_init[0], etat_suiv = etat;
  static char etape = 1, nbInstructions = 5;
  static String alarmeText[4]={"COURT_CIRCUIT", "___ALARME____", "___NORMAL____", "_CONGRUENCE__"};

  static int voieActuelle = 0; 
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

        alarmeActuelle = _TabTension[_numAlarme];   //** */
        if(!_numAlarme){
          voieActuelle = (voieActuelle + 1)%97; //** */
          if(!voieActuelle){
            voieActuelle = 1; //** */
            nbToursFait++;
            _pc->sendMsg(ID_TEST_EN_COURS, bilan);
            _pc->sendMsg(ID_ACK_REQUEST_NB_TOURS_FAIT, (uint16_t)nbToursFait);
            _pc->sendMsgTempsDeReponse(ID_TEST_TEMPS_DE_REPONSE_FILTRAGE, bilan);
            for (uint8_t i = 0; i < 96; i++)
            {
              bilan.tempsReponse[i] = 0;
            }
          }
        }
        selectionVoie(alarmeActuelle, voieActuelle); //ICI on selectionne la voie et l'alarme pour le tour actuelle

        //Et à la suite on régle la voie et l'alarme pour le prochain tour, celui d'aprés
        if(_pc->getModeTension() == MODE_2_ALARMES){
          alarmeActuelle = _numAlarme?alarmeActuelle : _TabTension[2];
          _numAlarme = (_numAlarme + 1)%2;//Soit COURT_CIRCUIT soit ALARME //** */ ATTENTION!! La tension COURT_CIRCUIT sera renvoyé en tant que NORMAL!
        }
        else{//MODE_4_ALARMES
          _numAlarme = (_numAlarme + 1)%4;//Les 4 alarmes //** */
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
      ComBasicARAL(etat);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case CHECK_CAPTEURS:{
      // //Serial.println("CHECK_CAPTEURS");
      ComBasicARAL(etat);

      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      // //Serial.println("DIFINITIVE_SCRUTATION");
      ComBasicARAL(etat);
      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case VERIFICATION_BONNE_VOIE:{
      // //Serial.println("VERIFICATION_BONNE_VOIE");
      getTension(_voies);
      _pc->sendMsg(ID_ETAT_VOIES, _voies);
      //Serial.printf("Test de la voie %2d, alarme %s", voieActuelle, alarmeText[alarmeActuelle]);
      if(voieActuelle > 0 && voieActuelle <= 96){
        if(_voies.voies[voieActuelle-1] == alarmeActuelle){
          if(bilan.voies[voieActuelle-1] != VOIE_EN_DEFAUT){
            bilan.voies[voieActuelle-1] = VOIE_OK;
            //Serial.println(" OKAY!");
          }
          // else{
            // bilan.voies[voieActuelle-1] = false;
            //Serial.println(" Okay sur cette alarme mais sur d'autres alarmes non !");
          // }
          
          uint8_t diziemeDeSeconde = (millis() - startTimeVoie)/100;//En dizieme de seconde car la valeur est sur un octet, soit 255 au max
          if(diziemeDeSeconde >= bilan.tempsReponse[voieActuelle - 1]){
            bilan.tempsReponse[voieActuelle - 1] = diziemeDeSeconde;
            //Serial.printf(" Temps de réponse : %d\n", diziemeDeSeconde);
          }
          
          etat = SELECTION_DE_VOIE;
          etape = 1;
        }
        else{
          // Ici verifier si filtrage est activé, 
                       // si oui alors verifié si startTimeVoie n'est pas superieur à TIMEOUT_ACK
                                            //si oui alors la voie est en defaut
                                            //si non, on refait la DIFINITIVE_SCRUTATION avec _com->sendMsg(_com->ACK.id);
          if(_pc->isFiltrageTrue()){
            if(millis() - startTimeVoie > TIMEOUT_ACK){
              // Serial.println(" LA VOIE N'EST PAS BONNE!!");
              bilan.voies[voieActuelle-1] = VOIE_EN_DEFAUT;
              
              uint8_t diziemeDeSeconde = (millis() - startTimeVoie)/100;//En dizieme de seconde car la valeur est sur un octet, soit 255 au max
              if(diziemeDeSeconde >= bilan.tempsReponse[voieActuelle - 1]){
                bilan.tempsReponse[voieActuelle - 1] = diziemeDeSeconde;
                //Serial.printf(" Temps de réponse : %d\n", diziemeDeSeconde);
              }
              
              etat = SELECTION_DE_VOIE;
              etape = 1;
            }
            else{
              // Alors c'est normal que l'etat reste le meme et on attend que la carte ARAL prend en compte l'alarme
              // On repete la phase com pour recuperer le tableau des 96 voies
              // etat = PREMIERE_SCRUTATION;
              uint8_t diziemeDeSeconde = (millis() - startTimeVoie)/100;//En dizieme de seconde car la valeur est sur un octet, soit 255 au max
              if(diziemeDeSeconde >= bilan.tempsReponse[voieActuelle - 1]){
                bilan.tempsReponse[voieActuelle - 1] = diziemeDeSeconde;
                //Serial.printf(" Temps de réponse : %d\n", diziemeDeSeconde);
              }
              etape = 1;
              etat_suiv = etat_init[etape];
              etat = etat_suiv;
              etape++;
            }
          }
          else{
            // Serial.println(" LA VOIE N'EST PAS BONNE!!");
            bilan.voies[voieActuelle-1] = VOIE_EN_DEFAUT;

            uint8_t diziemeDeSeconde = (millis() - startTimeVoie)/100;//En dizieme de seconde car la valeur est sur un octet, soit 255 au max
            if(diziemeDeSeconde >= bilan.tempsReponse[voieActuelle - 1]){
              bilan.tempsReponse[voieActuelle - 1] = diziemeDeSeconde;
              //Serial.printf(" Temps de réponse : %d\n", diziemeDeSeconde);
            }

            etat = SELECTION_DE_VOIE;
            etape = 1;
          }
        }
        _pc->sendMsg(ID_ETAT_UNE_VOIE, (uint8_t)voieActuelle, (uint8_t)bilan.voies[voieActuelle-1]);
        _pc->sendMsg(ID_TEST_TEMPS_DE_REPONSE_FILTRAGE_UNE_VOIE, (uint8_t)voieActuelle, (uint8_t)bilan.tempsReponse[voieActuelle - 1]);
        displayEtatVoie(voieActuelle, alarmeText[alarmeActuelle], bilan.voies[voieActuelle-1]);

        LedAlarmeActuelle(alarmeActuelle);
        LedEtatVoieActuelle(bilan.voies[voieActuelle-1]);
      }
      else{
        etat = SELECTION_DE_VOIE;
        etape = 1;
      }
    }
    break;
    case ATT_ACK:{
      if(_com->checkACK()){
        // //Serial.println("ACK reçu ! ");
        LedACK();
        etat = etat_suiv;
        etape ++;
        if(etape > nbInstructions){//Sert a rien
          etat = SELECTION_DE_VOIE;
          etape = 1;
        }
      }
      else if(_com->checkRepeatRequest()){
        LedACK();
        //Serial.println("La carte ARAL demande une repetition du dernier message...");
        _com->sendMsg(_com->ACK.id);
        setLedColor(NUM_PIXELS, BLUE);
        printMidOLED("La carte ARAL demande une repetition du dernier message...", 1);
        _pc->sendMsg(ID_CARTE_ARAL_REPEAT_REQUEST, (uint8_t)_com->ACK.id);
      }
      else if((millis() - startTimeVoie)> TIMEOUT_ACK){
        startTimeVoie = millis();
        //Serial.println("La carte ARAL ne repond pas !");
        //Serial.println("Commande RESET ARAL...");
        printMidOLED("La carte ARAL \n ne repond pas ! \n Verifier les connexions", 1);
        setLedColor(NUM_PIXELS, BLUE);
        _com->sendMsg(_com->ACK.id);
        _pc->sendMsg(ID_CARTE_ARAL_NE_REPOND_PLUS, (uint8_t)_com->ACK.id);
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
      _pc->sendMsg(ID_ACK_REQUEST_NB_TOURS_FAIT, (uint16_t)nbToursFait);
      // nbToursFait = 0;
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

void General::TestCarteARALSansVerification(EtatUneVoie voie, bool random){
  static const AralState etat_init[5] = {SELECTION_DE_VOIE, PREMIERE_SCRUTATION, CHECK_CAPTEURS, DIFINITIVE_SCRUTATION, VERIFICATION_BONNE_VOIE};
  static AralState etat = etat_init[0], etat_suiv = etat;
  static char etape = 1, nbInstructions = 5;

  switch (etat)
  {
    case SELECTION_DE_VOIE:{
      if(random){
        TestContinueEnBoucleOuverte();
      }else{
        selectionVoie(voie.voie, voie.numVoie);
      }

      etape = 1;
      etat_suiv = etat_init[etape];
      etat = etat_suiv;
      etape++;
    }
    break;
    case PREMIERE_SCRUTATION:{
      ComBasicARAL(etat);
      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case CHECK_CAPTEURS:{
      ComBasicARAL(etat);
      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case DIFINITIVE_SCRUTATION:{
      ComBasicARAL(etat);
      etat_suiv = etat_init[etape];
      etat = ATT_ACK;
    }
    break;
    case VERIFICATION_BONNE_VOIE:{
      getTension(_voies);
      _pc->sendMsg(ID_ETAT_VOIES, _voies);

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
        if(etape > nbInstructions){//Sert a rien, mais au cas ou
          etat = SELECTION_DE_VOIE;
          etape = 1;
        }
      }
      else if(_com->checkRepeatRequest()){
        LedACK();
        _com->sendMsg(_com->ACK.id);
        setLedColor(NUM_PIXELS, BLUE);
        printMidOLED("La carte ARAL demande une repetition du dernier message...", 1);
        _pc->sendMsg(ID_CARTE_ARAL_REPEAT_REQUEST, (uint8_t)_com->ACK.id);
      }
    }
    break;
    default:
      break;
  }
}


void General::TestContinueEnBoucleOuverte(){
  static int voie = 1; static uint32_t startTimeVoie = 0, TempsSwitchVoie = 300; 
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