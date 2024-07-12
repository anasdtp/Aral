#include "CreateurTension.h"

CreateurTension::CreateurTension(CreateurTensionBUS Bus1, CreateurTensionBUS Bus2)
{
    _Bus1 = Bus1; _Bus2 = Bus2;

    initOutput();

    creationTensionVoie_1_48(COURT_CIRCUIT);
    creationTensionVoie_49_96(COURT_CIRCUIT);
}

CreateurTension::~CreateurTension()
{
}
void CreateurTension::initOutput(){
    pinMode(_Bus1.pinA, OUTPUT);    pinMode(_Bus1.pinB, OUTPUT);
    pinMode(_Bus2.pinA, OUTPUT);    pinMode(_Bus2.pinB, OUTPUT);

    digitalWrite(_Bus1.pinA, LOW);    digitalWrite(_Bus1.pinB, LOW);
    digitalWrite(_Bus2.pinA, LOW);    digitalWrite(_Bus2.pinB, LOW);
}

void CreateurTension::setBus(CreateurTensionBUS bus, uint8_t etat){
    bool etatPinA = (etat & 0x01)? HIGH : LOW;
    bool etatPinB = ((etat>>1) & 0x01)? HIGH : LOW;
    digitalWrite(bus.pinA, etatPinA);    digitalWrite(bus.pinB, etatPinB);
    // Serial.printf("CreateurTension::setBus : etat : %d, etatPinB : %d, etatPinA : %d\n", etat, etatPinB, etatPinA);
}

void CreateurTension::creationTensionVoie_1_48(Tension tension){
    setBus(_Bus1, tension);
}

void CreateurTension::creationTensionVoie_49_96(Tension tension){
    setBus(_Bus2, tension);
}