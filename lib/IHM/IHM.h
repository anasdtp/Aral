#ifndef _IHM_LIB

#define _IHM_LIB

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <Commun.h>

#define SDA_OLED_PIN 15
#define SCL_OLED_PIN 33

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO

#define PIN_WS2812B  12   // ESP32 pin that connects to WS2812B
#define NUM_PIXELS     8  // The number of LEDs (pixels) on WS2812B

#define NB_COLOR 4
extern uint8_t GREEN[3],BLUE[3],RED[3],WHITE[3],BLACK[3];

void IHM_begin();
void displayEtatVoie(int numVoie, String alarme, uint8_t etat);
void displayBilanTest(BilanTest voies);
void printMidOLED(String text, int textSize, int y);
void setLedColor(int numberOfLed, uint8_t couleur[3] = GREEN);
void testLed();

void LedEtatProgramme(int etat);
void LedAlarmeActuelle(Tension alarme);
void LedEtatVoieActuelle(uint8_t etat);
void LedACK();
void LedBilanTest(BilanTest bilan);


#endif // _IHM_LIB