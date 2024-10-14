#ifndef _General_LIB
#define _General_LIB

#include <Arduino.h>
#include <Commun.h>
#include <CommunicationARAL.h>
#include <CommunicationPC.h>
#include <IHM_secondVersion.h>


typedef struct etatVoieTestee{
    int voieActuelle;       //Numéro de la voie testée actuellement
    Tension alarmeActuelle;//Numéro de l'alarme testée actuellement
}etatVoieTestee;

class General
{
public:
    General(CommunicationARAL *com, CommunicationPC *pc, SelectionDeLaVoie *voie, CreateurTension *alarme);

    void IHMBegin();

    void run();

    int getNbRoundMade(){return nbToursFait;}

    void getTension(EtatVoies &voies, bool affichage = false);


private:
    CreateurTension *_alarme;
    SelectionDeLaVoie *_voie;
    CommunicationARAL *_com;
    CommunicationPC *_pc;

    enum GeneralState{
        INIT_COM,
        TEST_VOIES,
        BILAN,
    };
    GeneralState etat_gen;

    enum AralState{
        RESET,
        SELECTION_DE_VOIE,
        PREMIERE_SCRUTATION,
        CHECK_CAPTEURS,
        DIFINITIVE_SCRUTATION,
        VERIFICATION_BONNE_VOIE,
        ATT_ACK,
        FINISH,
        NONE,
    };

    EtatVoies _voies;
    int nbToursFait; // nombre de tours de tests avant le bilan
    EtatUneVoie _setUneVoie;

    void selectionVoie(Tension alarme, uint8_t voie = 1);

    void txManage();
    void ComBasicARAL(AralState etat);
    bool initialisationARAL();
    bool TestCarteARAL(BilanTest &bilan);
    void TestCarteARALSansVerification(EtatUneVoie voie, bool random = false);
    void TestContinueEnBoucleOuverte();
    bool ControleParMoniteurSerie();

    



};


#endif //_General_LIB