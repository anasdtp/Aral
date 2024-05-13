// #include "BTCom.h"

// // Macros : 
//     // Serial 
// #define sendData(packet, length)  	(_serial->write(packet, length))    // Write Over Serial
// #define flush()						(_serial->flush())					// Wait until buffer empty
// #define availableData() 			(_serial->available())    			// Check Serial Data Available
// #define readData()      			(_serial->read())         			// Read Serial Data
// #define peekData()      			(_serial->peek())         			// Peek Serial Data
// #define beginCom(args)  			(_serial->begin(args))   			// Begin Serial Comunication
// #define endCom()        			(_serial->end())          			// End Serial Comunication
//     //BT
// #define sendDataBT(packet, length)  (_serialBT->write(packet, length))  // Write Over BT
// #define connectedBT()				(_serialBT->connected())					
// #define availableDataBT() 			(_serialBT->available())    		// Check BT Data Available
// #define readDataBT()      			(_serialBT->read())         		// Read BT Data
// #define beginBT(name)  			    (_serialBT->begin(name))   			// Begin BT Comunication
// #define endBT()        			    (_serialBT->end())          		// End BT Comunication


// BTCom::BTCom(/* args */)
// {
// }

// BTCom::~BTCom()
// {
// }

// //begin with serial com
// void BTCom::begin(HardwareSerial *srl = &Serial, long baud = 115200){
//     _serial = srl;
//     beginCom(baud);
//     // _serial->onReceive(std::bind(&CommunicationARAL::onReceiveFunction, this));
// }

// //begin with BT com
// void BTCom::begin(String name){
//     _serialBT = new(BluetoothSerial);
//     beginBT(name);
//     // _serial->onReceive(std::bind(&CommunicationARAL::onReceiveFunction, this));
// }

// void BTCom::end(){

// }