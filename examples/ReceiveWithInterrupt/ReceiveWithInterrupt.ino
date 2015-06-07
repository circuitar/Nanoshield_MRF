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

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

// Buffer to hold received packet
byte buf[MRF_MAX_PAYLOAD_SIZE];

// Number of bytes received
volatile int byteCount = 0;

void setup() {
  Serial.begin(9600);

  // Initialize module
  mrf.begin();
  mrf.setAddress(2); // Network address
  
  // Get packet from module when there is an interrupt on pin INT1 (D3 on Arduino)
  attachInterrupt(1, receive, FALLING);
  
  // Check reception for the first time
  receive();
}

void loop() {
  int bc = atomicGet(&byteCount);

  if (bc > 0) {
    // Print data length
    Serial.print("Packet received: ");
    Serial.print(bc);
    Serial.println(" bytes");
    
    // Print data in hexadecimal format
    for (int i = 0; i < bc; i++) {
      char hex[4];
      sprintf(hex, "%02X ", buf[i]);
      Serial.print(hex);
    }
    Serial.println();
    Serial.println();
    
    atomicSet(&byteCount, 0);
  }
}

int atomicGet(volatile int* ptr) {
  noInterrupts();
  int i = *ptr;
  interrupts();
  return i;
}

void atomicSet(volatile int* ptr, int val) {
  noInterrupts();
  *ptr = val;
  interrupts();
}

void receive() {
  if (byteCount == 0 && mrf.receivePacket()) {
    byteCount = mrf.readBytes(buf, MRF_MAX_PAYLOAD_SIZE);
  }
}