
#include <Arduino.h>
#include <BluetoothSerial.h>



// Enlever le commentaire ci-dessous pour passer de BT à USB
//#define MODE_USB

#define CAN_MAX 256
#define BT_MAX 1024

#define SIZE_FIFO 32


typedef enum
{
  CANStandard = 0,
  CANExtended = 1,
  CANAny = 2
} CANFormat;

typedef enum
{
  CANData = 0,
  CANRemote = 1
} CANType;

typedef struct
{
  unsigned long id;      // 29 bit identifier
  unsigned char data[8]; // Data field
  unsigned char len;     // Length of data field in bytes
  CANFormat format;      // Format ::CANFormat
  CANType type;          // Type ::CANType
} CANMessage;

extern CANMessage rxMsgBLE[SIZE_FIFO];
extern unsigned char FIFO_ecriture;

void envoieBT(const CANMessage &msg);
void btRead();
void btMachine();
void canRead(int packetSize);
void initCanBus();
void BTQTsetup();
void BTQtLoop();
void remplirStruct(CANMessage &txMsg, int idf, char lenf, char dt0f, 
                         char dt1f, char dt2f, char dt3f, char dt4f, 
                         char dt5f, char dt6f, char dt7f);
void printBLEStruct(CANMessage msg);

void BTProcessRx(CANMessage &rxMsg);