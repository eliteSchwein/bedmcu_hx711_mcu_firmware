#include <Arduino.h>
#include <HX711_ADC.h>

// constants for pins
const int dataOutPin = 8;
const int debugLed = 13;

// objects for loadcells
HX711_ADC LoadCell_1(9, 10);
HX711_ADC LoadCell_2(5, 6);
HX711_ADC LoadCell_3(12, 11);
HX711_ADC LoadCell_4(3, 2);

// constants for code
static float tareValues[] = {0,0,0,0};
static float values[] = {0,0,0,0};
const int tareCounterLimit = 2048;
static int tareCounter = 0;

bool calibrateTare();

void parseValues();

void updateOutput();

void setup() {
    // set mode for outputs
    pinMode(debugLed, OUTPUT);
    pinMode(dataOutPin, OUTPUT);
    digitalWrite(dataOutPin, LOW);

    // signal with debug led that the firmware is starting
    digitalWrite(debugLed, HIGH);

    // setup load cells
    LoadCell_1.begin();
    LoadCell_2.begin();
    LoadCell_3.begin();
    LoadCell_4.begin();

    // signal with debug led that the firmware is bootet
    digitalWrite(debugLed,LOW);
}

void loop() {
    static boolean newDataReady = 0;

    // check for new data/start next conversion:
    if (LoadCell_1.update()) newDataReady = true;
    LoadCell_2.update();
    LoadCell_3.update();
    LoadCell_4.update();

    //get smoothed value from data set
    if ((newDataReady)) {
        // parse values
        parseValues();

        // use tare calibration
        bool isCalibrating = calibrateTare();

        // show debug the calibration process
        if(isCalibrating) {
            return;
        }

        // disable debug LED
        digitalWrite(debugLed, LOW);

        // update output data pin
        updateOutput();

        newDataReady = 0;
    }
}

void updateOutput() {
    int limit = sizeof(values)/sizeof(values[0]);
    char activeMode = LOW;

    for (int i=0; i < limit; i++) {
        if(activeMode) {
            continue;
        }

        float tareValue = tareValues[i];
        float rawValue = values[i];

        if(rawValue > tareValue) {
            activeMode = HIGH;
        }
    }

    // write output to debug led and output pin
    digitalWrite(debugLed, activeMode);
    digitalWrite(dataOutPin, activeMode);
}

bool calibrateTare() {
    if(tareCounter == tareCounterLimit) {
        return false;
    }

    int limit = sizeof(tareValues)/sizeof(tareValues[0]);

    for (int i=0; i < limit; i++) {
        float tareValue = tareValues[i];
        float rawValue = values[i];

        if(rawValue > tareValue) {
            tareValues[i] = rawValue;
        }
    }

    tareCounter++;

    return true;
}

void parseValues() {
    values[0] = LoadCell_1.getData();
    values[1] = LoadCell_2.getData();
    values[2] = LoadCell_3.getData();
    values[3] = LoadCell_4.getData();
}