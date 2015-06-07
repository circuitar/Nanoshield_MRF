/**
 * Send an integer using the MRF24J40 module.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

int i = 0;

void setup() {
  mrf.begin();
  mrf.setAddress(1); // Network address
}

void loop() {
  // Write packet containing an integer and send it to the module with address 2
  mrf.startPacket();
  mrf.writeInt(i);
  mrf.sendPacket(2);
  
  delay(1000);
  i++;
}

