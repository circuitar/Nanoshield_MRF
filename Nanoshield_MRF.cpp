/**
 * This is our new Arduino library to use the 2.4GHz MRF24J40 wireless radio from Microchip.
 * It supports communication using the 802.15.4 wireless protocol at the MAC level, and enables
 * use of the MRF24J40MA/B/C/D/E wireless modules from Microchip.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
*/
#include <Nanoshield_MRF.h>

#define MRF_RXMCR    0x00
#define MRF_PANIDL   0x01
#define MRF_PANIDH   0x02
#define MRF_SADRL    0x03
#define MRF_SADRH    0x04
#define MRF_RXFLUSH  0x0D
#define MRF_TXNCON   0x1B
#define MRF_PACON2   0x18
#define MRF_WAKECON  0x22
#define MRF_TXSTAT   0x24
#define MRF_SOFTRST  0x2A
#define MRF_TXSTBL   0x2E
#define MRF_INTSTAT  0x31
#define MRF_INTCON   0x32
#define MRF_GPIO     0x33
#define MRF_TRISGPIO 0x34
#define MRF_SLPACK   0x35
#define MRF_RFCTL    0x36
#define MRF_BBREG1   0x39
#define MRF_BBREG2   0x3A
#define MRF_BBREG6   0x3E
#define MRF_CCAEDTH  0x3F
#define MRF_RFCON0   0x200
#define MRF_RFCON1   0x201
#define MRF_RFCON2   0x202
#define MRF_RFCON6   0x206
#define MRF_RFCON7   0x207
#define MRF_RFCON8   0x208
#define MRF_RSSI     0x210
#define MRF_SLPCON0  0x211
#define MRF_SLPCON1  0x220
#define MRF_TESTMODE 0x22F

SPISettings Nanoshield_MRF::spiSettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);

Nanoshield_MRF::Nanoshield_MRF(Mrf24j40Type type, int cs) {
  if (type == MRF24J40MA) {
    this->paLna = false;
  } else {
    this->paLna = true;
  }

  this->cs = cs;
  this->panId = 0;
  this->srcAddr = 1;
  this->seqNumber = 0;
  this->txCount = 0;
  this->rxSize = 0;
  this->rxCount = 0;
  this->rssi = 0;
  this->lqi = 0;
}

void Nanoshield_MRF::begin() {
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  SPI.begin();

  // MRF24J40 module configuration
  writeShort(MRF_PACON2, 0b10011000);    // Setup recommended PA/LNA control timing (TXONTS = 0x6)
  writeShort(MRF_TXSTBL, 0b10010101);    // Setup recommended PA/LNA control timing (RFSTBL = 0x9)
  writeLong(MRF_RFCON0, 0b00000011);     // Set recommended value for RF Optimize Control (RFOPT = 0x3)
  writeLong(MRF_RFCON1, 0b00000010);     // Set recommended value for VCO Optimize Control (VCOOPT = 0x2)
  writeLong(MRF_RFCON2, 0b10000000);     // Enable PLL (PLLEN = 1)
  writeLong(MRF_RFCON6, 0b10010000);     // Set recommended value for TX Filter Control and 20MHz Clock Recovery Control
                                         //  (TXFIL = 1, 20MRECVR = 1)
  writeLong(MRF_RFCON7, 0b10000000);     // Use 100kHz internal oscillator for Sleep Clock (SLPCLKSEL = 0x2)
  writeLong(MRF_RFCON8, 0b00010000);     // Set recommended value for VCO Control (RFVCO = 1)
  writeLong(MRF_SLPCON0, 0b00000001);    // Disable the sleep clock to save power (/SLPCLKEN = 1)
  writeLong(MRF_SLPCON1, 0b00100001);    // Disable CLKOUT pin and set Sleep Clock Divisor to minimum of 0x01 for the
                                         //  100kHz internal oscillator (/CLOUKTEN = 1, SLPCLKDIV = 0x01)
  writeShort(MRF_TRISGPIO, 0b00001000);  // Make GPIO3 an output to control the PA voltage regulator (TRISGP3 = 1)
  writeShort(MRF_GPIO, 0b00001000);      // Set GPIO3 high to turn on the PA voltage regulator (GPIO3 = 1)
  writeShort(MRF_BBREG2, 0b10111000);    // Use CCA Mode 1 - Energy above threshold - and set CCA Carrier Sense
                                         //  Threshold to recommended value (CCAMODE = 0x2, CCACSTH = 0xE)
  writeShort(MRF_CCAEDTH, 0b01100000);   // Set energy detection threshold to recommended value (CCAEDTH = 0x60)
  writeShort(MRF_BBREG6, 0b01000000);    // Calculate RSSI for each packet received (RSSIMODE2 = 1)
  writeShort(MRF_INTCON, 0b11110111);    // Enable RX FIFO reception interrupt (RXIE = 0)
  setPanId(panId);
  setCoordinator(false);
  setPaLna(paLna);
  setChannel(11);                        // Set default channel (must keep 11) and reset state machine
}

void Nanoshield_MRF::setPanId(uint16_t panId) {
  this->panId = panId;
  writeShort(MRF_PANIDH, panId >> 8);
  writeShort(MRF_PANIDL, panId);
}

void Nanoshield_MRF::setAddress(uint16_t addr) {
  this->srcAddr = addr;
  writeShort(MRF_SADRH, addr >> 8);
  writeShort(MRF_SADRL, addr);
}

void Nanoshield_MRF::setPaLna(bool paLna) {
  this->paLna = paLna;
  writeLong(MRF_TESTMODE, paLna ? 0b00001111 : 0b00001000);
}

void Nanoshield_MRF::setCoordinator(bool coord) {
  uint8_t reg = readShort(MRF_RXMCR);
  writeShort(MRF_RXMCR, coord ? reg | 0b00001100 : reg & ~0b00001100);
}

void Nanoshield_MRF::setChannel(int channel) {
  if (channel < 11 || channel > 26) {
    return;
  }

  // Change channel
  writeLong(MRF_RFCON0, (channel - 11) << 4 | 0x03);
  
  // Perform RF state machine reset and wait for RF circuitry to calibrate
  writeShort(MRF_RFCTL, 0b00000100);
  writeShort(MRF_RFCTL, 0);
  delay(1);
}

float Nanoshield_MRF::measureSignalStrength() {
  // Initiate RSSI calculation (RSSIMODE1 = 1)
  writeShort(MRF_BBREG6, 0b11000000);
  
  // Wait until conversion is ready and read RSSI
  while (!(readShort(MRF_BBREG6) & 0b00000001));
  rssi = readLong(MRF_RSSI);

  return getSignalStrength();
}

float Nanoshield_MRF::getSignalStrength() {
  return rssi * 0.197948 - 87.646977;
}

int Nanoshield_MRF::getLinkQuality() {
  return lqi;
}

void Nanoshield_MRF::sleep() {
  writeShort(MRF_SOFTRST, 0b00000100); // Perform Power Management Reset (RSTPWR = 1)
  writeShort(MRF_WAKECON, 0b10000000); // Enable Immediate Wake-up Mode (IMMWAKE = 1)
  writeShort(MRF_SLPACK, 0b10000000);  // Put the module to sleep (SLPACK = 1)
}

void Nanoshield_MRF::wakeup() {
  // Wake up via SPI
  writeShort(MRF_WAKECON, 0b11000000); // Set REGWAKE to 1
  delayMicroseconds(50);               // Delay at least 10us, according to forum post
  writeShort(MRF_WAKECON, 0b10000000); // Set REGWAKE to 0
  writeShort(MRF_RFCTL, 0b00000100);   // Reset RF State Machine
  writeShort(MRF_RFCTL, 0);
  delay(10);                           // Wait at least 2ms to allow oscillator to stabilize
}

bool Nanoshield_MRF::write(uint8_t b) {
  return writeToBuffer(&b, 1);
}

uint8_t Nanoshield_MRF::read() {
  uint8_t result;
  return readFromBuffer(&result, 1) ? result : 0;
}

bool Nanoshield_MRF::writeInt(int16_t i) {
  return writeToBuffer(&i, 2);
}

int16_t Nanoshield_MRF::readInt() {
  int16_t result;
  return readFromBuffer(&result, 2) ? result : 0;
}

bool Nanoshield_MRF::writeUnsignedInt(uint16_t u) {
  return writeToBuffer(&u, 2);
}

uint16_t Nanoshield_MRF::readUnsignedInt() {
  uint16_t result;
  return readFromBuffer(&result, 2) ? result : 0;
}

bool Nanoshield_MRF::writeLong(int32_t l) {
  return writeToBuffer(&l, 4);
}

int32_t Nanoshield_MRF::readLong() {
  int32_t result;
  return readFromBuffer(&result, 4) ? result : 0;
}

bool Nanoshield_MRF::writeUnsignedLong(uint32_t u) {
  return writeToBuffer(&u, 4);
}

uint32_t Nanoshield_MRF::readUnsignedLong() {
  uint32_t result;
  return readFromBuffer(&result, 4) ? result : 0;
}

bool Nanoshield_MRF::writeFloat(float f) {
  return writeToBuffer(&f, sizeof(float));
}

float Nanoshield_MRF::readFloat() {
  float result;
  return readFromBuffer(&result, sizeof(float)) ? result : 0;
}

int Nanoshield_MRF::writeBytes(uint8_t* buf, int size) {
  if (size > bytesLeftToWrite()) {
    size = bytesLeftToWrite();
  }

  writeToBuffer(buf, size);
  return size;
}

int Nanoshield_MRF::readBytes(uint8_t* buf, int size) {
  if (size > bytesLeftToRead()) {
    size = bytesLeftToRead();
  }

  readFromBuffer(buf, size);
  return size;
}

int Nanoshield_MRF::writeString(char* str) {
  if (bytesLeftToWrite() == 0) {
    return 0;
  }

  int size = strlen(str);
  
  if (size > bytesLeftToWrite() - 1) {
    size = bytesLeftToWrite() - 1;
  }

  writeToBuffer(str, size);
  write('\0');
  return size + 1;
}

const char* Nanoshield_MRF::readString(char* str, int size) {
  char* result = (char*)&rxBuf[rxCount];

  if (str == NULL) {
    return result;
  }

  if (size > bytesLeftToRead()) {
    size = bytesLeftToRead();
  }
  
  if (size == 0) {
    return 0;
  }

  str[size - 1] = '\0';
  strncpy(str, (char*)&rxBuf[rxCount], size - 1);
  rxCount += strlen(str) + 1;
  return result;
}

void Nanoshield_MRF::startPacket() {
  txCount = 0;
}

int Nanoshield_MRF::bytesLeftToWrite() {
  return MRF_MAX_PAYLOAD_SIZE - txCount;
}

bool Nanoshield_MRF::sendPacket(uint16_t addr, bool ack) {
  if (txCount == 0) {
    return false;
  }

  // Send header size, frame size and header
  int i = 0;
  writeLong(i++, MRF_MHR_SIZE);                  // Header size
  writeLong(i++, MRF_MHR_SIZE + txCount);        // Frame size
  writeLong(i++, ack ? 0b01100001 : 0b01000001); // Frame control bits 0-7 (data frame, no security, no pending data, ack disabled, intra-PAN)
  writeLong(i++, 0b10001000);                    // Frame control bits 8-15 (16-bit addresses with PAN)
  writeLong(i++, seqNumber++);                   // Sequence number
  writeLong(i++, panId);                         // PAN ID
  writeLong(i++, panId >> 8);
  writeLong(i++, addr);                          // Destination address
  writeLong(i++, addr >> 8);
  writeLong(i++, srcAddr);                       // Source address
  writeLong(i++, srcAddr >> 8);

  // Send data payload
  for (int j = 0; j < txCount; j++) {
    writeLong(i++, txBuf[j]);
  }

  // Start transmission
  writeShort(MRF_TXNCON, ack ? 0b00000101 : 0b00000001);
  
  return true;
}

bool Nanoshield_MRF::transmissionDone() {
  return readShort(MRF_INTSTAT) & 0b00000001;
}

bool Nanoshield_MRF::transmissionSuccess() {
  return !(readShort(MRF_TXSTAT) & 0b00000001);
}

bool Nanoshield_MRF::receivePacket() {
  int frameSize;

  // Check RXIF in INTSTAT
  if (readShort(MRF_INTSTAT) & 0b00001000) {
    // Disable receiver
    writeShort(MRF_BBREG1, 0b00000100);
    
    // Packet received, get the number of bytes
    frameSize = readLong(0x300);
    
    // Copy the message bytes into the user buffer
    rxSize = 0;
    rxCount = 0;
    while (rxSize < (frameSize - MRF_MHR_SIZE - MRF_MFR_SIZE)) {
      rxBuf[rxSize] = readLong(0x301 + MRF_MHR_SIZE + rxSize);
      rxSize++;
    }
    
    // Read RSSI and LQI
    lqi = readLong(0x301 + frameSize);
    rssi = readLong(0x301 + frameSize + 1);
  
    // Flush the reception buffer, re-enable receiver
    writeShort(MRF_RXFLUSH, 0b00000001);
    writeShort(MRF_BBREG1, 0);
    
    // Wait until RXIF is cleared (takes a while)
    while(readShort(MRF_INTSTAT) & 0b00001000);
    
    return true;
  }
  
  return false;
}

int Nanoshield_MRF::bytesLeftToRead() {
  return rxSize - rxCount;
}

uint8_t Nanoshield_MRF::readShort(uint8_t addr) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(cs, LOW);
  SPI.transfer(addr << 1 & 0b01111110);
  uint8_t result = SPI.transfer(0);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  return result;
}

void Nanoshield_MRF::writeShort(uint8_t addr, uint8_t data) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(cs, LOW);
  SPI.transfer((addr << 1 & 0b01111110) | 0b00000001);
  SPI.transfer(data);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

uint8_t Nanoshield_MRF::readLong(uint16_t addr) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(cs, LOW);
  SPI.transfer(addr >> 3 | 0b10000000);
  SPI.transfer(addr << 5);
  uint8_t result = SPI.transfer(0);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  return result;
}

void Nanoshield_MRF::writeLong(uint16_t addr, uint8_t data) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(cs, LOW);
  SPI.transfer(addr >> 3 | 0b10000000);
  SPI.transfer(addr << 5 | 0b00010000);
  SPI.transfer(data);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

bool Nanoshield_MRF::writeToBuffer(void* data, int size) {
  if (size > bytesLeftToWrite()) {
    return false;
  }

  memcpy(&txBuf[txCount], data, size);
  txCount += size;
  return true;
}

bool Nanoshield_MRF::readFromBuffer(void* data, int size) {
  if (size > bytesLeftToRead()) {
    return false;
  }

  memcpy(data, &rxBuf[rxCount], size);
  rxCount += size;
  return true;
}
