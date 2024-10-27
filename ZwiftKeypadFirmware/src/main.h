//
// Created by Andrew Dorey on 08/10/2024.
//

#ifndef ZWIFTKEYPADFIRMWARE_MAIN_H
#define ZWIFTKEYPADFIRMWARE_MAIN_H

#include <BleKeyboard.h>  // Include the BLE Keyboard library
// External Hardware Definitions

#define VBATPIN A13  // VBAT pin (on the Adafruit Feather ESP32 V2, VBAT is internally connected to A13)

#define S1 12
#define S2 32
#define S3 36
#define S4 4
#define S5 27
#define S6 14
#define S7 5
#define S8 19
#define S9 33
#define S10 20
#define S11 7
#define S12 21
#define S13 15
#define S14 22
#define S15 37
#define S16 8

#define S1KEYCODE 'a'
#define S2KEYCODE '0'
#define S3KEYCODE 'g'
#define S4KEYCODE 't'

#define S5KEYCODE KEY_F5 // F5
#define S6KEYCODE KEY_F8 // F8
#define S7KEYCODE KEY_F10 // F10
#define S8KEYCODE 32 // Space

#define S9KEYCODE KEY_PAGE_UP // Page Up
#define S10KEYCODE KEY_RETURN  // Return
#define S11KEYCODE KEY_UP_ARROW // Up arrow
#define S12KEYCODE KEY_ESC // Esc

#define S13KEYCODE KEY_PAGE_DOWN // Page Down
#define S14KEYCODE KEY_LEFT_ARROW // Left arrow
#define S15KEYCODE KEY_DOWN_ARROW // Down arrow
#define S16KEYCODE KEY_RIGHT_ARROW // Right arrow

#define LEDRED 25
#define LEDGREEN 26

#define VBATPIN A13  // VBAT pin (on the Adafruit Feather ESP32 V2, VBAT is internally connected to A13)

enum ledColour { Red, Yellow, Green };

void setLEDStatus(ledColour colour);
double readBatteryVoltage();
int estimateBatteryPercentage(double voltage);

#endif //ZWIFTKEYPADFIRMWARE_MAIN_H

