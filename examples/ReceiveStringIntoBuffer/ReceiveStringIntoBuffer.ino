/**
 * Receive a character string into a buffer using the MRF24J40 module.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

void setup() {
  Serial.begin(9600);

  // Initialize module
  mrf.begin();
  mrf.setAddress(2); // Network address
}

void loop() {
  char str[10]; // Buffer with received string length plus one (terminator character)
  
  // If a packet has been received, print the string in it
  if (mrf.receivePacket()) {
    mrf.readString(str, 10); // Read the string into the buffer, up to its length
    Serial.println(str);     // Print the string
  }
}

