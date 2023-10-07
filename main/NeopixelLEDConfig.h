// configuration/setup file for neopixel LED that will be used to show weight being measured
// defines the pins for each button in keypad

#ifndef NEOPIXEL_LED_CONFIG_H
#define NEOPIXEL_LED_CONFIG_H

// declaration of the Neopixel LED object
extern Adafruit_NeoPixel pixels;

int PIN = 15; // led panel pin
int NUMPIXELS = 12; // num of pixels

void neopixelLEDSetup(){
  pixels.begin();
  pixels.clear();
  pixels.show();
}

//function takes in 3 values, a = R, b= G, c= B. abc forms rgb code 
void led_colour(int a , int b, int c){
  pixels.clear();
  pixels.show();
  pixels.setBrightness(5);//changed to 5 as led was too bright 
  //update timing 
  for(int i=0; i<(NUMPIXELS); i++){
    pixels.setPixelColor(i, pixels.Color(a,b,c));
    pixels.show();
  }
}

//Function to check if weight is within range and displays led 
void ledChange(int weight, int offset){
  if (weight<(-1*offset)){
    led_colour(255 , 80, 0);//orange led 
  }
  if (weight> offset){
    led_colour(255 , 0, 0);//red led 
  }
  if ((weight> -1 * offset) && weight <offset){
    led_colour(0 , 255, 0);//green led 
  }
}
#endif