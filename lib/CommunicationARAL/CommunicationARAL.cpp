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
    ACK.id = 0;
    ACK.waitingAckFrom = 0;
    ACK.AckFrom_FLAG = false;
    ACK.RepeatRequest_FLAG = false;
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
        //Serial.printf("%2X ",byte);

        switch (currentState) {
            case WAITING_HEADER:{
                if (byte == HEADER_DEBUT) { // HEADER
                    currentState = RECEIVING_ID;
                    checksum ^= byte;
                }
                }
                break;

            case RECEIVING_ID:{
                rxMsg[FIFO_ecriture].id = byte;
                rxMsg[FIFO_ecriture].len = 0;
                if(MsgAvecBlocINFOS(rxMsg[FIFO_ecriture].id)){currentState = RECEIVING_NB;}
                else{currentState = RECEIVING_CHECKSUM;}
                checksum ^= byte;
                }break;

            case RECEIVING_NB:{
                if (rxMsg[FIFO_ecriture].data) {
                    delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée si elle existe
                    rxMsg[FIFO_ecriture].data = nullptr;
                }
                rxMsg[FIFO_ecriture].len = byte - 0x10;
                // Serial.println("rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];");
                rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];//Allouée de la mémoire
                currentState = RECEIVING_DATA;
                dataCounter = 0;
                checksum ^= byte;
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
                    Serial.printf("CommunicationARAL::onReceiveFunction() : Erreur calcul checksum. checksum calculé : %d (int), checksum reçu : %d (int) \n", checksum, byte);
                    currentState = WAITING_HEADER;
                }
                }break;

            case WAITING_FOOTER:{
                if (byte == HEADER_FIN) { // FOOTER
                    // Le message est complet
                    // printMessage(rxMsg[FIFO_ecriture]);
                    FIFO_ecriture = (FIFO_ecriture + 1) % SIZE_FIFO;
                }
                currentState = WAITING_HEADER;
                checksum = 0;
                // Serial.printf("delete[] rxMsg[FIFO_ecriture = %d].data;\n", FIFO_ecriture);
                // delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée, pour les prochaines données
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
    // printMessage(rxMsg[FIFO_lecture]);

    if ((ACK.waitingAckFrom == rxMsg[FIFO_lecture].id))
    {
        //Serial.printf(" ack recu \n");
        ACK.waitingAckFrom = 0;
        ACK.AckFrom_FLAG = true;
        ACK.RepeatRequest_FLAG = false;
    }
    switch (rxMsg[FIFO_lecture].id)
    {
        // case ID_ACKNOWLEDGE_POLLING:{
        //     //Sans bloc INFOS

        // }break;

        case ID_ACKNOWLEDGE_POLLING_DATA:{
            //Avec bloc INFOS

        }break;

        case ID_ACKNOWLEDGE_CHECK_CAPTEURS:{
            //Avec bloc INFOS
            for (int i = 0; i < rxMsg[FIFO_lecture].len; i++){
                if(i>=96){break;}
                etatVoies.voies[i] = rxMsg[FIFO_lecture].data[i];
            }
        }break;

        // case ID_ACKNOWLEDGE_ORDRES:{
        //     //Sans bloc INFOS

        // }break;

        // case ID_ACKNOWLEDGE_PREMIERE_SCRUTATION:{
        //     //Sans bloc INFOS

        // }break;

        // case ID_ACKNOWLEDGE_DIFINITIVE_SCRUTATION:{
        //     //Sans bloc INFOS

        // }break;

        // case ID_ACKNOWLEDGE_RESET:{
        //     //Sans bloc INFOS

        // }break;

        case ID_DEMANDE_REPETITION_ARAL:{
            //Sans bloc INFOS
            ACK.RepeatRequest_FLAG = true;
        }break;

        // case ID_ACKNOWLEDGE_INHIBITION:{
        //     //Sans bloc INFOS

        // }break;
    
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
        uint8_t checksum = packet[0] ^ packet[1];
        packet[2] = checksum; //checksum
        packet[3] = HEADER_FIN;
    }else{
        //Bloc avec INFOS
        uint8_t checksum = packet[0] ^ packet[1], dataCounter = 2;
        for (int i = 0; i < txMsg.len; i++)
        {
           packet[dataCounter] = txMsg.data[i];
           checksum ^= packet[dataCounter];
           dataCounter++;
        }
        packet[dataCounter++] = checksum; //checksum
        packet[dataCounter] = HEADER_FIN;
    }
    // for (int i = 0; i < lenght; i++)
    // {
        //Serial.printf(" %2X", packet[i]);
    // }
    //Serial.println("");
    
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

bool CommunicationARAL::checkACK(bool afterCkeck){
    if(ACK.AckFrom_FLAG){
       ACK.AckFrom_FLAG = afterCkeck;
       return true; 
    }
    return false;
}

bool CommunicationARAL::checkRepeatRequest(bool afterCkeck){
    if(ACK.RepeatRequest_FLAG){
       ACK.RepeatRequest_FLAG = afterCkeck;
       return true; 
    }
    return false;
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
        case ID_PREMIERE_SCRUTATION:{//Sans bloc INFOS
            blocAvecINFOS = false;
        }
        break;
        case ID_DIFINITIVE_SCRUTATION:{//Sans bloc INFOS
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
      Serial.printf("ID : %2X", msg.id);
      if(msg.len){
        //Serial.printf(", len : %d, data[%d] = ", msg.len, msg.len);
        for (int i = 0; i < msg.len; i++)
        {
            Serial.printf("[%2X] ", msg.data[i]);
        }
      }
      Serial.println(".");
      Serial.println ("*************************************************");
}