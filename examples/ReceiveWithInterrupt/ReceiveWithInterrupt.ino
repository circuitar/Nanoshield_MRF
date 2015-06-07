/**
 * Receive raw data via interrupt using the MRF24J40 module.
 *
 * The INT pin from MRF24J40 must be connected to the INT1 pin in the ATmega328 (Arduino D3 pin).
 *
 * Copyright (c) 2015 Circuitar
 * All rights reserved.
 *
 * This software is released under a BSD license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>
#include <util/atomic.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

// Indicates a packet has been received
volatile bool received = false;

void setup() {
  Serial.begin(9600);

  // Initialize module
  mrf.begin();
  mrf.setAddress(2);        // Network address
  mrf.setCoordinator(true); // Module is coordinator
  
  // Check reception for the first time
  receive();

  // Get packet from module when there is an interrupt on pin INT1 (D3 on Arduino)
  attachInterrupt(1, receive, FALLING);
}

void loop() {
  // Check if packet was received (variables shared with ISR must be read atomically)
  bool ready;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    ready = received;
  }

  if (ready) {
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

    // Allow interrupt to continue receiving packets (write atomically)
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      received = false;
    }
  }
}

void receive() {
  received = !received && mrf.receivePacket();
}