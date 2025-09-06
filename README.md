# ESP32 C6 Supermini GC9A01 Colored Wheel Clock
The Colored Wheel Clock sketch is a colorfull clock realized with an **ESP32-C6 microcontroller** and a **GC9A01 round display**.

This is the accompanying repository for my article "**Create a Color Wheel Clock on a round GC9A01 TFT display with an ESP32-C6 Supermini microcontroller**" available here: 

![Image 1](./images/esp32_epaper_meteomatics_04_600w.png)

## Required Library
````plaintext
TFT_eSPI by "Bodmer" version Mar 6, 2024 (https://github.com/Bodmer/TFT_eSPI)
````

## Runtime Environment
````plaintext
Arduino 2.3.6 (Windows)
esp32 boards version 3.2.0
For compiling and uploading, I'm using the "ESP32-C6 Dev Module" board
````

## Display
My display is a 1.28-inches large TFT display with 240 x 240 pixels resolution.

### Settings for the display specific setup file
I'm using a display specific setup file for the combination ESP32-C6 Supermini with TFT display driver GC9A01.This file contains e.g. the display driver, size and pins for the ESP32-device. If your display has a different size please change the height and width accordingly.

### Copy a file to the User_Setups folder
Please copy the file:
````plaintext
Setup708_C6_SM_GC9A01_240x240_Max_ADC.h
to the User_Setups folder.
````

### Edit the User_Setup_Select.h file
You need to place the following line in the root folder's "User_Setup_Select.h" file
````plaintext
#include <User_Setups/Setup708_C6_SM_GC9A01_240x240_Max_ADC.h> // ESP32-C6 Supermini, 80 MHz
Second: please comment all other "#include..." entries like this, especially the "//#include <User_Setup.h>" entry.

// Example User_Setup files are stored in the "User_Setups" folder. These can be used
// unmodified or adapted for a particular hardware configuration.
#ifndef USER_SETUP_LOADED //  Lets PlatformIO users define settings in
                          //  platformio.ini, see notes in "Tools" folder.
///////////////////////////////////////////////////////
//   User configuration selection lines are below    //
///////////////////////////////////////////////////////
// Only ONE line below should be uncommented to define your setup.  Add extra lines and files as needed.
//#include <User_Setup.h>                       // Default setup is root library folder
// Setup file in folder Arduino/libraries (updates will not overwrite your setups)
#include <User_Setups/Setup708_C6_SM_GC9A01_240x240_Max_ADC.h> // ESP32-C6 Super Mini, 80 MHz
````

## Wiring
````plaintext
Pin order is from LEFT to RIGHT, seen from displays side
Nr name  ESP32-C6 pin
1  GND   GND 
2  VCC   VCC is 3.3 volt, not 5 volt
3  SCL   17 // GPIO 17 is labled as 'RX'
4  SDA   19 // 'SDA' terminal is known as 'MOSI' 
5  RST   20 
6  D/C   14
7  CS    18
8  BL     0 // 'BL' or 'BACKL' is the backlight pin connected to a PWM pin
````
