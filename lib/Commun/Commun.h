#ifndef _COMMUN_LIB
#define _COMMUN_LIB
#include <Arduino.h>
#include <CreateurTension.h>
#include <SelectionDeLaVoie.h>



typedef struct EtatVoies{
    Tension voies[96];
}EtatVoies;

#define VOIE_OK 0x30
#define VOIE_EN_DEFAUT 0x10
#define VOIE_NONE 0
typedef struct BilanTest{
  uint8_t voies[96]; //Si VOIE_OK, voie fonctionnelle, si VOIE_EN_DEFAUT voie HS
  int nbToursFait;
  uint8_t tempsReponse[96];//Si filtrage activé, temps de reponse de chaque voies, //En dizieme de seconde car la valeur est sur un octet, soit 255 au max
}BilanTest;


typedef struct Message{
    uint8_t id;
    uint8_t len;
    uint8_t *data;
    uint8_t checksum;
    // uint8_t checksum;
}Message;
#define SIZE_FIFO 32 //maximum 150 du fait du type char


#endif //_COMMUN_LIB