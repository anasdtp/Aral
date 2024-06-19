#include "CommunicationPC.h"
#include <functional>
#include <IHM.h>

// Macros : 
#define sendData(packet, length)  	(_serial->write(packet, length))    // Write Over Serial
#define availableData() 			(_serial->available())    			// Check Serial Data Available
#define readData()      			(_serial->read())         			// Read Serial Data
#define peekData()      			(_serial->peek())         			// Peek Serial Data
#define beginCom(args)  			(_serial->begin(args))   			// Begin Serial Comunication
#define endCom()        			(_serial->end())          			// End Serial Comunication

#define sendDataBT(packet, length)  (_serialBT->write(packet, length))      // Write Over Serial
#define availableDataBT() 			(_serialBT->available())    	    	// Check Serial Data Available
#define readDataBT()      			(_serialBT->read())         			// Read Serial Data
#define peekDataBT()      			(_serialBT->peek())         			// Peek Serial Data
#define beginComBT(args)  			(_serialBT->begin(args))   			    // Begin Serial Comunication
#define endComBT()        			(_serialBT->end())          			// End Serial Comunication

CommunicationPC* CommunicationPC::instance = nullptr;  // Initialize the static instance pointer

CommunicationPC::CommunicationPC(/* args */)
{
    FIFO_ecriture = 0;
}

void CommunicationPC::begin(HardwareSerial *srl, long baud, String nameBT){
    instance = this;  // Set the instance pointer to the current object
    _serial = srl;
    _serialBT = new BluetoothSerial();
    beginCom(baud);
    beginComBT(nameBT);
    _serial->onReceive(std::bind(&CommunicationPC::onReceiveFunction, this));
    _serialBT->register_callback(onReceiveFunctionBTStatic);

    setNombreTours(1);
    _resetTestRequest = false; _StopTestRequest = false;
}

void CommunicationPC::end()
{
    endCom();
    endComBT();
    instance = nullptr;
}

void CommunicationPC::onReceiveFunction(void) {

    static StateRx currentState = WAITING_HEADER;
    static uint8_t dataCounter = 0;

    while (availableData()) {
        uint8_t byte = readData();
        // Serial.printf("%2X ",byte);
        // _serial->write(byte);
        switch (currentState) {
            case WAITING_HEADER:{
                if (byte == 0xFF) { // HEADER
                    currentState = RECEIVING_ID;
                    rxMsg[FIFO_ecriture].checksum = 0;
                }
                }
                break;

            case RECEIVING_ID:{
                rxMsg[FIFO_ecriture].id = byte;
                rxMsg[FIFO_ecriture].len = 0;
                currentState = RECEIVING_LEN;
                rxMsg[FIFO_ecriture].checksum ^= byte;
                }break;

            case RECEIVING_LEN:{
                if (rxMsg[FIFO_ecriture].data) {
                    delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée si elle existe
                    rxMsg[FIFO_ecriture].data = nullptr;
                }
                rxMsg[FIFO_ecriture].len = byte;
                // Serial.println("rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];");
                rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];//Allouée de la mémoire
                currentState = RECEIVING_DATA;
                dataCounter = 0;
                rxMsg[FIFO_ecriture].checksum ^= byte;
                }break;

            case RECEIVING_DATA:{
                rxMsg[FIFO_ecriture].data[dataCounter++] = byte;
                rxMsg[FIFO_ecriture].checksum ^= byte;//XOR
                if (dataCounter >= rxMsg[FIFO_ecriture].len) {
                    currentState = RECEIVING_CHECKSUM;
                }
                }break;

            case RECEIVING_CHECKSUM:{
                if (byte == rxMsg[FIFO_ecriture].checksum) {
                    currentState = WAITING_FOOTER;
                } else {
                    // Gérer l'erreur de checksum ici
                    // Serial.printf("CommunicationARAL::onReceiveFunction() : Erreur calcul checksum. checksum calculé : %d (int), checksum reçu : %d (int) \n", checksum, byte);
                    sendMsg(ID_REPEAT_REQUEST);
                    currentState = WAITING_HEADER;
                }
                }break;

            case WAITING_FOOTER:{
                if (byte == 0xFF) { // FOOTER
                    // Le message est complet
                    // printMessage(rxMsg[FIFO_ecriture]);
                    FIFO_ecriture = (FIFO_ecriture + 1) % SIZE_FIFO;
                    // sendMsg(ID_ACK_GENERAL);
                }
                currentState = WAITING_HEADER;
                rxMsg[FIFO_ecriture].checksum = 0;
                // Serial.printf("delete[] rxMsg[FIFO_ecriture = %d].data;\n", FIFO_ecriture);
                // delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée, pour les prochaines données
                }break;
        }
    }
}

void CommunicationPC::onReceiveFunctionBTStatic(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
    if(instance){
        instance->onReceiveFunctionBT(event, param);  // Call the member function on the current instance
    }
}

void CommunicationPC::onReceiveFunctionBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    static StateRx currentState = WAITING_HEADER;
    static uint8_t dataCounter = 0;

    while (availableDataBT()) {
        uint8_t byte = readDataBT();
        // Serial.printf("%2X ",byte);
        // _serial->write(byte);
        switch (currentState) {
            case WAITING_HEADER:{
                if (byte == 0xFF) { // HEADER
                    currentState = RECEIVING_ID;
                    rxMsg[FIFO_ecriture].checksum = 0;
                }
                }
                break;

            case RECEIVING_ID:{
                rxMsg[FIFO_ecriture].id = byte;
                rxMsg[FIFO_ecriture].len = 0;
                currentState = RECEIVING_LEN;
                rxMsg[FIFO_ecriture].checksum ^= byte;
                }break;

            case RECEIVING_LEN:{
                if (rxMsg[FIFO_ecriture].data) {
                    delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée si elle existe
                    rxMsg[FIFO_ecriture].data = nullptr;
                }
                rxMsg[FIFO_ecriture].len = byte;
                // Serial.println("rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];");
                rxMsg[FIFO_ecriture].data = new uint8_t[rxMsg[FIFO_ecriture].len];//Allouée de la mémoire
                currentState = RECEIVING_DATA;
                dataCounter = 0;
                rxMsg[FIFO_ecriture].checksum ^= byte;
                }break;

            case RECEIVING_DATA:{
                rxMsg[FIFO_ecriture].data[dataCounter++] = byte;
                rxMsg[FIFO_ecriture].checksum ^= byte;//XOR
                if (dataCounter >= rxMsg[FIFO_ecriture].len) {
                    currentState = RECEIVING_CHECKSUM;
                }
                }break;

            case RECEIVING_CHECKSUM:{
                if (byte == rxMsg[FIFO_ecriture].checksum) {
                    currentState = WAITING_FOOTER;
                } else {
                    // Gérer l'erreur de checksum ici
                    // Serial.printf("CommunicationARAL::onReceiveFunction() : Erreur calcul checksum. checksum calculé : %d (int), checksum reçu : %d (int) \n", checksum, byte);
                    sendMsg(ID_REPEAT_REQUEST);
                    currentState = WAITING_HEADER;
                }
                }break;

            case WAITING_FOOTER:{
                if (byte == 0xFF) { // FOOTER
                    // Le message est complet
                    // printMessage(rxMsg[FIFO_ecriture]);
                    FIFO_ecriture = (FIFO_ecriture + 1) % SIZE_FIFO;
                    // sendMsg(ID_ACK_GENERAL);
                }
                currentState = WAITING_HEADER;
                rxMsg[FIFO_ecriture].checksum = 0;
                // Serial.printf("delete[] rxMsg[FIFO_ecriture = %d].data;\n", FIFO_ecriture);
                // delete[] rxMsg[FIFO_ecriture].data; // Libérer la mémoire allouée, pour les prochaines données
                }break;
        }
    }
}

//Doit etre dans la loop
void CommunicationPC::RxManage(){
    static signed char FIFO_lecture = 0, FIFO_occupation = 0, FIFO_max_occupation = 0;

    FIFO_occupation = FIFO_ecriture - FIFO_lecture;
    if(FIFO_occupation<0){FIFO_occupation=FIFO_occupation+SIZE_FIFO;}
    if(FIFO_max_occupation<FIFO_occupation){FIFO_max_occupation=FIFO_occupation;}
    if(!FIFO_occupation){return;}
    //Alors il y a un nouveau message en attente de traitement
    // printMessage(rxMsg[FIFO_lecture]);

    switch (rxMsg[FIFO_lecture].id)
    {
        case ID_NB_TOURS:{
            uint16_t nbTours = rxMsg[FIFO_lecture].data[1]<<8 | rxMsg[FIFO_lecture].data[0];
            setNombreTours(nbTours);
            printMidOLED(("Nombre de Tours :\n" + String(nbTours)), 2, 1);

            sendMsg(ID_ACK_NB_TOURS);
        }break;

        case ID_RELANCER_TEST:{
            _resetTestRequest = true;
            printMidOLED(("Relancement du test"), 2, 1);

            sendMsg(ID_ACK_GENERAL);
        }break;

        case ID_ARRET_TEST:{
            _StopTestRequest = true;
            printMidOLED(("Arret du test"), 2, 1);

            sendMsg(ID_ACK_GENERAL);
        }break;

        default:
            sendMsg(ID_ACK_GENERAL);
            break;
    }

    FIFO_lecture = (FIFO_lecture + 1) % SIZE_FIFO;
}

void CommunicationPC::sendMsg(Message txMsg){
    static uint8_t *packet;
    size_t lenght = 5;//les 2 Header, l'id, la len, le checksum, soit la taille minimal sans l'octet data
    if(txMsg.len){
        lenght += txMsg.len;
    }else{
        lenght += 1;//La taille minimal pour la data, meme si il y en a pas.
    }
    packet = new uint8_t[lenght];

    packet[0] = 0xFF;//Header
    packet[1] = txMsg.id;
    packet[2] = txMsg.len;//len
    txMsg.checksum = packet[1] ^ packet[2];
    uint8_t dataCounter = 3;
    for (int i = 0; i < txMsg.len; i++)
    {
        packet[dataCounter] = txMsg.data[i];
        txMsg.checksum ^= packet[dataCounter];
        dataCounter++;
    }
    if(!txMsg.len){packet[dataCounter] = 0; dataCounter++;}
    packet[dataCounter++] = txMsg.checksum; // checksum
    packet[dataCounter] = 0xFF;//Header
    
    sendData(packet, lenght);
    sendDataBT(packet, lenght);

    delete[] packet; // Libérer la mémoire allouée pour les données
}

void CommunicationPC::sendMsg(uint8_t id){
    Message txMsg = (Message){id, 0};
    txMsg.data = new uint8_t[1];
    txMsg.data[0] = 0;
    sendMsg(txMsg);
}

void CommunicationPC::sendMsg(uint8_t id, uint8_t len, uint8_t *data){
    Message txMsg = (Message){id, len};
    txMsg.data = new uint8_t[txMsg.len];
    for (int i = 0; i < txMsg.len; i++)
    {
        txMsg.data[i] = data[i];
    }
    sendMsg(txMsg);

    delete[] txMsg.data;
}

void CommunicationPC::sendMsg(uint8_t id, uint8_t octet){
    const uint8_t len = 1;
    uint8_t data[len] = {octet};
    sendMsg(id, len, data);
}

void CommunicationPC::sendMsg(uint8_t id, uint16_t nb){
    const uint8_t len = 2;
    uint8_t data[len];
    data[0] = (uint8_t)((nb)    &0xFF);
    data[1] = (uint8_t)((nb>>8) &0xFF);
    sendMsg(id, len, data);
}

void CommunicationPC::sendMsg(uint8_t id, uint32_t nb){
    const uint8_t len = 4;
    uint8_t data[len];
    data[0] = (uint8_t)((nb)    &0xFF);
    data[1] = (uint8_t)((nb>>8) &0xFF);
    data[2] = (uint8_t)((nb>>16)&0xFF);
    data[3] = (uint8_t)((nb>>24)&0xFF);
    sendMsg(id, len, data);
}

void CommunicationPC::sendMsg(uint8_t id, BilanTest &bilan){
    const uint8_t len = 96;
    uint8_t data[len];
    memcpy(data, bilan.voies, len);
    sendMsg(id, len, data);
}

void CommunicationPC::sendMsg(uint8_t id, EtatVoies &voies){
    const uint8_t len = 96;
    uint8_t data[len];
    memcpy(data, voies.voies, len);
    sendMsg(id, len, data);
}




void CommunicationPC::printMessage(Message msg){
      Serial.println ("*************************************************");
      Serial.println("Reception d'un nouveau message");
      Serial.printf("ID : %2X, data[%d] = ", msg.id, msg.len);
      if(msg.len){
        //Serial.printf(", len : %d, data[%d] = ", msg.len, msg.len);
        for (int i = 0; i < msg.len; i++)
        {
            Serial.printf("[%2X] ", msg.data[i]);
        }
      }
      Serial.printf("\nchecksum : %2X.\n", msg.checksum);
    //   Serial.println(".");
      Serial.println ("*************************************************");
}