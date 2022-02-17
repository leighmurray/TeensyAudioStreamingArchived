#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "NetworkManager.h"
#include "AudioManager.h"

NetworkManager networkManager;
AudioManager audioManager;

void setup() {
  Serial.begin(115200);
  networkManager.Setup();
  //audioManager.Setup();
}

void loop() {
  //audioManager.Loop();
  networkManager.Loop();
  
  //Serial.println("Done Checking Buffer!");
  delay(1000);
}
