/**
 * Receive a data packet using the MRF24J40 module.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf/*(MRF24J40MA)*/; // Make sure to select the right module

byte b;
int i;
unsigned int ui;
long l;
unsigned long ul;
float f;
char str[MRF_MAX_PAYLOAD_SIZE];
byte buf[MRF_MAX_PAYLOAD_SIZE];

void setup() {
  Serial.begin(9600);

  // Initialize module
  mrf.begin();
  mrf.setAddress(2);        // Network address
  mrf.setCoordinator(true); // Module is coordinator
}

void loop() {
  // Check if a packet has been received
  if (mrf.receivePacket()) {
    // Get packet data
    b = mrf.read();
    i = mrf.readInt();
    ui = mrf.readUnsignedInt();
    l = mrf.readLong();
    ul = mrf.readUnsignedLong();
    f = mrf.readFloat();
    mrf.readString(str, MRF_MAX_PAYLOAD_SIZE);
    mrf.readBytes(buf, 3);

    // Print packet data
    printPacket();
  }
}

void printPacket() {
  Serial.println("Packet received: ");
  Serial.print(" Byte: ");
  Serial.print(b);
  Serial.println(" (packet counter)");
  Serial.print(" Integer: ");
  Serial.println(i);
  Serial.print(" Unsigned integer: ");
  Serial.println(ui);
  Serial.print(" Long integer: ");
  Serial.println(l);
  Serial.print(" Unsigned long integer: ");
  Serial.println(ul);
  Serial.print(" Floating point number: ");
  Serial.println(f, 3);
  Serial.print(" Character string: \"");
  Serial.print(str);
  Serial.println("\"");
  char output[100];
  sprintf(output, " Array of bytes: %02X %02X %02X", buf[0], buf[1], buf[2]);
  Serial.println(output);
  Serial.println();
}
