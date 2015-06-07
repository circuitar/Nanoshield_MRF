/**
 * Wireless link test for the MRF24J40 module.
 *
 * To run this test, just upload it to one module with MASTER set to true and to another module with MASTER set to false.
 * Make sure to select the MODULE_TYPE according to the Microchip module you are using.
 * The other constants can be tweaked, but this is not necessary to run the tests.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Select if module is master or slave
#define MASTER true

// Select module type (MRF24J40MA/B/C/D/E)
#define MODULE_TYPE MRF24J40MB

// Enable/disable acknowledgement and automatic retransmission
#define ACK false

// Select radio channel (11 to 26)
#define CHANNEL 11

// Select number of packets sent per burst
#define BURST_LENGTH 100

// Timeout after which master sends another burst if nothing was received, in milliseconds
#define TIMEOUT 2000

// Number of terminator packets to send
#define TERMINATORS 10

// Create wireless module object
Nanoshield_MRF mrf(MODULE_TYPE);

// Network addresses
int localAddr = MASTER ? 1 : 2;
int destAddr = MASTER ? 2 : 1;

void setup() {
  Serial.begin(9600);
  
  mrf.begin();
  mrf.setAddress(localAddr);
  mrf.setCoordinator(MASTER);
  mrf.setChannel(CHANNEL);
  
  // If this is the master, wait for slave to reset and start sending packets
  if (MASTER) {
    sendPackets(0, 0, 0);
  }
}

void loop() {
  int count = 0;
  unsigned long t0 = millis();
  float rssiSum = 0;
  unsigned long lqiSum = 0;
  int localLinkRate = 0;
  float localRssi = 0;
  int localLqi = 0;
  int remoteLinkRate = 0;
  float remoteRssi = 0;
  int remoteLqi = 0;
  
  // Count the number of packets received until the terminator packet or timeout
  while (MASTER ? (millis() - t0 < TIMEOUT) : true) {
    if (mrf.receivePacket()) {
      if (mrf.read() == 0) {
        mrf.readInt();                  // Packet number (ignore)
        remoteLinkRate = mrf.readInt(); // Link rate
        remoteRssi = mrf.readFloat();   // RSSI
        remoteLqi = mrf.readInt();      // LQI
        rssiSum += mrf.getSignalStrength();
        lqiSum += mrf.getLinkQuality();
        count++;
      } else {
        delay(5 * TERMINATORS); // Wait until all terminators are received
        mrf.receivePacket();    // Clear reception buffer
        break;
      }
    }
  }
  
  // Compute local link information
  localLinkRate = 100L * count / BURST_LENGTH;
  localRssi = rssiSum / BURST_LENGTH;
  localLqi = lqiSum / BURST_LENGTH;
  
  // Print link information
  printLinkInfo("Local", localLinkRate, localRssi, localLqi);
  printLinkInfo("Remote", remoteLinkRate, remoteRssi, remoteLqi);
  Serial.println();
  
  // Send next burst
  sendPackets(localLinkRate, localRssi, localLqi);

  // Flush reception buffer
  while (mrf.receivePacket());
}

void sendPackets(int rate, float rssi, int lqi) {
  // Send packet burst
  for (int i = 0; i < BURST_LENGTH; i++) {
    mrf.startPacket();
    mrf.write(0);
    mrf.writeInt(i);      // Packet number
    mrf.writeInt(rate);   // Link rate
    mrf.writeFloat(rssi); // RSSI
    mrf.writeInt(lqi);    // LQI
    mrf.sendPacket(destAddr, ACK);
    while (!mrf.transmissionDone());
  }
  
  // Send terminator packets
  for (int i = 0; i < TERMINATORS; i++) {
    mrf.startPacket();
    mrf.write(1);
    mrf.sendPacket(destAddr);
    while (!mrf.transmissionDone());
  }
}

void printLinkInfo(char* name, int lr, float rssi, int lqi) {
  Serial.print(name);
  Serial.print(":\r\n Link rate: ");
  Serial.print(lr);
  Serial.print("%\r\n Received Power: ");
  Serial.print(rssi, 1);
  Serial.print(" dBm\r\n LQI: ");
  Serial.println(lqi);
}
