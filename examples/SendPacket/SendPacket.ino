/**
 * Send a data packet using the MRF24J40 module.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

byte b = 1;
int i = -2015;
unsigned int ui = 2015;
long l = -2015000;
unsigned long ul = 2015000;
float f = 2.015;
char* str = "Circuitar";
byte buf[3] = { 0x01, 0x02, 0x03 };

void setup() {
  mrf.begin();
  mrf.setAddress(1);        // Network address
  mrf.setCoordinator(true); // Module is coordinator
}

void loop() {
  // Write packet data
  mrf.startPacket();
  mrf.write(b++);
  mrf.writeInt(i);
  mrf.writeUnsignedInt(ui);
  mrf.writeLong(l);
  mrf.writeUnsignedLong(ul);
  mrf.writeFloat(f);
  mrf.writeString(str);
  mrf.writeBytes(buf, 3);
  
  // Send packet to the module with address 2
  mrf.sendPacket(2);
  
  delay(1000);
}

