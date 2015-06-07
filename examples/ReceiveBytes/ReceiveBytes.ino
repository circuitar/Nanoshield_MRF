/**
 * Receive raw data using the MRF24J40 module.
 *
 * Copyright (c) 2015 Circuitar
 * All rights reserved.
 *
 * This software is released under a BSD license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

void setup() {
  Serial.begin(9600);

  // Initialize module
  mrf.begin();
  mrf.setAddress(2);        // Network address
  mrf.setCoordinator(true); // Module is coordinator
}

void loop() {
  if (mrf.receivePacket()) {
    // Print data length
    Serial.print("Packet received: ");
    Serial.print(mrf.bytesLeftToRead());
    Serial.println(" bytes");
    
    // Print data in hexadecimal format
    while (mrf.bytesLeftToRead()) {
      char buf[4];
      sprintf(buf, "%02X ", mrf.read());
      Serial.print(buf);
    }
    Serial.println();
    Serial.print("Received Signal Power: ");
    Serial.print(mrf.getSignalStrength(), 1);
    Serial.print(" dBm\r\n Link Quality Indicator (LQI): ");
    Serial.println(mrf.getLinkQuality());
    Serial.println();
  }
}

