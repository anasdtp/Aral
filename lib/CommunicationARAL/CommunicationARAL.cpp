#include "CommunicationARAL.h"
#include <functional>

// Macros : 
#define sendData(packet, length)  	(_serial->write(packet, length))    // Write Over Serial
#define flush()						(_serial->flush())					// Wait until buffer empty
#define availableData() 			(_serial->available())    			// Check Serial Data Available
#define readData()      			(_serial->read())         			// Read Serial Data
#define peekData()      			(_serial->peek())         			// Peek Serial Data
#define beginCom(args)  			(_serial->begin(args))   			// Begin Serial Comunication
#define endCom()        			(_serial->end())          			// End Serial Comunication

CommunicationARAL::CommunicationARAL()
{
    FIFO_ecriture = 0;
}

void CommunicationARAL::begin(HardwareSerial *srl, long baud){
    _serial = srl;
    beginCom(baud);
    _serial->onReceive(std::bind(&CommunicationARAL::onReceiveFunction, this));
}

void CommunicationARAL::end()
{
    endCom();
}

void CommunicationARAL::onReceiveFunction(void) {

    static StateRx currentState = WAITING_HEADER;
    static uint8_t dataCounter = 0;
    static uint8_t checksum = 0;

    while (availableData()) {
        uint8_t byte = readData();

        switch (currentState) {
            case WAITING_HEADER:{
                if (byte == HEADER_DEBUT) { // HEADER
                    currentState = RECEIVING_ID;
                }
                }
                break;

            case RECEIVING_ID:{
                rxMsg[FIFO_ecriture].id = byte;
                rxMsg[FIFO_ecriture].len = 0;
                if(MsgAvecBlocINFOS(rxMsg[FIFO_ecriture].id)){currentState = RECEIVING_NB;}
                else{currentState = RECEIVING_CHECKSUM;}
                checksum = 0;
                }break;

            case RECEIVING_NB:{
                rxMsg[FIFO_ecriture].len = byte - 0x10;
                rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];
                currentState = RECEIVING_DATA;
                dataCounter = 0;
                }break;

            case RECEIVING_DATA:{
                rxMsg[FIFO_ecriture].data[dataCounter++] = byte;
                checksum ^= byte;//XOR
                if (dataCounter >= rxMsg[FIFO_ecriture].len) {
                    currentState = RECEIVING_CHECKSUM;
                }
                }break;

            case RECEIVING_CHECKSUM:{
                if (byte == checksum) {
                    currentState = WAITING_FOOTER;
                } else {
                    // Gérer l'erreur de checksum ici
                    Serial.println("CommunicationARAL::onReceiveFunction() : Erreur calcul checksum");
                    currentState = WAITING_FOOTER;
                }
                }break;

            case WAITING_FOOTER:{
                if (byte == HEADER_FIN) { // FOOTER
                    // Le message est complet
                    // printMessage(rxMsg[FIFO_ecriture]);
                    FIFO_ecriture = (FIFO_ecriture + 1) % SIZE_FIFO;
                }
                currentState = WAITING_HEADER;
                delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée, pour les prochaines données
                }break;
        }
    }
}

//Doit etre dans la loop
void CommunicationARAL::RxManage(){
    static signed char FIFO_lecture = 0, FIFO_occupation = 0, FIFO_max_occupation = 0;


    FIFO_occupation = FIFO_ecriture - FIFO_lecture;
    if(FIFO_occupation<0){FIFO_occupation=FIFO_occupation+SIZE_FIFO;}
    if(FIFO_max_occupation<FIFO_occupation){FIFO_max_occupation=FIFO_occupation;}
    if(!FIFO_occupation){return;}
    //Alors il y a un nouveau message en attente de traitement

    switch (rxMsg->id)
    {
        case ID_ACKNOWLEDGE_POLLING:{
            //Sans bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_POLLING_DATA:{
            //Avec bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_CHECK_CAPTEURS:{
            //Avec bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_ORDRES:{
            //Sans bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_PREMIERE_SCRUTATION:{
            //Sans bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION:{
            //Sans bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_RESET:{
            //Sans bloc INFOS

        }break;

        case ID_DEMANDE_REPETITION_ARAL:{
            //Sans bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_INHIBITION:{
            //Sans bloc INFOS

        }break;
    
        default:
            break;
    }

    FIFO_lecture = (FIFO_lecture + 1) % SIZE_FIFO;
}

//Envoi d'un message du uC à la carte Aral
void CommunicationARAL::sendMsg(Message txMsg){
    static uint8_t *packet;
    size_t lenght = (MIN_SIZE + txMsg.len);
    packet = new uint8_t[lenght];

    packet[0] = HEADER_DEBUT;
    packet[1] = txMsg.id;
    if(MsgAvecBlocINFOS(txMsg.id) == false){
        //Bloc sans INFOS
        packet[2] = 0; //checksum
        packet[3] = HEADER_FIN;
    }else{
        //Bloc avec INFOS
        uint8_t checksum = 0, dataCounter = 2;
        for (int i = 0; i < txMsg.len; i++)
        {
           packet[dataCounter] = txMsg.data[i];
           checksum ^= packet[dataCounter];
           dataCounter++;
        }
        packet[dataCounter++] = checksum; //checksum
        packet[dataCounter] = HEADER_FIN;
    }

    sendData(packet, lenght);

    delete[] packet; // Libérer la mémoire allouée pour les données
}

//Sans bloc INFOS
void CommunicationARAL::sendMsg(uint8_t id){
    Message txMsg = (Message){id, 0};
    sendMsg(txMsg);
}

//Avec bloc INFOS
void CommunicationARAL::sendMsg(uint8_t id, uint8_t len, uint8_t *data){
    Message txMsg;
    txMsg.id = id;
    txMsg.len = len;
    txMsg.data = new uint8_t[txMsg.len];
    for (int i = 0; i < txMsg.len; i++)
    {
        txMsg.data[i] = data[i];
    }
    sendMsg(txMsg);

    delete[] txMsg.data;
}


bool CommunicationARAL::MsgAvecBlocINFOS(uint8_t ID){
    bool blocAvecINFOS = false;

    // uC vers ARAL : 
    switch (ID)
    {
        case ID_POLLING_ARAL:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_CHECK_CAPTEURS:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_ORDRES:{//Avec bloc INFOS
            blocAvecINFOS = true;
        }
        break;
        case ID_PREMIERE_SCUTATION:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_DIFINITIVE_SCUTATION:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_RESET:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_DEMANDE_REPETITION_UC:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_INHIBITION_96_VOIES:{//Avec bloc INFOS
            blocAvecINFOS = true;
        }
        break;
    default:
        break;
    }


    //ARAL vers uC : 
    switch (ID)
    {
        case ID_ACKNOWLEDGE_POLLING:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_ACKNOWLEDGE_POLLING_DATA:{//Avec bloc INFOS
            blocAvecINFOS = true;
        }
        break;
        case ID_ACKNOWLEDGE_CHECK_CAPTEURS:{//Avec bloc INFOS
            blocAvecINFOS = true;
        }
        break;
        case ID_ACKNOWLEDGE_ORDRES:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_ACKNOWLEDGE_PREMIERE_SCRUTATION:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_ACKNOWLEDGE_RESET:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_DEMANDE_REPETITION_ARAL:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_ACKNOWLEDGE_INHIBITION:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
    
    default:
        break;
    }

    return blocAvecINFOS;
}

void CommunicationARAL::printMessage(Message msg){
      Serial.println ("*************************************************");
      Serial.println("Reception d'un nouveau message");
      Serial.printf("ID : %d", msg.id);
      if(msg.len){
        Serial.printf(", len : %d, data[%d] = ", msg.len, msg.len);
        for (int i = 0; i < msg.len; i++)
        {
            Serial.printf("[%X] ", msg.data[i]);
        }
      }
      Serial.println(".");
      Serial.println ("*************************************************");
}