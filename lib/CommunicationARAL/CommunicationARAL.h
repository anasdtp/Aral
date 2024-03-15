#include <Arduino.h>


#define HEADER_DEBUT 0x02
#define HEADER_FIN 0x03
// uC vers ARAL : 
/*
Protocole de communication : 
Bloc Avec Info : 
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [DATA] taille variable, minimum 1 octet
  [CHEKSUM] 1 octet, checksum 
  [HEADER] A octet = 0x03 [03]

Bloc sans Info :
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [CHEKSUM] 1 octet, checksum, toujours égale à 0
  [HEADER] A octet = 0x03 [03]
*/
//Liste des ID : 
#define ID_POLLING_ARAL 0x11 //Sans bloc INFOS
#define ID_CHECK_CAPTEURS 0x12 //Sans bloc INFOS
#define ID_ORDRES 0x13 //Avec bloc INFOS
#define ID_PREMIERE_SCUTATION 0x14 //Sans bloc INFOS
#define ID_DIFINITIVE_SCUTATION 0x15 //Sans bloc INFOS
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
  [CHEKSUM] 1 octet, checksum, XOR de tous les octets data 
  [HEADER] A octet = 0x03 [03]

Bloc Sans Info :    
  [HEADER] 1 octect = 0x02 [02]
  [ID] 1 octet Identité du message
  [CHEKSUM] 1 octet, checksum, toujours égale à 0
  [HEADER] A octet = 0x03 [03]
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



#define SIZE_FIFO 150


typedef struct Message{
    uint8_t id;
    uint8_t len;
    uint8_t *data;
    uint8_t checksum;
}Message;

// État de la réception
enum StateRx{
  WAITING_HEADER,
  RECEIVING_ID,
  RECEIVING_NB,
  RECEIVING_DATA,
  RECEIVING_CHECKSUM,
  WAITING_FOOTER
};

class CommunicationARAL
{
public:
    CommunicationARAL(HardwareSerial *srl, long baud = 115200);
    ~CommunicationARAL();

private:
    HardwareSerial *_serial;
    Message rxMsg[SIZE_FIFO]; int FIFO_ecriture;

    void onReceiveFunction(void);
    bool MsgAvecBlocINFOS(uint8_t ID);
};


