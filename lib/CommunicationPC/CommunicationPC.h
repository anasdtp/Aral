#ifndef _CommunicationPC_LIB
#define _CommunicationPC_LIB
#include <Arduino.h>
#include <Commun.h>
#include <BluetoothSerial.h>

#define ID_NB_TOURS 0xA0 //On reçoit le nombre de tours à faire sur 2 octets
#define ID_ACK_NB_TOURS 0xA1 //On accuse la réception

#define ID_INITIALISATION_ARAL_EN_COURS 0xB0 //renvoi le nombre de tentative de com
#define ID_INITIALISATION_ARAL_FAITE 0xB1 //ne renvoi rien
#define ID_TEST_EN_COURS 0xB2 //envoi le bilan de test du tours effectué, tableau des 96 voies
#define ID_TEST_TERMINEE 0xB3 //envoi le bilan de test, tableau des 96 voies
#define ID_ETAT_VOIES 0xB4 //Envoi de l'etat des voies directement de la carte ARAL au PC
#define ID_ETAT_UNE_VOIE 0xB5 //Envoi de l'etat d'une voie en defaut ou ok avec data[0]->(Num de la voie) et data[1]->(Etat, OK=0x30, Defaut = 0x10, non testée = 0)
#define ID_CARTE_ARAL_NE_REPOND_PLUS 0xB6
#define ID_CARTE_ARAL_REPEAT_REQUEST 0xB7
#define ID_TEST_TEMPS_DE_REPONSE_FILTRAGE 0xB8 //Si jamais le filtrage est activé, Envoi du temps de réponse des 96 voies, tableau de 96 cases, en dizieme de seconde
#define ID_TEST_TEMPS_DE_REPONSE_FILTRAGE_UNE_VOIE 0xB9 //Envoi du temps de reponse d'une seule voie avec data[0]->(Num de la voie) et data[1]->dizieme de secondes

#define ID_ACK_GENERAL 0xC0 //Ack pour tous le reste
#define ID_RELANCER_TEST 0xC1
#define ID_ARRET_TEST 0xC2 
#define ID_COMMANDE_VOIES 0xC3//Hors test, commande des voies directement via un interface graphique. Envoi de la voie sur 1 octet et de l'etat sur 1 octet  [FF][ID_COMMANDE_VOIES][2][VOIE][ETAT][CKS][FF]
#define ID_RANDOM_SELECTION_VOIE 0xC4 //Selection aléatoire des voies, 1 = activer la selection aléatoire, 0 = desactiver

#define ID_REPEAT_REQUEST 0xD0
#define ID_REQUEST_NB_TOURS_FAIT 0xD1
#define ID_ACK_REQUEST_NB_TOURS_FAIT 0xD2
#define ID_REQUEST_BILAN 0xD3 //Reponse avec l'id ID_TEST_TERMINEE si terminée sinon ID_TEST_EN_COURS si test en cours
#define ID_SET_FILTRAGE 0xD4 //Filtrage des alarmes en fonction des switch SW2. Filtrage = etat stable pendant un temps desiré (etat stable = ne prends pas en compte les changements)
#define ID_SET_MODE_TENSION 0xD5 //Choix du mode de tension, soit 4 tensions soit 2. De base est à 4.

class CommunicationPC
{
public:
    CommunicationPC(/* args */);

    void begin(HardwareSerial *srl = &Serial, long baud = 921600, String nameBT = "Banc_de_test_ARAL");
    void end();

    void RxManage();

    void sendMsg(Message &txMsg);
    void sendMsg(uint8_t id, uint8_t len, uint8_t *data);
    void sendMsg(uint8_t id);
    void sendMsg(uint8_t id, uint8_t octet);
    void sendMsg(uint8_t id, uint8_t octet1, uint8_t octet2);
    void sendMsg(uint8_t id, uint16_t nb);
    void sendMsg(uint8_t id, uint32_t nb);
    void sendMsg(uint8_t id, BilanTest &bilan);
    void sendMsgTempsDeReponse(uint8_t id, BilanTest &bilan);
    void sendMsg(uint8_t id, EtatVoies &voies);
    void printMessage(Message msg);

    int getNombreTours(){return NBTOURS;}
    void setNombreTours(int nbTours){NBTOURS = nbTours;}

    bool getRestartTestRequest(bool afterCheck = false){
      if(_resetTestRequest){
        _resetTestRequest = afterCheck;
        return true;
      }
      return false;
    }

    bool getStopTestRequest(bool afterCheck = false){
      if(_StopTestRequest){
        _StopTestRequest = afterCheck;
        return true;
      }
      return false;
    }

    bool getNbToursFaitRequest(bool afterCheck = false){
      if(_NbToursFaitRequest){
        _NbToursFaitRequest = afterCheck;
        return true;
      }
      return false;
    }

    bool getBilanRequest(bool afterCheck = false){
      if(_BilanRequest){
        _BilanRequest = afterCheck;
        return true;
      }
      return false;
    }

    void setFiltrage(bool filtrage){
      _filtrage = filtrage;
    }
    bool isFiltrageTrue(){
      return _filtrage;
    }
    
    void setCmdVoie(EtatUneVoie cmdVoie){
      _cmdVoie = cmdVoie; 
      _cmdVoieReceived = true;
    }
    bool getCmdVoie(EtatUneVoie &cmdVoie, bool afterCheck = false){
      if(_cmdVoieReceived){
        cmdVoie = _cmdVoie; 
        _cmdVoieReceived = afterCheck; 
        return true;
      } 
      return false;
    }

    void setCmdRandomSelectionVoie(bool cmdRandomSelectionVoie){_cmdRandomSelectionVoie = cmdRandomSelectionVoie;}
    bool getCmdRandomSelectionVoie(){return _cmdRandomSelectionVoie;}

    void setModeTension(MODE_TENSION mode){_mode = mode;}
    MODE_TENSION getModeTension(){return _mode;}

private:
    HardwareSerial *_serial;
    BluetoothSerial *_serialBT;
    Message rxMsg[SIZE_FIFO]; int FIFO_ecriture;

    int NBTOURS; 
    bool _resetTestRequest, _StopTestRequest, _NbToursFaitRequest, _BilanRequest;
    bool _filtrage;//Flag pour savoir si le filtrage est activé ou non
    EtatUneVoie _cmdVoie; bool _cmdVoieReceived = false;
    bool _cmdRandomSelectionVoie = true;

    MODE_TENSION _mode = MODE_4_ALARMES;

    // État de la réception
    enum StateRx{
      WAITING_HEADER,
      RECEIVING_ID,
      RECEIVING_LEN,
      RECEIVING_DATA,
      RECEIVING_CHECKSUM,
      WAITING_FOOTER
    };

    void onReceiveFunction(void);

    static CommunicationPC* instance;  // Pointer to the current instance
    static void onReceiveFunctionBTStatic(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    void onReceiveFunctionBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

};




#endif // _CommunicationPC_LIB