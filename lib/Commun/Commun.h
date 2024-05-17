#ifndef _COMMUN_LIB
#define _COMMUN_LIB
#include <Arduino.h>
#include <CreateurTension.h>
#include <SelectionDeLaVoie.h>
#include <CommunicationARAL.h>

typedef struct EtatVoies{
    Tension voies[96];
}EtatVoies;

#define VOIE_OK 0x30
#define VOIE_EN_DEFAUT 0x10
#define VOIE_NONE 0
typedef struct BilanTest{
  uint8_t voies[96]; //Si VOIE_OK, voie fonctionnelle, si VOIE_EN_DEFAUT voie HS
}BilanTest;


#endif //_COMMUN_LIB