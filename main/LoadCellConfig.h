// configuration/setup file for load cell
// defines the load cell pins

#ifndef LOADCELL_CONFIG_H
#define LOADCELL_CONFIG_H

const int LOADCELL_DOUT_PIN = 32;
const int LOADCELL_SCK_PIN = 33; 

// calibration factor the load cell - this value is obtained by calibrating the scale with known weights
// see the README for details
const int CALIBRATION_FACTOR = 293.35;

#endif