/**
 * Receive a character string using the MRF24J40 module.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf/*(MRF24J40MA)*/; // Make sure to select the right module

void setup() {
  Serial.begin(9600);

  // Initialize module
  mrf.begin();
  mrf.setAddress(2);        // Network address
  mrf.setCoordinator(true); // Module is coordinator
}

void loop() {
  // If a packet has been received, print the string in it
  if (mrf.receivePacket()) {
    Serial.println(mrf.readString());
    
    // If you want to assign it to a variable, you must use "const":
    //   const char* str = mrf.readString();
    //   Serial.println(str);
    // Note: this is simple and efficient, but you won't be able to overwrite the data in the string
    //       returned by readString(), and when you call receivePacket() again it might be overwritten.
    //       If that doesn't work for you, use example ReadStringIntoBuffer.
  }
}

