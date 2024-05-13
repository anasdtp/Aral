// #include <espQtCAN.h>
// #include <ident_club_tech.h>
// #include <Strategie.h>




// BluetoothSerial SerialBT;


// CANMessage rxMsgBLE[SIZE_FIFO];
// unsigned char FIFO_ecriture;

// CANMessage canBuffer[CAN_MAX];
// int canRempli = 0;
// int canVide = 0;
// int canPerdu = 0;

// uint8_t btBuffer[BT_MAX];
// int btRempli = 0;
// int btVide = 0;
// int btPerdu = 0;

// int etatBT = 0;
// CANMessage canBT;
// uint8_t checksumBT = 0;
// uint8_t lenBT = 0;

// uint8_t msgBT[15];
// int lenMsgBT = 0;


// void envoieBT(const CANMessage &msg)
// {
//   uint8_t checksum;
//   if (msg.format == CANStandard)
//   {
//     msgBT[0] = (msg.type == CANData) ? 0x55 : 0x5A;
//     msgBT[1] = (msg.id & 0x00000700) >> 8;
//     msgBT[2] = msg.id & 0x000000FF;
//     msgBT[3] = msg.len;
//     checksum = msgBT[0] + msgBT[1] + msgBT[2] + msgBT[3];
//     lenMsgBT = 4;
//     if (msg.type == CANData)
//     {
//       for (int i = 0; i < msg.len; i++)
//       {
//         msgBT[4 + i] = msg.data[i];
//         checksum += msg.data[i];
//       }
//       lenMsgBT += msg.len;
//     }
//   }
//   else
//   {
//     msgBT[0] = (msg.type == CANData) ? 0xA5 : 0xAA;
//     msgBT[1] = (msg.id & 0x1F000000) >> 24;
//     msgBT[2] = (msg.id & 0x00FF0000) >> 16;
//     msgBT[3] = (msg.id & 0x0000FF00) >> 8;
//     msgBT[4] = msg.id & 0x000000FF;
//     msgBT[5] = msg.len;
//     checksum = msgBT[0] + msgBT[1] + msgBT[2] + msgBT[3] + msgBT[4] + msgBT[5];
//     lenMsgBT = 6;
//     if (msg.type == CANData)
//     {
//       for (int i = 0; i < msg.len; i++)
//       {
//         msgBT[6 + i] = msg.data[i];
//         checksum += msg.data[i];
//       }
//       lenMsgBT += msg.len;
//     }
//   }
//   msgBT[lenMsgBT++] = ~checksum;
//   SerialBT.write(msgBT, lenMsgBT);

// }

// void btRead()
// {
//   while (SerialBT.available())
//   {
//     btBuffer[btRempli++] = SerialBT.read();

//     if (btRempli == BT_MAX)
//     {
//       btRempli = 0;
//     }
//     if (btRempli == btVide)
//     { // buffer plein on perd un caractère
//       btVide++;
//       if (btVide == BT_MAX) {
//         btVide = 0;
//       }
//       btPerdu++;
//     }
//   }
// }

// void btMachine()
// {
//   uint8_t c = btBuffer[btVide++];
//   if (btVide == BT_MAX)
//     btVide = 0;
//   switch (etatBT)
//   {
//   case 0:
//     checksumBT = c;
//     if (c == 0x55)
//     {
//       etatBT = 1;
//       canBT.format = CANStandard;
//       canBT.type = CANData;
//     }
//     else if (c == 0x5A)
//     {
//       etatBT = 1;
//       canBT.format = CANStandard;
//       canBT.type = CANRemote;
//     }
//     else if (c == 0xA5)
//     {
//       etatBT = 1;
//       canBT.format = CANExtended;
//       canBT.type = CANData;
//     }
//     else if (c == 0xAA)
//     {
//       etatBT = 1;
//       canBT.format = CANExtended;
//       canBT.type = CANRemote;
//     }
//     break;
//   case 1:
//     checksumBT += c;
//     canBT.id = c;
//     etatBT = 2;
//     break;
//   case 2:
//     checksumBT += c;
//     canBT.id = (canBT.id << 8) + c;
//     etatBT = (canBT.format == CANExtended) ? 3 : 5;
//     break;
//   case 3:
//     checksumBT += c;
//     canBT.id = (canBT.id << 8) + c;
//     etatBT = 4;
//     break;
//   case 4:
//     checksumBT += c;
//     canBT.id = (canBT.id << 8) + c;
//     etatBT = 5;
//     break;
//   case 5:
//     checksumBT += c;
//     canBT.len = c;
//     if (c > 8)
//     {
//       etatBT = 0; // erreur
//     }
//     else
//     {
//       etatBT = ((canBT.type == CANData) && (c != 0)) ? 6 : 7;
//       lenBT = 0;
//     }
//     break;
//   case 6:
//     checksumBT += c;
//     canBT.data[lenBT++] = c;
//     if (lenBT == canBT.len)
//     {
//       etatBT = 7;
//     }
//     break;
//   case 7:
//     checksumBT = (~checksumBT);
//     etatBT = 0;
//     if (c == checksumBT)
//     {
//       bool rtr = (canBT.type == CANRemote);
//       if (canBT.format == CANStandard) {
//         //CAN.beginPacket(canBT.id, canBT.len, rtr);        
//       } else {
//         //CAN.beginExtendedPacket(canBT.id, canBT.len, rtr);
//       }
//       if (!rtr) {
//         //CAN.write(canBT.data, canBT.len);
//       }
//       //CAN.endPacket();
//       printBLEStruct(canBT);
//       BTProcessRx(canBT);
//     }
//     break;
//   }
// }

// void canRead(int packetSize)
// {
//   // canBuffer[canRempli].id = CAN.packetId();
//   // canBuffer[canRempli].format = CAN.packetExtended() ? CANExtended : CANStandard;
//   // canBuffer[canRempli].len = CAN.packetDlc();
//   // if (CAN.packetRtr()) {
//   //   canBuffer[canRempli].type = CANRemote;
//   // } else {
//   //   canBuffer[canRempli].type = CANData;
//   //   int i = 0;
//   //   while (CAN.available()) {
//   //     canBuffer[canRempli].data[i++] = CAN.read();
//   //   }
//   // }
//   canRempli++;
//   if (canRempli == CAN_MAX)
//   {
//     canRempli = 0;
//   }
//   if (canRempli == canVide)
//   { // buffer plein on perd un message
//     canVide++;
//     if (canVide == CAN_MAX) {
//       canVide = 0;
//     }
//     canPerdu++;
//   }
// }

// void initCanBus()
// {/*
//   // start the CAN bus at 1000 kbps
//   while (!CAN.begin(1000E3))
//   {
//     Serial.println("Starting CAN failed!");
//   }
//   CAN.onReceive(canRead);*/
// }

// void BTQTsetup()
// {

//   SerialBT.begin("ESP32Qt");
// }

// void BTQtLoop()
// {
//   // put your main code here, to run repeatedly:
//   while (SerialBT.available()) {

//     btRead();
//   }
//   if (canRempli != canVide)
//   {

//     if (SerialBT.connected())
//     {

//       envoieBT(canBuffer[canVide]);
//       canVide++;
//       if (canVide == CAN_MAX)
//       {
//         canVide = 0;
//       }

//     } else {
//       Serial.printf("BT not connected\n");
//     }

//   }
//   if (btRempli != btVide)
//   {
//     btMachine();
//   }
// }

// void remplirStruct(CANMessage &txMsg, int idf, char lenf, char dt0f, char dt1f, char dt2f, char dt3f, char dt4f, char dt5f, char dt6f, char dt7f){
//   txMsg.type = CANData;
//   txMsg.format = CANStandard;

//   txMsg.id = idf;
//   txMsg.len = lenf;
//   txMsg.data[0] = dt0f;
//   txMsg.data[1] = dt1f;
//   txMsg.data[2] = dt2f;
//   txMsg.data[3] = dt3f;
//   txMsg.data[4] = dt4f;
//   txMsg.data[5] = dt5f;
//   txMsg.data[6] = dt6f;
//   txMsg.data[7] = dt7f;
// }

// void printBLEStruct(CANMessage msg){
//     printf("  ID      = 0x%.3x\n", msg.id);
//     printf("  Length  = %d\n", msg.len);
//     printf("  Data    = 0x");            
//     for(int i = 0 ; i < msg.len ; i++){printf(" %.2X", msg.data[i]);}
//     printf("\n");
//  }

// void BTProcessRx(CANMessage &rxMsg){
//     rxMsgBLE[FIFO_ecriture] = rxMsg;
//     FIFO_ecriture=(FIFO_ecriture+1)%SIZE_FIFO;
// }