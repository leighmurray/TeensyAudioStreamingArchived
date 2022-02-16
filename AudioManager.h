#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H


// GUItool: begin automatically generated code
AudioInputUSB            usb1;           //xy=244.00000762939453,177.00000476837158
AudioRecordQueue         bufferUSBLeft;         //xy=409.0000686645508,152.00009536743164
AudioRecordQueue         bufferUSBRight;         //xy=410.0000762939453,204.00007438659668
AudioPlayQueue           i2sBufferRight;         //xy=414.0000114440918,408.00000953674316
AudioPlayQueue           i2sBufferLeft;         //xy=415,363
AudioOutputI2S           i2s1;           //xy=581.0001373291016,375.00005054473877
AudioConnection          patchCord1(usb1, 0, bufferUSBLeft, 0);
AudioConnection          patchCord2(usb1, 1, bufferUSBRight, 0);
AudioConnection          patchCord3(i2sBufferRight, 0, i2s1, 1);
AudioConnection          patchCord4(i2sBufferLeft, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=204.00000762939453,317.00000762939453
// GUItool: end automatically generated code

byte inputBufferLeft[256];
byte inputBufferRight[256];

void startInputBuffer(){
  Serial.println("Begin Buffer");
  bufferUSBLeft.begin();
  bufferUSBRight.begin();
}

bool handleInputBuffer(){
  //Serial.print("Available buffer:");
  //Serial.println(bufferUSBLeft.available());
  if (bufferUSBLeft.available() >= 1 && bufferUSBRight.available() >= 1) {
    // Fetch 1 blocks from the audio library
    memcpy(inputBufferLeft, bufferUSBLeft.readBuffer(), 256);
    bufferUSBLeft.freeBuffer();

    memcpy(inputBufferRight, bufferUSBRight.readBuffer(), 256);
    bufferUSBRight.freeBuffer();
    
    //for (int i = 0; i<sizeof(buffer); i++) Serial.print(buffer[i], HEX);
    //Serial.println("Handled Buffer");
    return true;
  } else {
    return false;
  }
}

void handleOutputBuffer(){
  memcpy(i2sBufferLeft.getBuffer(), inputBufferLeft, 256);
  memcpy(i2sBufferRight.getBuffer(), inputBufferRight, 256);
  i2sBufferLeft.playBuffer();
  i2sBufferRight.playBuffer();
}


void handleUSBVolume(){
  NetworkManagerLoop();
  // read the PC's volume setting
  float vol = usb1.volume();

  // scale to a nice range (not too loud)
  // and adjust the audio shield output volume
  if (vol > 0) {
    // scale 0 = 1.0 range to:
    //  0.3 = almost silent
    //  0.8 = really loud
    vol = 0.3 + vol * 0.5;
  }

  // use the scaled volume setting.  Delete this for fixed volume.
  sgtl5000_1.volume(vol);
}

void AudioManagerSetup(){
  AudioMemory(60);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  
  startInputBuffer();
}

void AudioManagerLoop(){
  handleUSBVolume();
  
  //Serial.println("Checking Buffer...");
  if (handleInputBuffer() == true){
    handleOutputBuffer();
  }
}
#endif
