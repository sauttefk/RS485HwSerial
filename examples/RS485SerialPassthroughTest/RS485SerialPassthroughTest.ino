/*
  Example serial passthrough sketch for the RS485HwSerial library to test if
  your hardware setup works. Tested on a ATmega328PB having two hardware USARTs.

  This library currently only works on atmel avr chips. It works with hardware
  USARTs like the single one on the ATmega328PB. Multiple hardware USARTs are
  also supported e.g. available on the ATmega2560 (4) or the new ATmega328PB (2).

  The RS485HwSerial library is an enhanced version of the standard Arduino
  serial functions. It automatically enables the transmit enable pin (TE) on
  half-duplex RS485 transceivers when a character has to be sent and disables
  the TE pin when the last stop bit was sent (TXC), by utilizing the USART
  transmit complete interrupt (USART_TXC_vect).

  2019-02-03 by Frank Sautter
*/

#include <RS485HwSerial.h>

////////////////////////////////////////////////////////////////////////////////
// RS485_TE:
// rs485 transceiver's transmit enable pin (often called TE or DE).
// can be tied together with negated receive enable pin to disable local echo
// (often called /RE). this pin can freely be chosen.
const uint8_t RS485_TE = 13;

// RS485_RX:
// RX pin of serial 1 on atmega328pb. if /RE on rs485 transceiver is used then
// there should be pullup resistor on RX or a least a weak pullup configured on
// the RX pin (see below), as the RO pin of the rs485 transceiver becomes high
// impedance if the receiver is disabled.
const uint8_t RS485_RX = 12;

// RS485_BPS:
// baud rate/bps on rs485 port
const uint32_t RS485_BPS = 9600;

// RS232_BPS:
// baud rate/bps on rs232 port
const uint32_t RS232_BPS = 115200;


/*//////////////////////////////////////////////////////////////////////////////
  illustration of the TE signal and half-duplex usage of the RS458-bus

      _____ __ _..._ __ _____________..._______________
RXD:       X__X_..._X__X

      __________..._______ __ __ __ _..._ __ __________
TXD:                      X__X__X__X_..._X__X

                         ____________...______
TE:   __________..._____|                     |________

      __________...______                 _____________
UDRE:                    ||__|__|__|_..._|

      __________...______                      ________
TXC:                     |___________...______|

      -----RX---...-----|-------TX---...------|---RX---

RXD:  USART Receive Data
TXD:  USART Transmit Data
TE:   Transceiver Transmit Enable
UDRE: USART Data Register Empty
TXC:  USART Transmit Complete
*/


/*//////////////////////////////////////////////////////////////////////////////
  ascii schematic of connecting half-duplex RS485 transceiver e.g. MAX487CSA+
  to a microcontroller.
  shielded twisted pair network cable is recommended. e.g. S/UTP, F/UTP, SF/UTP

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

  excellent information about the electrical aspects of RS485 like proper
  termination, biasing and shielding can be found here:
  http://support.fccps.cz/download/adv/frr/RS485/RS485.html
  this covers additionally the aspects of ESD-protection:
  https://www.renesas.com/eu/en/www/doc/whitepapers/interface/rs-485-transceiver-tutorial.pdf
  https://www.renesas.com/eu/en/www/doc/whitepapers/interface/rs-485-transceiver-tutorial_de.pdf

  */

////////////////////////////////////////////////////////////////////////////////
void setup() {
  RS485HwSerial0.begin(RS232_BPS);

  // rs485 transceiver transmitter enable pin
  RS485HwSerial1.transmitterEnable(RS485_TE);
  // rs485 uart setup
  RS485HwSerial1.begin(RS485_BPS);
  // enable weak pull-up on RX pin
  pinMode(RS485_RX, INPUT_PULLUP);

  RS485HwSerial0.println(F("\n\nHello Serial0 RS232"));
  RS485HwSerial0.println(F("Characters typed in here on Serial0 will be sent to Serial1."));

  RS485HwSerial1.println(F("\n\nHello Serial1 RS485"));
  RS485HwSerial1.println(F("Characters typed in here on Serial1 will be sent to Serial0."));
}


void loop() {
  if (RS485HwSerial0.available()) {                // any data incoming on serial0,
    RS485HwSerial1.write(RS485HwSerial0.read());   // read it in and send it out on serial1
  }

  if (RS485HwSerial1.available()) {                // any data incoming on serial1,
    RS485HwSerial0.write(RS485HwSerial1.read());   // read it in and send it out on serial0
  }
}
