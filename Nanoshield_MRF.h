/**
 * This is our new Arduino library to use the 2.4GHz MRF24J40 wireless radio from Microchip.
 * It supports communication using the 802.15.4 wireless protocol at the MAC level, and enables
 * use of the MRF24J40MA/B/C/D/E wireless modules from Microchip.
 *
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
*/
#ifndef NANOSHIELD_MRF_h
#define NANOSHIELD_MRF_h

#include <Arduino.h>
#include <SPI.h>

// Packet Transmission Details
//
// 802.15.4 MAC Data Frame (max size = max PSDU size = 127)
// -------------------------------------------------------------
// | MAC Header (MHR) | Data Payload (MSDU) | MAC Footer (MFR) |
// -------------------------------------------------------------
// |        9         |       1 - 116       |         2        |
// -------------------------------------------------------------
//
// 802.15.4 MAC Header (MHR)
// -------------------------------------------------------
// | Frame Control | Sequence Number | Addressing Fields |
// -------------------------------------------------------
// |       2       |        1        |         6         |
// -------------------------------------------------------
//
// 802.15.4 Addressing Fields (16-bit addressing with PAN ID)
// -------------------------------------------------
// | PAN ID | Destination Address | Source Address |
// -------------------------------------------------
// |    2   |          2          |        2       |
// -------------------------------------------------
//
// MRF24J40 TX FIFO
// ---------------------------------------------------------------------
// | Header Length | Frame Length | Header (MHR) | Data Payload (MSDU) |
// ---------------------------------------------------------------------
// |       1       |       1      |       9      |       1 - 116       |
// ---------------------------------------------------------------------

#define MRF_PAN_ID_SIZE 2
#define MRF_DEST_ADDR_SIZE 2
#define MRF_SRC_ADDR_SIZE 2
#define MRF_ADDR_FIELDS_SIZE (MRF_PAN_ID_SIZE + MRF_DEST_ADDR_SIZE + MRF_SRC_ADDR_SIZE)
#define MRF_MHR_SIZE (2 + 1 + MRF_ADDR_FIELDS_SIZE)
#define MRF_MFR_SIZE 2
#define MRF_MAX_PSDU_SIZE 127
#define MRF_MAX_PAYLOAD_SIZE (MRF_MAX_PSDU_SIZE - MRF_MHR_SIZE - MRF_MFR_SIZE)
#define MRF_MAX_TX_FIFO_SIZE (1 + 1 + MRF_MHR_SIZE + MRF_MAX_PAYLOAD_SIZE)

// Packet Reception Details
//
// MRF24J40 RX FIFO
// ------------------------------------------------------------------------
// | Frame Length | Header (MHR) | Data Payload (MSDU) | FCS | LQI | RSSI |
// ------------------------------------------------------------------------
// |       1      |       9      |       1 - 116       |  2  |  1  |   1  |
// ------------------------------------------------------------------------

#define MRF_MAX_RX_FIFO_SIZE (1 + MRF_MHR_SIZE + MRF_MAX_PAYLOAD_SIZE + 2 + 1 + 1)

enum Mrf24j40Type { MRF24J40MA, MRF24J40MB, MRF24J40MC, MRF24J40MD, MRF24J40ME };

class Nanoshield_MRF
{
  public:

    Nanoshield_MRF(Mrf24j40Type type, int cs = A3);
    void begin(void);
    void setPanId(uint16_t panId);
    void setAddress(uint16_t addr);
    void setPaLna(bool paLna);
    void setCoordinator(bool coord);
    void setChannel(int channel);
    int getSignalStrength();
    int getLinkQuality();

    bool write(uint8_t b);
    uint8_t read();
    bool writeInt(int16_t i);
    int16_t readInt();
    bool writeUnsignedInt(uint16_t u);
    uint16_t readUnsignedInt();
    bool writeLong(int32_t l);
    int32_t readLong();
    bool writeUnsignedLong(uint32_t u);
    uint32_t readUnsignedLong();
    bool writeFloat(float f);
    float readFloat();
    int writeBytes(uint8_t* buf, int size);
    int readBytes(uint8_t* buf, int size);
    int writeString(char* str);
    const char* readString(char* str = NULL, int size = 0);
    
    void startPacket();
    int bytesLeftToWrite();
    bool sendPacket(uint16_t addr, bool ack = false);
    bool transmissionDone();
    bool transmissionSuccess();
    bool receivePacket();
    int bytesLeftToRead();

  private:
    static SPISettings spiSettings;
  
    int cs;
    uint16_t panId;
    uint16_t srcAddr;
    bool paLna;
    uint8_t seqNumber;
    int txCount;
    uint8_t txBuf[MRF_MAX_PAYLOAD_SIZE];
    int rxCount;
    int rxSize;
    uint8_t rxBuf[MRF_MAX_RX_FIFO_SIZE];
    uint8_t rssi;
    uint8_t lqi;
 
    uint8_t readShort(uint8_t address);
    void writeShort(uint8_t address, uint8_t data);
    uint8_t readLong(uint16_t address);
    void writeLong(uint16_t address, uint8_t data);
    bool writeToBuffer(void* data, int size);
    bool readFromBuffer(void* data, int size);
};

#endif
