Nanoshield_MRF
==============

This is our new Arduino library to use the 2.4GHz `MRF24J40 wireless radio`_ from Microchip.
It supports communication using the 802.15.4 wireless protocol at the MAC level, and enables use of
the `MRF24J40MA/B/C/D/E wireless modules`_ from Microchip.

The reference board to use this library is the `MRF24J40 Nanoshield`_ from Circuitar.

The following features are provided:

* Basic transmission and reception
* Packet assembly/disassembly with different data types: byte, signed/unsigned integer,
  signed/unsigned long integer, floating point, character string and byte array
* PAN ID selection
* Set/unset module as network coordinator
* External PA/LNA control to use MRF24J40B/C/D/E modules from Microchip
* Channel frequency selection
* Automatic acknowledgement and packet retransmission
* Measurement of received power
* Sleep mode to reduce power consumption

To install, just copy it under ``<Arduino sketch folder>/libraries/``

The following examples are provided:

- SendInteger_ + ReceiveInteger_: transfer a packet over the air containing one integer number.
- SendString_ + ReceiveString_ or ReceiveStringIntoBuffer_: transfer a packet over the air
  containing one character string.
- SendPacket_ + ReceivePacket_: transfer an assembled packet over the air containing multiple types
  of data together.
- ReceiveBytes_: receives wireless packets, printing their content as a raw byte string. It also
  prints received signal strength and link quality indicators (RSSI and LQI). It can be used along
  with any of the `Send`* examples above.
- ReceiveWithInterrupt_: similar to ReceiveBytes, but reception is done using an interrupt signal.
- ChannelScanner_: scans all channels (11 to 26) and plots the average received power on each one.
  It is useful to assess the clearest channel to use.
- SleepWakeup_: how to put the module to sleep and wake it up.

.. _`MRF24J40 wireless radio`: http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en027752
.. _`MRF24J40 Nanoshield`: https://www.circuitar.com.br/en/nanoshields/modules/mrf24j40/
.. _`MRF24J40MA/B/C/D/E wireless modules`: http://www.microchip.com/pagehandler/en-us/technology/personalareanetworks/home.html
.. _ChannelScanner: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ChannelScanner/ChannelScanner.ino
.. _LinkTest: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/LinkTest/LinkTest.ino
.. _ReceiveBytes: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ReceiveBytes/ReceiveBytes.ino
.. _ReceiveInteger: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ReceiveInteger/ReceiveInteger.ino
.. _ReceivePacket: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ReceivePacket/ReceivePacket.ino
.. _ReceiveString: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ReceiveString/ReceiveString.ino
.. _ReceiveStringIntoBuffer: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ReceiveStringIntoBuffer/ReceiveStringIntoBuffer.ino
.. _ReceiveWithInterrupt: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/ReceiveWithInterrupt/ReceiveWithInterrupt.ino
.. _SendInteger: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/SendInteger/SendInteger.ino
.. _SendPacket: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/SendPacket/SendPacket.ino
.. _SendString: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/SendString/SendString.ino
.. _SleepWakeup: https://github.com/circuitar/Nanoshield_MRF/blob/master/examples/SleepWakeup/SleepWakeup.ino

----

Copyright (c) 2015 Circuitar
All rights reserved.

This software is released under an MIT license. See the attached LICENSE file for details.
