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
int magSensorPin = 12;
int txPin = 7;
int val = 0;     // variable for reading the sensor status
String doorState = "Open";

void setup()
{
    Serial.begin(9600);	  // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2400);	 // Bits per sec
    vw_set_tx_pin(txPin);                // pin 3 is used as the transmit data out into the TX Link module, change this to suit your needs. 
    
    pinMode(magSensorPin, INPUT);
    digitalWrite(magSensorPin, HIGH);
}

void loop()
{    
    val = digitalRead(magSensorPin);  // read input value
    Serial.println(val);
    if (val==LOW) {
      doorState = "Closed";
    }
    else{
      doorState = "Open";      
    }
    char msg[sizeof(doorState) + 1];
    doorState.toCharArray(msg, sizeof(doorState) + 1);

    digitalWrite(13, true); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(13, false);
    delay(200);
}
