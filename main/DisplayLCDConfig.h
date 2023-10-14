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
  //delay(50);
}

// Function for display ingredient only, auto new line once space is detected 
void displayIngredientOnLCD(String header){
  lcd.clear(); //clear screen 
  lcd.setCursor(0, 0);
  int space_pos = header.indexOf(" ");//find position of space in ingredient name. Potential bug here whereby if users add " " infront or " " behind.
  if (space_pos != -1){ //if -1 means space doesn't exist. 
    lcd.print(header.substring(0, space_pos));
    lcd.setCursor(0,1);
    lcd.print(header.substring(space_pos+1, header.length()));
  }
  else{
    lcd.print(header);
  }
  delay(1000);//screentime 
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
  delay(50);
}

// Function for displaying confirmation screen
void displayConfirmRecipeOnLCD(String text){
  lcd.setCursor(0, 0);
  if (text.length()> 16){
    lcd.scrollDisplayLeft();
  }
  lcd.print(text);
}

//Function during MQTT phase 
void displayPickReminder(){
  lcd.clear();//clear screen
  lcd.setCursor(0,0);
  lcd.print("Pick Item");
  lcd.setCursor(0,1);
  lcd.print("Follow Light");
  delay(3000);//time for picking
  lcd.clear();//clear screen for next prompt 
}

//Function that reminds users to tare at the start before weighing 
void displayTareReminder(){
  lcd.setCursor(0,0);
  lcd.print("Place Bowl");//possible implementation in future: bowl detection
  lcd.setCursor(0,1);
  lcd.print("Press TARE");
}

#endif