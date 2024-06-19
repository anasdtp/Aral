#ifndef _CommunicationPC_LIB
#define _CommunicationPC_LIB
#include <Arduino.h>
#include <Commun.h>
#include <BluetoothSerial.h>

#define ID_NB_TOURS 0xA0 //On reçoit le bombre de tours à faire sur 2 octets
#define ID_ACK_NB_TOURS 0xA1 //On accuse la réception

#define ID_INITIALISATION_ARAL_EN_COURS 0xB0 //renvoi le nombre de tentative de com
#define ID_INITIALISATION_ARAL_FAITE 0xB1 //ne renvoi rien
#define ID_TEST_EN_COURS 0xB2 //envoi le bilan de test du tours effectué
#define ID_TEST_TERMINEE 0xB3 //envoi le bilan de test
#define ID_ETAT_VOIES 0xB4 //Envoi de l'etat des voies directement de la carte ARAL au PC

#define ID_ACK_GENERAL 0xC0 //Ack pour tous le reste
#define ID_RELANCER_TEST 0xC1
#define ID_ARRET_TEST 0xC2
#define ID_REPEAT_REQUEST 0xD0

class CommunicationPC
{
public:
    CommunicationPC(/* args */);

    void begin(HardwareSerial *srl = &Serial, long baud = 921600, String nameBT = "Banc_de_test_ARAL");
    void end();

    void RxManage();

    void sendMsg(Message txMsg);
    void sendMsg(uint8_t id);
    void sendMsg(uint8_t id, uint8_t len, uint8_t *data);
    void sendMsg(uint8_t id, uint8_t octet);
    void sendMsg(uint8_t id, uint16_t nb);
    void sendMsg(uint8_t id, uint32_t nb);
    void sendMsg(uint8_t id, BilanTest &bilan);
    void sendMsg(uint8_t id, EtatVoies &voies);
    void printMessage(Message msg);

    uint16_t getNombreTours(){return NBTOURS;}
    void setNombreTours(uint16_t nbTours){NBTOURS = nbTours;}

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

private:
    HardwareSerial *_serial;
    BluetoothSerial *_serialBT;
    Message rxMsg[SIZE_FIFO]; int FIFO_ecriture;

    uint16_t NBTOURS; bool _resetTestRequest, _StopTestRequest;

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