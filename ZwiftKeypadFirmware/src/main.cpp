#include "main.h"
#include <Arduino.h>
#include <BleKeyboard.h>  // Include the BLE Keyboard library



BleKeyboard bleKeyboard("Bike Keypad", "AD", 100);  // Create a BLE Keyboard object with custom name and manufacturer


hw_timer_t *timer = nullptr;  // Declare a timer object
volatile bool timerFlag = false;  // A flag to indicate when the timer has triggered

volatile const int buttonPins[16] = {S1, S2, S3, S4, S5, S6, S7, S8,
                         S9, S10, S11, S12, S13, S14, S15, S16};

volatile const int keyCodes[16] = {S1KEYCODE, S2KEYCODE, S3KEYCODE, S4KEYCODE, S5KEYCODE,
                          S6KEYCODE, S7KEYCODE, S8KEYCODE, S9KEYCODE, S10KEYCODE,
                          S11KEYCODE, S12KEYCODE, S13KEYCODE, S14KEYCODE,
                          S15KEYCODE, S16KEYCODE};

volatile bool lastButtonState[16];      // Stores the last button state
volatile unsigned long pressTime[16];   // Time when the button was pressed
volatile unsigned long debounceDelay = 50;   // Debounce time in milliseconds
volatile unsigned long pressDuration = 0;
volatile const unsigned long longPressTime = 2000;  // Long press time threshold (1 second)

volatile uint8_t currentView = 1;

double readBatteryVoltage() {
    // Read the raw ADC value (0 to 4095) corresponding to the VBAT pin
    int raw = analogRead(VBATPIN);

    // Convert raw ADC value to voltage
    double voltage = (raw / 4095.0) * 2 * 3.3;  // Multiply by 2 because of the voltage divider, then scale by 3.3V reference

    return voltage;
}

// Function to estimate battery percentage based on voltage
int estimateBatteryPercentage(double voltage) {
    // These voltage levels are typical for LiPo batteries
    if (voltage >= 4.2) {
        setLEDStatus(Green);
        return 100;
    } else if (voltage > 4.1) {
        setLEDStatus(Green);
        return 90;
    } else if (voltage > 4.0) {
        setLEDStatus(Yellow);
        return 80;
    } else if (voltage > 3.9) {
        setLEDStatus(Yellow);
        return 70;
    } else if (voltage > 3.8) {
        setLEDStatus(Yellow);
        return 60;
    } else if (voltage > 3.7) {
        setLEDStatus(Yellow);
        return 50;
    } else if (voltage > 3.6) {
        setLEDStatus(Yellow);
        return 40;
    } else if (voltage > 3.5) {
        setLEDStatus(Yellow);
        return 30;
    } else if (voltage > 3.4) {
        setLEDStatus(Red);
        return 20;
    } else if (voltage > 3.3) {
        setLEDStatus(Red);
        return 10;
    } else {
        setLEDStatus(Red);
        return 0;  // Below 3.3V, the battery is critically low
    }
}

void IRAM_ATTR onTimer() {
    // This function is called when the timer triggers (runs every second)
    timerFlag = true;  // Set the flag when the timer triggers
}

// Set the colour of the status LED
void setLEDStatus(ledColour colour){
    switch (colour){
        case Red:
            digitalWrite(LEDRED, HIGH);
            digitalWrite(LEDGREEN, LOW);
            break;
        case Yellow:
            digitalWrite(LEDRED, HIGH);
            digitalWrite(LEDGREEN, HIGH);
            break;
        case Green:
            digitalWrite(LEDRED, LOW);
            digitalWrite(LEDGREEN, HIGH);
            break;
    }
}

void changeView(){
    if (bleKeyboard.isConnected()) {
        // Send the appropriate keycode
        bleKeyboard.write(48 + currentView);
    }

    currentView += 1;
    if (currentView > 9) {
        currentView = 0;
    }
}

void setup() {
    // Initialize Red LED as output
    pinMode(LEDRED, OUTPUT);
    digitalWrite(LEDRED, HIGH);

    // Initialize GREEN LED as output
    pinMode(LEDGREEN, OUTPUT);
    digitalWrite(LEDGREEN, LOW);

    // Initialize buttons
    for (int i = 0; i < 16; i++) {
        pinMode(buttonPins[i], INPUT);
        lastButtonState[i] = LOW;
    }

    // Configure timer: Timer 0, prescaler 80 (for 1 microsecond tick), count-up
    timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80 (for 1MHz), count-up

    // Attach the timer interrupt to the onTimer function
    timerAttachInterrupt(timer, &onTimer, true);

    // Set the timer to trigger every 10 second (10,000,000 microseconds)
    timerAlarmWrite(timer, 10000000, true);  // 10 second (10000000 us)

    // Enable the timer alarm
    timerAlarmEnable(timer);

    // Start the BLE keyboard
    bleKeyboard.begin();
}

void loop() {
    // Check if the timer has triggered
    if (timerFlag) {
        // Read battery voltage
        double batteryVoltage = readBatteryVoltage();

        // Estimate battery percentage
        int batteryPercentage = estimateBatteryPercentage(batteryVoltage);

        // Check if BLE keyboard is connected
        if (bleKeyboard.isConnected()) {
            bleKeyboard.setBatteryLevel(batteryPercentage);
        }
        timerFlag = false;
    }

    for (int i = 0; i < 16; i++) {
        bool reading = digitalRead(buttonPins[i]);  // Read the button state

        // Check if button state has changed
        if (reading != lastButtonState[i]) {
            if (reading == HIGH) {  // Button just pressed
                pressTime[i] = millis();  // Record the press start time
            } else if (reading == LOW) {  // Button just released
                pressDuration = millis() - pressTime[i];  // Calculate press duration

                if (pressDuration < longPressTime) { // short press
                    if (i == 1){
                        changeView();
                    }
                    else {
                        if (bleKeyboard.isConnected()) {
                            // Send the appropriate keycode
                            bleKeyboard.write(keyCodes[i]);
                        }
                    }
                } else { // long press
                    if (i == 1){ // View button long pressed
                        currentView = 1; // reset to default view
                        changeView();
                    }
                    else if (i == 6){ // Camera button long pressed
                        if (bleKeyboard.isConnected()) {
                            // Send F9 combo for screen recording
                            bleKeyboard.write(KEY_F9);
                        }
                    }
                    else {
                        if (bleKeyboard.isConnected()) {
                            // Send the appropriate keycode
                            bleKeyboard.write(keyCodes[i]);
                        }
                    }
                }
            }
            // Update last button state
            lastButtonState[i] = reading;
        }

    }

    // Simple debounce delay (to prevent multiple readings from button bounce)
    delay(debounceDelay);
}
