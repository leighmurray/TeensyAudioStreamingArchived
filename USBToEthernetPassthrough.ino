#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "NetworkManager.h"
#include "AudioManager.h"

void setup() {
  Serial.begin(115200);
  NetworkManagerSetup();
  AudioManagerSetup();
}

void loop() {
  AudioManagerLoop();
  NetworkManagerLoop();
  
  //Serial.println("Done Checking Buffer!");
  delay(2);
}
