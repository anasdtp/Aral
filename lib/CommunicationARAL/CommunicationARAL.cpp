#include "CommunicationARAL.h"
#include <functional>
// Macros : 
#define sendData(packet, length)  	(_serial->write(packet, length))    	// Write Over Serial
#define flush()						(_serial->flush())					// Wait until buffer empty
#define availableData() 			(_serial->available())    			// Check Serial Data Available
#define readData()      			(_serial->read())         			// Read Serial Data
#define peekData()      			(_serial->peek())         			// Peek Serial Data
#define beginCom(args)  			(_serial->begin(args))   				// Begin Serial Comunication
#define endCom()        			(_serial->end())          			// End Serial Comunication



CommunicationARAL::CommunicationARAL(HardwareSerial *srl, long baud)
{
    _serial = srl;
    beginCom(baud);
    FIFO_ecriture = 0;
    _serial->onReceive(std::bind(&CommunicationARAL::onReceiveFunction, this));
}

CommunicationARAL::~CommunicationARAL()
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
                checksum ^= byte;
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
                // delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée pour les données
                }break;
        }
    }
}

bool CommunicationARAL::MsgAvecBlocINFOS(uint8_t ID){
    bool blocAvecINFOS = false;
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
