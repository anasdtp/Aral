#ifndef _CommunicationARAL_LIB
#define _CommunicationARAL_LIB
#include <Arduino.h>
#include <Commun.h>

#define HEADER_DEBUT 0x02
#define HEADER_FIN 0x03

#define MIN_SIZE 4 //taille minimale d'une trame en octets

// uC vers ARAL : 
/*
Protocole de communication : 
Bloc Avec Info : 
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [DATA] taille variable, minimum 1 octet
  [CHECKSUM] 1 octet, checksum 
  [HEADER] 1 octet = 0x03 [03]

Bloc sans Info :
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [CHECKSUM] 1 octet, checksum, toujours égale à 0
  [HEADER] 1 octet = 0x03 [03]
*/
//Liste des ID : 
#define ID_POLLING_ARAL 0x11 //Sans bloc INFOS
#define ID_CHECK_CAPTEURS 0x12 //Sans bloc INFOS
#define ID_ORDRES 0x13 //Avec bloc INFOS
#define ID_PREMIERE_SCRUTATION 0x14 //Sans bloc INFOS
#define ID_DIFINITIVE_SCRUTATION 0x15 //Sans bloc INFOS
#define ID_RESET 0x16 //Sans bloc INFOS
#define ID_DEMANDE_REPETITION_UC 0x30 //Sans bloc INFOS
#define ID_INHIBITION_96_VOIES 0x49 //Avec bloc INFOS

//ARAL vers uC : 
/*
Protocole de communication : 
Bloc Avec Info :    
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [NB] 1 octet, NB = (Nombre d'octets dans DATA + 0x10)
  [DATA] taille variable, minimum 1 octet
  [CHECKSUM] 1 octet, checksum, XOR de tous les octets data 
  [HEADER] 1 octet = 0x03 [03]

Bloc Sans Info :    
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [CHECKSUM] 1 octet, checksum, toujours égale à 0
  [HEADER] 1 octet = 0x03 [03]
*/
//Liste des ID : 
#define ID_ACKNOWLEDGE_POLLING 0x20 //Sans bloc INFOS
#define ID_ACKNOWLEDGE_POLLING_DATA 0x21 //Avec bloc INFOS
#define ID_ACKNOWLEDGE_CHECK_CAPTEURS 0x22 //Avec bloc INFOS //Capteurs = contacts secs avec alarmes
#define ID_ACKNOWLEDGE_ORDRES 0x23 //Sans bloc INFOS
#define ID_ACKNOWLEDGE_PREMIERE_SCRUTATION 0x24 //Sans bloc INFOS
#define ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION 0x25 //Sans bloc INFOS
#define ID_ACKNOWLEDGE_RESET 0x26 //Sans bloc INFOS
#define ID_DEMANDE_REPETITION_ARAL 0x31 //Sans bloc INFOS
#define ID_ACKNOWLEDGE_INHIBITION 0x49 //Sans bloc INFOS

/*
ACKNOWLEDGE = ack = accusé réception
*/
/*
Initialisation du Dialogue uC -> ARAL
  1 : ID_RESET
      a) attente ID_ACKNOWLEDGE_RESET
  2 : ID_PREMIERE_SCUTATION
      b) attente ID_ACKNOWLEDGE_PREMIERE_SCRUTATION
  3 : ID_CHECK_CAPTEURS
      c) attente ID_ACKNOWLEDGE_CHECK_CAPTEURS -> Actualisation de l'etat des 96 voies
  4 : ID_DIFINITIVE_SCUTATION
      d) attente ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION
*/
/*
Pour chaque attente d'ack, la carte est déclarée en défaut au bout de 3 essais consécutifs et sans succès. Temps d'attente avant repetition = 100 ms
*/
/*
ID_ACKNOWLEDGE_POLLING : 
    L'UC envoie un POLLING à l'ARAL avec FCT=11H.
        L'ARAL retourne un message avec FCT=21H, NB=2+10H et INFOS, si RAZ est effectué. 
        Bloc INFOS :
              data[0] = 14
              data[1] = 0+10H
ID_CHECK_CAPTEURS : 
    L'UC envoie un message à l'ARAL avec FCT=12H.
    L'ARAL retourne un message avec FCT=22H, NB et un bloc INFOS, si la transmission est correcte.
    Le bloc INFOS (taille=96 octets) contient l'état brut des 96 capteurs de la carte ARAL.
*/
/*
ID_ORDRES : 
    Lorsque l'UC désire envoyer un ordre d'inhibition ( ou autre ) d'un capteur à l'ARAL, elle envoie 
    un message avec FCT=13H et un bloc INFOS.
    Structure du bloc INFOS : 
                      data[0] = Code Commande
                      data[1] = Numéro Capteur - 10H
    L'ARAL retourne un accusé réception avec FCT=23H, si la transmission est correcte.
*/
/*
ID_DIFINITIVE_SCUTATION : 

*/

//Etat des voies
#define ETAT_COURT_CIRCUIT 0x0
#define ETAT_ALARME 0x10
#define ETAT_NORMAL 0x30
#define ETAT_CONGRUENCE 0x70

#define TIMEOUT_ACK 5000 //ms, timeout à la fois de reponse com et de prise en compte de l'alarme

typedef struct AlarmeVoies{
    uint8_t voies[96];
}AlarmeVoies;

typedef struct ManageACK{
  uint8_t id;
  uint8_t waitingAckFrom;
  bool AckFrom_FLAG;
  bool RepeatRequest_FLAG;
}ManageACK;

class CommunicationARAL
{
public:
    CommunicationARAL();

    void begin(HardwareSerial *srl = &Serial2, long baud = 2400);
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
    void sendMsg(uint8_t id, EtatVoies &voies);

    void printMessage(Message msg);

    AlarmeVoies etatVoies;
    ManageACK ACK;
    bool checkACK(bool afterCkeck = false);
    bool checkRepeatRequest(bool afterCkeck = false);

private:
    HardwareSerial *_serial;
    Message rxMsg[SIZE_FIFO]; int FIFO_ecriture;

    // État de la réception
    enum StateRx{
      WAITING_HEADER,
      RECEIVING_ID,
      RECEIVING_NB,
      RECEIVING_DATA,
      RECEIVING_CHECKSUM,
      WAITING_FOOTER
    };

    void onReceiveFunction(void);
    bool MsgAvecBlocINFOS(uint8_t ID);
};

#endif //_CommunicationARAL_LIB