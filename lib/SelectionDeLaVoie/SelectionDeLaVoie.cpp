#include "SelectionDeLaVoie.h"

//Les Voies 1 à 48 et 49 à 96 sont liées. Si on selectionne la voie 1, on selectionnera aussi la voie 49, et ainsi de suite
//Pour reellement ne selectionner qu'une seule voie, cela dependra de la tension créée avec CreateurTension.h

SelectionDeLaVoie::SelectionDeLaVoie(SelectionDeLaVoieBUS bus, SelectionDeLaVoieENABLEMUX enable)
{
    _bus = bus;
    _enable = enable;

    initOutput();
    disableMUX();
}

SelectionDeLaVoie::~SelectionDeLaVoie()
{
}

void SelectionDeLaVoie::initOutput(){
    pinMode(_bus.pinA, OUTPUT);    pinMode(_bus.pinB, OUTPUT);  pinMode(_bus.pinC, OUTPUT);

    pinMode(_enable.EN_MUX0, OUTPUT);   pinMode(_enable.EN_MUX1, OUTPUT);   pinMode(_enable.EN_MUX2, OUTPUT);
    pinMode(_enable.EN_MUX3, OUTPUT);   pinMode(_enable.EN_MUX4, OUTPUT);   pinMode(_enable.EN_MUX5, OUTPUT);

    digitalWrite(_bus.pinA, LOW);    digitalWrite(_bus.pinB, LOW); digitalWrite(_bus.pinC, LOW);

    digitalWrite(_enable.EN_MUX0, LOW); digitalWrite(_enable.EN_MUX1, LOW); digitalWrite(_enable.EN_MUX2, LOW);
    digitalWrite(_enable.EN_MUX3, LOW); digitalWrite(_enable.EN_MUX4, LOW); digitalWrite(_enable.EN_MUX5, LOW);

}

void SelectionDeLaVoie::setBus(uint8_t etat){
    if(etat>7){
        return;
    }
    bool etatPinA = (etat & 0x01)? HIGH : LOW;
    bool etatPinB = ((etat>>1) & 0x01)? HIGH : LOW;
    bool etatPinC = ((etat>>2) & 0x01)? HIGH : LOW;

    digitalWrite(_bus.pinA, etatPinA);   digitalWrite(_bus.pinB, etatPinB);   digitalWrite(_bus.pinC, etatPinC);      
    // Serial.printf("SelectionDeLaVoie::setBus : etat : %d, etatPinC : %d, etatPinB : %d, etatPinA : %d\n", etat, etatPinC, etatPinB, etatPinA);  
}

void SelectionDeLaVoie::enableMUX(uint8_t etat){//0, 1, 2, 3, 4 ou 5; Si etat>5, 6 = 0, 7 = 1...etc etat strictement inferieur à 12
    if(etat>11){
        return;
    }
    if(etat>5){
        etat -= 6;
    }
    bool A = (etat & 0x01)? HIGH : LOW;
    bool B = ((etat>>1) & 0x01)? HIGH : LOW;
    bool C = ((etat>>2) & 0x01)? HIGH : LOW;

    bool    E0 = (!C)&(!B)&(!A),
            E1 = (!C)&(!B)&( A),
            E2 = (!C)&( B)&(!A),
            E3 = (!C)&( B)&( A),
            E4 = ( C)&(!B)&(!A),
            E5 = ( C)&(!B)&( A);

    digitalWrite(_enable.EN_MUX0, E0);   digitalWrite(_enable.EN_MUX1, E1);   digitalWrite(_enable.EN_MUX2, E2);
    digitalWrite(_enable.EN_MUX3, E3);   digitalWrite(_enable.EN_MUX4, E4);   digitalWrite(_enable.EN_MUX5, E5);   
    // Serial.printf("SelectionDeLaVoie::enableMUX : etat : %d\n", etat); 
}

void SelectionDeLaVoie::disableMUX(bool disable){
    if(disable){
        digitalWrite(_enable.EN_MUX0, false);   digitalWrite(_enable.EN_MUX1, false);   digitalWrite(_enable.EN_MUX2, false);
        digitalWrite(_enable.EN_MUX3, false);   digitalWrite(_enable.EN_MUX4, false);   digitalWrite(_enable.EN_MUX5, false);   
        // Serial.printf("SelectionDeLaVoie::disableMUX\n"); 
    }
}

//voie entre 1 et 48; si au dessus de 48, 49 = 1, 50 = 2, ...etc Cela doit donc etre strictement inferieur à 97
void SelectionDeLaVoie::selectionVoie(uint8_t voie){
    if(voie>96 || voie == 0){
        return;
    }
    if(voie>48){
        voie -= 48;
    }
    voie -= 1;//passe de 48 à 47, de 47 à 46...etc, pour les calculs
    //0~7   15  23  31  39  47
    //          /8 ->
    //  0   1   2   3   4   5
    uint8_t MUXChoisi = (uint8_t)(voie/8);
    enableMUX(MUXChoisi);

    uint8_t voieMUX = voie - (MUXChoisi * 8);
    setBus(voieMUX);
}