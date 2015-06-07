/**
 * Send an integer using the MRF24J40 module, putting the MCU and radio module to sleep between each packet transmission.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <Nanoshield_MRF.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>

// Sleep time, in seconds
#define SLEEP_TIME 1

// Create wireless module object (MRF24J40MA/B/C/D/E)
Nanoshield_MRF mrf(MRF24J40MB);

// Number of times the watchdog timer expired
volatile int wdtCount = 0;

// Packet counter
int packets = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Reset");
  
  mrf.begin();
  mrf.setAddress(1); // Network address

  // Use "Power Down" sleep mode for ATmega328
  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // Initialize watchdog timer to expire in 1 second
  watchdogSetup(WDTO_1S);
}

void loop() {
  int seconds;
  
  // Get wdtCount value atomically - this is the number of times the watchdog timer expired, which
  //  is equal to the number of seconds that have passed since the processor was put to sleep.
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    seconds = wdtCount;
  }
  
  if (seconds >= SLEEP_TIME) {
    Serial.println(packets);
    // Wake radio module up
    mrf.wakeup();
    
    // Write packet containing an integer and send it to the module with address 2
    mrf.startPacket();
    mrf.writeInt(packets++);
    mrf.sendPacket(2);
    
    // Wait until last packet is sent before going to sleep
    delay(100);
    Serial.println(mrf.transmissionSuccess() ? "OK" : "Failed");
    mrf.sleep();

    // Reset the WDT interrupt counter atomically
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      wdtCount = 0;
    }
  }

  // Put the processor to sleep
  Serial.flush();
  //sleep();
}

// Initializes the watchdog timer in Interrupt Mode with timeouts according to avr/wdt.h (WDT0_xxx)
void watchdogSetup(int timeout) {
  // Enable watchdog using wdt_enable() in avr/wdt.h
  wdt_enable(timeout);
  
  // Change from System Reset Mode to Interrupt Mode
  volatile uint8_t wdtcsr = (WDTCSR | (1<<WDCE)) & ~(1<<WDE);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = wdtcsr;
  WDTCSR |= (1<<WDIE);
}

// Watchdog interrupt handling
ISR(WDT_vect) {
  // Increase watchdog expiration counter
  wdtCount++;
}

// Put ATmega to sleep
void sleep() {
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}
