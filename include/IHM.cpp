#include "IHM.h"

#ifndef min
  #define min(a,b) ((a < b)?(a): (b))
#endif

#ifndef max
  #define max(a,b) ((a > b)?(a): (b))
#endif

Adafruit_NeoPixel WS2812B(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void IHM_begin(){
    Wire.begin(SDA_OLED_PIN, SCL_OLED_PIN, 400000);
    delay(250); // wait for the OLED to power up
    display.begin(i2c_Address, true); // Address 0x3C default
    display.display();
    WS2812B.begin();
}

void printMidOLED(String text, int textSize, int y) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(SH110X_WHITE);
  
  int charWidth = 6 * textSize;  // Largeur approximative d'un caractère en pixels
  int maxCharsPerLine = SCREEN_WIDTH / charWidth;  // Nombre maximum de caractères par ligne
  
  int lineHeight = 8 * textSize;  // Hauteur approximative d'une ligne de texte en pixels
  
  // Découper le texte en lignes en fonction des retours à la ligne (\n)
  int startIdx = 0;
  int currentY = y;
  while (startIdx < text.length()) {
    int endIdx = text.indexOf('\n', startIdx);
    if (endIdx == -1) {
      endIdx = text.length();
    }
    String line = text.substring(startIdx, endIdx);

    // Si la ligne est trop longue, découper en plusieurs sous-lignes
    while (line.length() > maxCharsPerLine) {
      String subLine = line.substring(0, maxCharsPerLine);
      int x = (SCREEN_WIDTH - (subLine.length() * charWidth)) / 2;
      display.setCursor(x, currentY);
      display.println(subLine);
      currentY += lineHeight;
      line = line.substring(maxCharsPerLine);
    }
    
    // Afficher la ligne ou la sous-ligne restante
    int x = (SCREEN_WIDTH - (line.length() * charWidth)) / 2;
    display.setCursor(x, currentY);
    display.println(line);
    currentY += lineHeight;

    // Passer au prochain segment après le retour à la ligne
    startIdx = endIdx + 1;
  }
  
  display.display();
}


void displayEtatVoie(int numVoie, String alarme, uint8_t etat){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Etat Voie");
  display.setTextSize(2);
  display.println(numVoie);
  display.setTextSize(1);
  display.print("alarme: ");
  display.println(alarme);
  display.setTextSize(3);
  if(etat == VOIE_OK){
    display.print("OK");
  }
  else if(etat == VOIE_EN_DEFAUT){
    display.print("HS !!");
    display.invertDisplay(true);
  }
  else{
    display.print("idk");
  }
  display.display();
}

void displayBilanTest(BilanTest voies) {
  static uint32_t startTimeVoie = 0;
  const uint32_t TempsSwitchVoie = 3000;
  static int page = 0;
  const int NB_PAGES = 6;
  const int VOIES_PAR_PAGE = 16;

  if (millis() - startTimeVoie > TempsSwitchVoie) {
    startTimeVoie = millis(); // Reset the timer

    display.clearDisplay();
    display.setTextSize(1);
    display.invertDisplay(false);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("Bilan Test");

    int startVoie = page * VOIES_PAR_PAGE;
    int endVoie = startVoie + VOIES_PAR_PAGE;

    int nbMotParLigne = 0;
    for (int i = startVoie; i < endVoie; i++) {
      if(nbMotParLigne > 4){nbMotParLigne=0; display.println();}else{nbMotParLigne++;}
      display.printf("%2d:", i + 1);
      switch (voies.voies[i]) {
        case VOIE_OK:
          display.print("OK, ");
          break;
        case VOIE_EN_DEFAUT:
          display.setTextColor(SH110X_BLACK, SH110X_WHITE);
          display.print("HS");
          display.setTextColor(SH110X_WHITE);
          display.print(", ");
          break;
        default:
          display.print("~ , ");
          break;
      }
    }

    display.display();
    page = (page + 1) % NB_PAGES; // Passer à la page suivante
  }
}

void setLedColor(int numberOfLed, uint8_t couleur[3]){
  WS2812B.clear();
  numberOfLed = numberOfLed%(NUM_PIXELS+1);
  for (int i = 0; i < numberOfLed; i++)
  {
    WS2812B.setPixelColor(i, WS2812B.Color(couleur[0], couleur[1], couleur[2]));
  }
  WS2812B.show();
}

void testLed(){
  WS2812B.clear();  // set all pixel colors to 'off'. It only takes effect if pixels.show() is called

  // turn pixels to green one-by-one with delay between each pixel
  for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {         // for each pixel
    WS2812B.setPixelColor(pixel, WS2812B.Color(0, 255, 0));  // it only takes effect if pixels.show() is called
    WS2812B.show();                                          // update to the WS2812B Led Strip

    delay(500);  // 500ms pause between each pixel
  }

  // turn off all pixels for two seconds
  WS2812B.clear();
  WS2812B.show();  // update to the WS2812B Led Strip
  delay(2000);     // 2 seconds off time

  // turn on all pixels to red at the same time for two seconds
  for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {         // for each pixel
    WS2812B.setPixelColor(pixel, WS2812B.Color(255, 0, 0));  // it only takes effect if pixels.show() is called
  }
  WS2812B.show();  // update to the WS2812B Led Strip
  delay(1000);     // 1 second on time

  // turn off all pixels for one seconds
  WS2812B.clear();
  WS2812B.show();  // update to the WS2812B Led Strip
  delay(1000);     // 1 second off time
}

void rainbow(uint8_t waitms) {
  static uint32_t start_time = 0; static long firstPixelHue = 0;
  if((millis()-start_time)>waitms){
    // Serial.println("yes");
      start_time = millis();
      WS2812B.rainbow(firstPixelHue, -1, 255, 100);
      WS2812B.show();
      firstPixelHue = (firstPixelHue + 256)%(3*65536);
    //   vTaskDelay(pdMS_TO_TICKS(waitms));
  }
}

void TransitionDelay(int waitms){
  static uint32_t start_time = 0;
  start_time = millis();
  while((millis()-start_time)<waitms){
    rainbow(10);
  }
}


uint8_t GREEN[3] = {0, 255 / 2, 0};
uint8_t BLUE[3] = {0, 0, 255 / 2};
uint8_t RED[3] = {255 / 2, 0, 0};
uint8_t WHITE[3] = {255 / 3, 255 / 3, 255 / 3};
uint8_t BLACK[3] = {0, 0, 0};
uint8_t PURPLE[3] = {238, 130/3, 238/2};


uint8_t *EtatHuitLeds[NUM_PIXELS]={
  BLACK, //0 Etat du programme, Initialisation(vert), test en cours(bleu), test fini bilan(blanc)
  BLACK, //1 Alarme de la voie testée
  BLACK, //2 Etat de la voie testée, None, OK ou HS
  BLACK, //3 Historique des voies, si une des voies etait en alarme en rouge sinon en vert
  BLACK, //4 Bilan des test
  BLACK, //5 
  BLACK, //6 COM ARAL RX
  BLACK  //7 COM ARAL  TX
};

void setHuitLedEtat(){
  WS2812B.clear();
  for (int p = 0; p < NUM_PIXELS; p++){
    uint8_t r = EtatHuitLeds[p][0], g = EtatHuitLeds[p][1], b = EtatHuitLeds[p][2];
    WS2812B.setPixelColor(p, WS2812B.Color(r, g, b));
  }
  WS2812B.show();
}

void LedEtatProgramme(int etat){
  switch (etat)
  {
  case 0:
    EtatHuitLeds[0] = GREEN;
    break;
  case 1:
    EtatHuitLeds[0] = BLUE;
  break;
  case 2:
    EtatHuitLeds[0] = WHITE;
  break;
  
  default:
    break;
  }
  setHuitLedEtat();
}

void LedAlarmeActuelle(Tension alarme){
  switch (alarme)
  {
  case COURT_CIRCUIT:
    EtatHuitLeds[1] = BLUE;
    break;
  case ALARME:
    EtatHuitLeds[1] = PURPLE;
    break;
  case NORMAL:
    EtatHuitLeds[1] = GREEN;
    break;
  case CONGRUENCE:
    EtatHuitLeds[1] = BLACK;
    break;
  default:
    break;
  }
  setHuitLedEtat();
}

void LedEtatVoieActuelle(uint8_t etat){
  if(etat == VOIE_OK){
    EtatHuitLeds[2] = GREEN;
  }
  else if(etat == VOIE_EN_DEFAUT){
    EtatHuitLeds[2] = RED;
    EtatHuitLeds[3] = RED;
  }
  else if (etat == VOIE_NONE){
    EtatHuitLeds[2] = BLACK;
  }
  setHuitLedEtat();
}

void LedACK(){
  static bool shift = false;
  EtatHuitLeds[6] = shift?BLACK:BLUE;
  EtatHuitLeds[7] = shift?BLUE:BLACK;
  shift = !shift;
  setHuitLedEtat();
}

void LedBilanTest(BilanTest bilan){
  for (int i = 0; i < 96; i++)
  {
    if(bilan.voies[i] == VOIE_EN_DEFAUT){
      EtatHuitLeds[0] = RED;
      setHuitLedEtat();
      return;
    }
  }
  EtatHuitLeds[0] = WHITE;
  setHuitLedEtat();
}

void LedsCommunicationARAL(bool shift){

}