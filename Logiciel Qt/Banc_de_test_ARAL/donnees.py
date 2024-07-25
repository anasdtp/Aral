
import struct

SERIAL_BAUDRATE = 921600

ID_NB_TOURS     = 0xA0 #On envoi le nombre de tours à faire sur 2 octets
ID_ACK_NB_TOURS = 0xA1 #RIEN

ID_INITIALISATION_ARAL_EN_COURS = 0xB0 #Reception du nombre de tentative de com
ID_INITIALISATION_ARAL_FAITE    = 0xB1 #RIEN
ID_TEST_EN_COURS = 0xB2 #Reception du bilan de test du tours effectué
ID_TEST_TERMINEE = 0xB3 #Reception du bilan de test
ID_ETAT_VOIES = 0xB4 #Reception de l'etat des voies directement de la carte ARAL
ID_ETAT_UNE_VOIE = 0xB5 #Reception de l'etat d'une voie en defaut ou ok avec data[0]->(Num de la voie) et data[1]->(Etat, OK=0x30, Defaut = 0x10, non testée = 0)
ID_CARTE_ARAL_NE_REPOND_PLUS = 0xB6
ID_CARTE_ARAL_REPEAT_REQUEST = 0xB7
ID_TEST_TEMPS_DE_REPONSE_FILTRAGE = 0xB8 #Si jamais le filtrage est activé, Envoi du temps de réponse des 96 voies, tableau de 96 cases
ID_TEST_TEMPS_DE_REPONSE_FILTRAGE_UNE_VOIE = 0xB9 #Envoi du temps de reponse d'une seule voie avec data[0]->(Num de la voie) et data[1]->dizieme de secondes

ID_ACK_GENERAL      = 0xC0
ID_RELANCER_TEST    = 0xC1
ID_ARRET_TEST       = 0xC2
ID_REPEAT_REQUEST   = 0xD0
ID_REQUEST_NB_TOURS_FAIT = 0xD1
ID_ACK_REQUEST_NB_TOURS_FAIT = 0xD2
ID_REQUEST_BILAN = 0xD3
ID_SET_FILTRAGE = 0xD4 #Filtrage des alarmes en fonction des switch SW2. Filtrage = etat stable pendant un temps desiré (etat stable = ne prends pas en compte les changements)

idComEnText = {
    0 : "",
    0xA0 : "ID_NB_TOURS",
    0xA1 : "ID_ACK_NB_TOURS",
    0xB0 : "ID_INITIALISATION_ARAL_EN_COURS",
    0xB1 : "ID_INITIALISATION_ARAL_FAITE",
    0xB2 : "ID_TEST_EN_COURS",
    0xB3 : "ID_TEST_TERMINEE",
    0xB4 : "ID_ETAT_VOIES",
    0xB5 : "ID_ETAT_UNE_VOIE",
    0xB6 : "ID_CARTE_ARAL_NE_REPOND_PLUS",
    0xB7 : "ID_CARTE_ARAL_REPEAT_REQUEST",
    0xB8 : "ID_TEST_TEMPS_DE_REPONSE_FILTRAGE",
    0xB9 : "ID_TEST_TEMPS_DE_REPONSE_FILTRAGE_UNE_VOIE",
    0xC0 : "ID_ACK_GENERAL",
    0xC1 : "ID_RELANCER_TEST",
    0xC2 : "ID_ARRET_TEST",
    0xD0 : "ID_REPEAT_REQUEST",
    0xD1 : "ID_REQUEST_NB_TOURS_FAIT",
    0xD2 : "ID_ACK_REQUEST_NB_TOURS_FAIT",
    0xD3 : "ID_REQUEST_BILAN",
    0xD4 : "ID_SET_FILTRAGE",
}

class Message():
    def __init__(self, id=0, length=0, data=None, checksum=0):
        self.id = id
        self.len = length
        self.data = data if data else []
        self.checksum = checksum

    def build_packet(self):
        # Calculate checksum as a simple example
        self.checksum = (self.id ^ self.len) & 0xFF
        for i in range(self.len):
            self.checksum ^= self.data[i]
        length = self.len if(self.len) else 1
        # Construct the packet with start marker, ID, length, data, checksum, and end marker
        packet_format = f'<B B B {length}s B B'
        packet_data = bytes(self.data)
        return struct.pack(packet_format, 0xFF, self.id, self.len, packet_data, self.checksum, 0xFF)

SIZE_FIFO = 32
class COMMUNICATION():
    def __init__(self):
        self.rxMsg = [Message() for _ in range(SIZE_FIFO)]
        self.FIFO_Ecriture = 0
        self.serial_thread = None
        self.ecritureEnCours = False #Flag pour faire savoir qu'on a lancé une ecriture
        self.problemeEnEcriture = False #Flag pour dire que le serial.write n'a pas fonctionné
com = COMMUNICATION()

NOMBRE_VOIES = 96

etatBilan = {
    "test non fait" : 0,
    "OK" : 0x30,
    "DEFAUT" : 0x10,
}
etatVoies = {
    "COURT_CIRCUIT" : 0,
    "ALARME" : 1,
    "NORMAL" : 2,
    "CONGRUENCE" : 3,
    "NONE" : 4,
}
class VOIE():
    def __init__(self):
        self.voies = [etatVoies["NONE"] for _ in range(NOMBRE_VOIES)]
        self.bilan = [etatBilan["test non fait"] for _ in range(NOMBRE_VOIES)]
        self.perteDeCom = 0
        self.tempsDeReponse = [0.0 for _ in range(NOMBRE_VOIES)] #reçu en diziéme de secondes, convertit lors du stockage ici en secondes
        self.nombreDeTourFait = 0
voies = VOIE()