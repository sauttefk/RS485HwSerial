The **RS485HwSerial** library is an enhanced version of the standard Arduino serial
functions with control for RS485 transmit enable.  
It automatically enables the transmit enable pin `TE` on half-duplex RS485 transceivers
when a character has to be sent and disables the `TE` pin when the last stop bit was sent
`TXC`, by utilizing the USART transmit complete interrupt `USART_TXC_vect`.  
The `TE` signal is only active (and therefore the transmitter blocking the bus) as long as it is really needed.  
Half-duplex RS485 communication can be achieved **non-blocking**

This is a major improvement over
* hardware solutions that use monoflops to control RS485 `TE` triggered by the `TX`-signal
* software solutions that are (blocking) waiting for the transmit buffer to be empty and
another extra delay for the last byte to be shifted out

#### Alternative hardware solution
Instead of using a microcontroller based solution for controlling the `TE` signal like this or any other RS485 library, you may use a RS485 transceiver with _auto direction control_ like the [MAX13487E](https://www.maximintegrated.com/en/products/interface/transceivers/MAX13487E.html).

## Installation
* Download the RS485HwSerial master zip file.
* Extract all files into a `RS485HwSerial` subdirectory in your `Arduino/libraries` directory.

## Usage

```
#include <RS485HwSerial.h>

// RS485 transceiver's transmit enable pin (often called TE or DE).
// Can be tied together with negated receive enable pin to disable local echo
// (often called /RE). This pin can freely be chosen.
#define RS485_TE  13

// RX pin of serial 1 on ATmega328PB. if /RE on RS485 transceiver is used then
// there should be pull-up resistor on RX or a least a weak pull-up configured on
// the RX pin (see below), as the RO pin of the RS485 transceiver becomes high
// impedance if the receiver is disabled.
#define RS485_RX  12

// Baud rate/BPS on RS485 port
#define RS485_BPS  9600


void setup() {
  // rs485 transceiver transmitter enable pin
  RS485HwSerial1.transmitterEnable(RS485_TE);
  // rs485 uart setup
  RS485HwSerial1.begin(RS485_BPS);

  // enable weak pull-up on RX pin (better use pull-up resistor)
  pinMode(RS485_RX, INPUT_PULLUP);
}


void loop() {
  RS485HwSerial1.println(F("\nHello Serial1 RS485"));
  delay (1000);
}
```

## Illustration of the `TE` signal and half-duplex usage of the RS458-bus

```
      _____ __ _..._ __ _____________..._______________
RXD:       X__X_..._X__X

      __________..._______ __ __ __ _..._ __ __________
TXD:                      X__X__X__X_..._X__X  

                         ____________...______
TE:   __________..._____|                     |________

      __________...______                 _____________
UDRE:                    ||__|__|__|_..._|

      __________...______                     _________
TXC:                     |___________..._____|

      -----RX---...-----|-------TX---...------|---RX---
```
`RXD`: USART Receive Data  
`TXD`: USART Transmit Data  
`TE`: Transceiver Transmit Enable  
`UDRE`: USART Data Register Empty  
`TXC`: USART Transmit Complete  

## Connecting RS485 Transceiver
ASCII schematic of connecting a half-duplex RS485 transceiver e.g. MAX487CSA+ to a microcontroller.  
Shielded twisted pair network cable is recommended. E.g. S/UTP, F/UTP, SF/UTP

```
----+      * Vcc
    |      |
AVR |     | | 47k     /|             * Vcc
    |     | |        / | A           |
    |     | |       /  +-------+    | | 560R
    |      |   RO  /   |       |    | | bias pull-up
 RX +------*------+    |       |    | | (only on master)
    |              \   | B     |     |
    |           /RE o  o---+   *-----*-------------------------- RS485 A/+
    |               |\ |   |   |     |
    |               | \|   |   |     |
 TE +---------------*      |   |    | | 120R
    |               |      |   |    | | termination resistor
    |             |\|      |   |    | | (on each bus end)
    |             | + TE   |   |     |
    |             |  \   B |   |     |
    |          DI |   o----*---|-----*-------------------------- RS485 B/-
 TX +-------------+    \       |     |
    |             |     +      |    | | 560R                +--- cableshield
    |             |    / A     |    | | bias pull-down      |
    |             |   +--------+    | | (only on master)   | | 100R
    |             |  /               |                     | | 1W
    |             |/                 * GND                 | |
----+                                                       |
                                                            * GND           
```
Excellent information about the electrical aspects of RS485 like proper termination, biasing
and shielding can be found here:  
http://support.fccps.cz/download/adv/frr/RS485/RS485.html  
This covers additionally the aspects of ESD-protection:  
https://www.renesas.com/eu/en/www/doc/whitepapers/interface/rs-485-transceiver-tutorial.pdf  
https://www.renesas.com/eu/en/www/doc/whitepapers/interface/rs-485-transceiver-tutorial_de.pdf  

## Author
* **[Frank Sautter](https://sautter.com)**
* Based on the Arduino hardware serial library of **Nicholas Zambetti**
