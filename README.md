Nanoshield_MRF
==============

This is our new Arduino library to use the 2.4GHz [MRF24J40 wireless radio](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en027752) from Microchip.
It supports communication using the 802.15.4 wireless protocol at the MAC level, and enables use of the [MRF24J40MA/B/C/D/E wireless modules](http://www.microchip.com/pagehandler/en-us/technology/personalareanetworks/home.html) from Microchip.

The reference board to use this library is the [MRF24J40 Nanoshield](https://www.circuitar.com.br/en/nanoshields/modules/mrf24j40/) from Circuitar.

The following features are provided:

* Basic transmission and reception
* Packet assembly/disassembly with different data types: byte, signed/unsigned integer, signed/unsigned long integer, floating point, character string and byte array
* PAN ID selection
* Set/unset module as network coordinator
* External PA/LNA control to use MRF24J40B/C/D/E modules from Microchip
* Channel frequency selection
* Automatic acknowledgement and packet retransmission
* Measurement of received power

To install, just copy it under &lt;Arduino sketch folder&gt;/libraries/

The following examples are provided:

- **SendInteger** + **ReceiveInteger**: transfer a packet over the air containing one integer number.
- **SendString** + **ReceiveString** or **ReceiveStringIntoBuffer**: transfer a packet over the air containing one character string.
- **SendPacket** + **ReceivePacket**: transfer an assembled packet over the air containing multiple types of data together.
- **ReceiveBytes**: receives wireless packets, printing their content as a raw byte string. It also prints received signal strength and link quality indicators (RSSI and LQI). It can be used along with any of the **Send*** examples above.
- **ReceiveWithInterrupt**: similar to ReceiveBytes, but reception is done using an interrupt signal.
- **ChannelScanner**: scans all channels (11 to 26) and plots the average received power on each one. It is useful to assess the clearest channel to use.

---
Copyright (c) 2015 Circuitar
All rights reserved.

This software is released under an MIT license. See the attached LICENSE file for details.
