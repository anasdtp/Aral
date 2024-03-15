#include <Arduino.h>

typedef struct CreateurTensionBUS
{
    int pinA, pinB;
}CreateurTensionBUS;

enum Tension{
    COURT_CIRCUIT = 0,//0V  //00
    ALARME = 1,//3.3V       //01
    NORMAL = 2,//6V         //10
    CONGRUENCE = 3//15V     //11
};

class CreateurTension
{
public:
    CreateurTension(CreateurTensionBUS Bus1, CreateurTensionBUS Bus2);
    ~CreateurTension();
    void setBus(CreateurTensionBUS bus, uint8_t etat);
    void creationTensionVoie_1_48(Tension tension);
    void creationTensionVoie_49_96(Tension tension);
private:
    CreateurTensionBUS _Bus1, _Bus2;

    void initOutput();
};


