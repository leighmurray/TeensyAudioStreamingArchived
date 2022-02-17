#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "NetworkManager.h"
#include "AudioManager.h"

NetworkManager networkManager;
AudioManager audioManager;

void handleRemoteAudio();
void handleLocalAudio();

void setup() {
  Serial.begin(115200);
  networkManager.Setup();
  audioManager.Setup();
}

void loop() {
  handleLocalAudio();
  handleRemoteAudio();
}

void handleLocalAudio(){
  byte inputAudioBufferLeft[256];
  byte inputAudioBufferRight[256];
  bool hasLocalAudioBuffers = audioManager.getInputAudioBuffers(inputAudioBufferLeft, inputAudioBufferRight);
  
  // if there is an audio buffer send it to the other device
  if (hasLocalAudioBuffers){
    networkManager.sendAudioBuffers(inputAudioBufferLeft, inputAudioBufferRight);
  }
}

void handleRemoteAudio(){
  byte outputAudioBufferLeft[256];
  byte outputAudioBufferRight[256];
  if (networkManager.receiveAudioBuffers(outputAudioBufferLeft, outputAudioBufferRight)){
    audioManager.setOutputAudioBuffers(outputAudioBufferLeft, outputAudioBufferRight);
  }
}
