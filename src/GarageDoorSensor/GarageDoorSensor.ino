// transmitter.pde
//
// Simple example of how to use VirtualWire to transmit messages
// Implements a simplex (one-way) transmitter with an TX-C1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@open.com.au)
// Copyright (C) 2008 Mike McCauley
// $Id: transmitter.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include <VirtualWire.h>
int magSensorPin1 = 11;
int magSensorPin2 = 12;
int txPin = 7;
int val1 = 0;     // variable for reading the sensor status
int val2 = 0;     // variable for reading the sensor status
String doorState = "00";

void setup()
{
    Serial.begin(9600);	  // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2400);	 // Bits per sec
    vw_set_tx_pin(txPin);                // pin 3 is used as the transmit data out into the TX Link module, change this to suit your needs. 
    
    pinMode(magSensorPin1, INPUT);
    digitalWrite(magSensorPin1, HIGH);
    pinMode(magSensorPin2, INPUT);
    digitalWrite(magSensorPin2, HIGH);
}

void loop()
{    
    val1 = digitalRead(magSensorPin1);  // read input value
    val2 = digitalRead(magSensorPin2);  // read input value
    if (val1==LOW && val2==LOW) {
      doorState = "11";
    }
    else if (val1==LOW && val2==HIGH) {
      doorState = "10";
    }
    else if (val1==HIGH && val2==LOW) {
      doorState = "01";
    }
    else{
      doorState = "00";
    }
    Serial.println(doorState);
    char msg[sizeof(doorState) + 1];
    doorState.toCharArray(msg, sizeof(doorState) + 1);

    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
    delay(200);
}
