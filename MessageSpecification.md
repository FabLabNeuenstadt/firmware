# Audio transmission protocol definition


This document describes the communication between the blinkenrocket and the audio generating device attached to it. It relies on the implementation of the [tagsu-avr-modem](https://github.com/Jartza/tagsu-avr-modem) for low-level communication.

The communication relies on multiple components:

##### START 
A *`START`* signal which indicates the start of a transmission. It consists of two times a specific 8-bit binary pattern ( `10011001` respectively `0x99`).

##### PATTERN 
A *`PATTERN`* signal which indicates that either the start of an animation or text pattern. It consists of two times the 8-bit binary pattern ( `10101001` respectively `0xA9`)

##### HEADER 
A generic *`HEADER`* which contains two byte of metadata to describe the data that follows. The two byte contain 12 bit of length information and 4 bit of data type information.

```
XXXXXXXX XXXXYYYY
<----------->
    LENGTH   <-->
   			 TYPE
```

Thus the data length can be up to 4kByte of data (4096 byte). A type of `0001` denotes a `TEXT` type pattern, a type `0010` denotes an `ANIMATION` type pattern.
The modem only receives data for this pattern until length is exceeded. E.g. when a *`HEADER`* with the contents `11111111 11110001` is received by the modem it will read 4098 byte for the current pattern (2 byte header, 4096 byte of data).  

##### TEXT METADATA 

A *`TEXTMETA`* is a two byte (16 bit) length metadata field for text type pattern. It encodes the speed (first nibble), the delay (second nibble) and the direction (third nibble). The fourth nibble is reserved for future use.

```
XXXX XXXX XXXX XXXX (MSB -> LSB)
---- (speed)
     ---- (delay)
          ---- (direction)
               ---- (reserved)
```

The speed and delay ranges from a numeric value from 0 (0000) to 15 (1111). The higher the number, the faster the speed and the longer the delay. A direction of 0 (0000) specifies a left direction, a direction of 1 (0001) specifies a right direction.

##### ANIMATION METADATA

A *`ANIMMETA`* is a two byte (16 bit) length metadata field for animation type pattern. It encodes the frame rate in the lower nibble of the first byte and the delay in the lower nibble of the second byte.

```
0000XXXX 0000XXXX
<------> <------>
 SPEED     DELAY
```

The speed and delay ranges from a numeric value from 0 (0000) to 15 (1111).
## Message format

The message transmitted has to follow the following diagram:

```
       +--------------------------------------------------------+
       |                                                        |
       |                                         +--------+     |
       |                                         v        |     |
       |                         +--> TEXTMETA +--> DATA -+--+  |
       v                         |                           |  |
START +--> PATTERN +--> HEADER +-|                           +---> END
                                 |                           |
                                 +--> ANIMMETA +--> DATA -+--+
                                                 ^        |
                                                 +--------+
```

First there is a `START` signal to indicate that the transmission is about to start. This should initialize the internal state machine. After that a `PATTERN` indicator follows to indicate that a pattern is following. This is followed by the generic `HEADER` to set the size and type of the following data. Now depending on the type of the transmission there is either (XOR) the `TEXTMETA` or `ANIMMETA` fields followed by `DATA` fields that may repeat up to the length specified in the `HEADER`. After this the transmission either encounters an `END` or a repetition of the sequence starting with `PATTERN`.

Important note: the length of the data must be even (e.g. `length(data) % 2 == 0`) to make the Hamming(24,16) error correction work properly.

## Error detection and correction

The error correction is performed using the Hamming-Code. For this application the Hamming(24,16) code is used, which contains 2 bytes of data with a ECC of 1 byte. The error correction is capabable of correcting up to two bit flips, which should be sufficient for this application.

```
XXXXXXXX YYYYYYYY EEEEEEEE
-------- -------- --------
 First    Second   Hamming
```

The encoding of the Hamming code is performed using [this (click me)](https://github.com/RobotRoom/Hamming) avr-specific library and [this particular](https://github.com/RobotRoom/Hamming/blob/master/HammingCalculateParitySmallAndFast.c) implementation optimized for speed and code footprint.


