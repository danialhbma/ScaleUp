// LCD Display file
// defines address of LCD panel and number of cols and rows in LCD display

#ifndef DISPLAY_LCD_CONFIG_H
#define DISPLAY_LCD_CONFIG_H

// declaration of the LCD object
extern LiquidCrystal_I2C lcd;

// lcd address
byte LCDADDRESS = 0x27;

// set the LCD number of columns and rows
int LCDCOLUMNS = 16;
int LCDROWS = 2;

void lcdSetup(){
  //initialize LCD(note that the pins for lcd is not accurate also supposed to be SDA 21 AND SCL 22 for esp32 wroom)
  //i2c specific pins for this cannot be changed 
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  //Intro message
  lcd.print("ScaleUp Project"); //Display a intro message
  delay(500); //Wait for display to show info
  lcd.clear(); //Then clean it
  //lcd.noBacklight(); // turn off backlight
}

// Function for default display (positioning of weight and stuff)
//will autoscroll if ingredient characters exceed 16 
void displayWeightOnLCD(String header, int weight){
  lcd.setCursor(0, 0);
  lcd.print(header);
  lcd.setCursor(0, 1);
  lcd.print("Offset:");
  //Weight is 8 characters 
  lcd.setCursor(8, 1);
  lcd.print(weight); 
  lcd.setCursor(15,1);
  lcd.print("g");
  delay(50);
}
//Function for displaying weight only, no ingredient 
void displayWeightOnLCDonly(int weight){
  lcd.setCursor(0, 1);
  lcd.print("Offset:");
  //Weight is 8 characters 
  lcd.setCursor(8, 1);
  lcd.print(weight); 
  lcd.setCursor(15,1);
  lcd.print("g");
  delay(100);
}

// Function for displaying confirmation screen
void displayConfirmRecipeOnLCD(String text){
  lcd.setCursor(0, 0);
  if (text.length()> 16){
    lcd.scrollDisplayLeft();
  }
  lcd.print(text);
  delay(100);
}


#endif