/**
 * Scan wireless channels using the MRF24J40 module.
 *
 * This is useful to select the cleanest channel to use for communication.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>

// Number of samples per channel
#define SAMPLES 500

// Create wireless module object
Nanoshield_MRF mrf(MRF24J40MB);

void setup() {
  Serial.begin(9600);
  
  mrf.begin();
  mrf.setAddress(1);
}

void loop() {
  float channels[16];
  
  // Measure signal strength for each channel
  for (int i = 0; i < 16; i++) {
    mrf.setChannel(i + 11);
    channels[i] = 0;
    for (int j = 0; j < SAMPLES; j++) {
      channels[i] += mrf.measureSignalStrength();
    }
    channels[i] /= SAMPLES;
  }

  // Print information for all channels
  for (int i = 0; i < 16; i++) {
    Serial.print(i + 11);
    Serial.print(": ");
    Serial.print(channels[i], 1);
    Serial.print(" dBm ");
    for (int j = 0; j < 5 * (channels[i] + 90); j++) {
      Serial.print("=");
    }
    Serial.println();
  }
  Serial.println();
}

