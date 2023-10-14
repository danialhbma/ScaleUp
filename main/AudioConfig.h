//file here helps to control diff audio files
//Main hardware mp3 module from Continental which plays audio by shorting pins
#ifndef AUDIO_H
#define AUDIO_H

#include "Arduino.h"

//Assigning output pins to audio files. A1 on mp3 module is linked to first file in sd card
void audioSetup(){
  //intialising pins 
  pinMode (25,OUTPUT);//audio file - 'More'
  pinMode (26,OUTPUT);//audio file - 'Less'
  pinMode (27,OUTPUT);//audio file - 'Nice'
  pinMode (14,OUTPUT);//audio file - 'Place bowl and Tare'
  //resolve bug that some pins will be set to gnd accidentally and play audio during setup 
  digitalWrite(25,HIGH);
  digitalWrite(26,HIGH);
  digitalWrite(27,HIGH);
  digitalWrite(14,HIGH); 

}

//function shorts pins to play audio file
//pin>data pin linked to audio file, repeat>no of times to repeat audio
//play once  
void playaudio_once(int pin){
  digitalWrite(pin,LOW);//short pins
  delay(100);//some delay required for audio to play first before switching back to pin high mode
  digitalWrite(pin,HIGH);
}

//changes audio according to weight 
void audioChange(int weight,int offset){
  if (weight<(-1*offset)){
    playaudio_once(25);//more  
  }
  if (weight> offset){
    playaudio_once(26);//less
  }
  if ((weight> -1 * offset) && weight <offset){
    playaudio_once(27);//nice
  }
}

#endif