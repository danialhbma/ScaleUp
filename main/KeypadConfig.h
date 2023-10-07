// keypad configuration/setup file
// defines the pins for each button in keypad

#ifndef KEYPAD_CONFIG_H
#define KEYPAD_CONFIG_H

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char KEYS [ROWS] [COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte ROWPINS[ROWS] = {19,18,5,17}; 
byte COLPINS[COLS] = {16,4,0,2};

#endif