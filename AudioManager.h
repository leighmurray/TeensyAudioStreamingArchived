#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputUSB            inputDeviceUSB; //xy=85.00003051757812,75.00000286102295
AudioInputI2S            inputDeviceI2S;           //xy=88,247.00000667572021
AudioRecordQueue         inputBufferI2SLeft;         //xy=284.00000762939453,214.0000057220459
AudioRecordQueue         inputBufferI2SRight;         //xy=285,275
AudioRecordQueue         inputBufferUSBRight; //xy=288.0000457763672,112.00000381469727
AudioRecordQueue         inputBufferUSBLeft; //xy=291.0000457763672,47.000006675720215
AudioPlayQueue           outputBufferUSBLeft;         //xy=478.00001525878906,47.000000953674316
AudioPlayQueue           outputBufferUSBRight;         //xy=481.00001525878906,112.00000286102295
AudioPlayQueue           outputBufferI2SLeft; //xy=485.00000762939453,212.00000667572021
AudioPlayQueue           outputBufferI2SRight; //xy=486.0000534057617,275.00000762939453
AudioOutputI2S           outputDeviceI2S; //xy=680.0000152587891,244.0000114440918
AudioOutputUSB           outputDeviceUSB;           //xy=685.0000152587891,75.00000190734863
AudioConnection          patchCord1(inputDeviceUSB, 0, inputBufferUSBLeft, 0);
AudioConnection          patchCord2(inputDeviceUSB, 1, inputBufferUSBRight, 0);
AudioConnection          patchCord3(inputDeviceI2S, 0, inputBufferI2SLeft, 0);
AudioConnection          patchCord4(inputDeviceI2S, 1, inputBufferI2SRight, 0);
AudioConnection          patchCord5(outputBufferUSBLeft, 0, outputDeviceUSB, 0);
AudioConnection          patchCord6(outputBufferUSBRight, 0, outputDeviceUSB, 1);
AudioConnection          patchCord7(outputBufferI2SLeft, 0, outputDeviceI2S, 0);
AudioConnection          patchCord8(outputBufferI2SRight, 0, outputDeviceI2S, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=267.00000762939453,369.0000057220459
// GUItool: end automatically generated code

class AudioManager{

public:
  void Setup(){
    AudioMemory(60);
    sgtl5000_1.enable();

    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    
    // max without clipping from headphone output is 0.7
    sgtl5000_1.volume(0.7);
    
//    activeInputBufferLeft = &inputBufferUSBLeft;
//    activeInputBufferRight = &inputBufferUSBRight;
    activeInputBufferLeft = &inputBufferI2SLeft;
    activeInputBufferRight = &inputBufferI2SRight;
    startInputBuffer();
  }
  
  bool getInputAudioBuffers(byte localAudioBufferLeft[256], byte localAudioBufferRight[256]){
    if (activeInputBufferLeft->available() >= 1 && activeInputBufferRight->available() >= 1) {
      // Fetch 1 blocks from the audio library
      memcpy(localAudioBufferLeft, activeInputBufferLeft->readBuffer(), 256);
      activeInputBufferLeft->freeBuffer();
      
      memcpy(localAudioBufferRight, activeInputBufferRight->readBuffer(), 256);
      activeInputBufferRight->freeBuffer();

      return true;
    } else {
      return false;
    }
  }

  bool setOutputAudioBuffers(byte outputAudioBufferLeft[256], byte outputAudioBufferRight[256]){
    memcpy(outputBufferI2SLeft.getBuffer(), outputAudioBufferLeft, 256);
    memcpy(outputBufferI2SRight.getBuffer(), outputAudioBufferRight, 256);
    outputBufferI2SLeft.playBuffer();
    outputBufferI2SRight.playBuffer();
    return true;
  }

private:
  AudioRecordQueue* activeInputBufferLeft;
  AudioRecordQueue* activeInputBufferRight;
  AudioPlayQueue* activeOutputBufferLeft;
  AudioPlayQueue* activeOutputBufferRight; 

  void startInputBuffer(){
    Serial.println("Begin Buffer");
    activeInputBufferLeft->begin();
    activeInputBufferRight->begin();
  }
  
//  void handleUSBVolume(){
//    // read the PC's volume setting
//    float vol = inputDeviceUSB.volume();
//  
//    // scale to a nice range (not too loud)
//    // and adjust the audio shield output volume
//    if (vol > 0) {
//      // scale 0 = 1.0 range to:
//      //  0.3 = almost silent
//      //  0.8 = really loud
//      vol = 0.3 + vol * 0.5;
//    }
//  
//    // use the scaled volume setting.  Delete this for fixed volume.
//    sgtl5000_1.volume(vol);
//  }

};

#endif
