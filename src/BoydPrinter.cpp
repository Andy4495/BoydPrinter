/* Library to control Seiko MTP-102 thermal printer.

   1.0.0    02/12/2018  A.T.   Original
*/
/*
   Uses Boyd Dimension Calculator as a control interface.
   The Arduino code sends commands to an 80C49 processor
   in the calculator which in turn controls the printer.
   See github, discussion board, olduino web pages

   Interface requires 10 digital out pins and 1 digital input pin.

   Pin Configuration:
   80C49               ARDUINO
   ----                -------

   The Arduino code deals with some "nuances" in the 80C49 firmware code:
   1. Arduino runs much faster than 80C49, so need to insert delays to
      ensure proper interfacing with the 80C49
   2. Even though the 80C49 makes available a "/READY" pin to indicate
      when it is ready to receive printer data, the code has not quite
      completed it's setup before activating /READY. So additional
      delays need to be added even when /READY is detected.

*/
/* Constructor:
      D0 - D7   : 8 digital output pins used to send characters to printer controller
      int_pin   : digital output pin used as an interrupt signal to printer controller
      RF_pin    : digital output pin used to signal a form feed request to printer controller
      ready_pin : digital input pin used to detect when printer controller is ready to receive
                  In exchange for slightly reduced print speed it is possible
                  to ignore the /READY pin (which is the way the Boyd printer
                  works anyway). You can also save an I/O pin by not using
                  /READY. To ignore the /READY pin, leave it off the constructor
                  parameter list.
*/
#include "BoydPrinter.h"

BoydPrinter::BoydPrinter(byte D0_pin, byte D1_pin, byte D2_pin, byte D3_pin,
                         byte D4_pin, byte D5_pin, byte D6_pin, byte D7_pin,
                         byte int_pin, byte PF_pin, byte ready_pin)
{
  d0_out =             D0_pin;
  d1_out =             D1_pin;
  d2_out =             D2_pin;
  d3_out =             D3_pin;
  d4_out =             D4_pin;
  d5_out =             D5_pin;
  d6_out =             D6_pin;
  d7_out =             D7_pin;
  interrupt_out =      int_pin;     // Active low
  pf_out =             PF_pin;      // Active low
  ready_in =           ready_pin;   // Active low

  // Data pins don't need to be initialized in constructor, since they are only
  // latched by the 80c49 processor when the interrupt signal is sent.
  pinMode(d0_out, OUTPUT);
  pinMode(d1_out, OUTPUT);
  pinMode(d2_out, OUTPUT);
  pinMode(d3_out, OUTPUT);
  pinMode(d4_out, OUTPUT);
  pinMode(d5_out, OUTPUT);
  pinMode(d6_out, OUTPUT);
  pinMode(d7_out, OUTPUT);
  digitalWrite(interrupt_out, HIGH);  // Make sure interrupt signal is inactive
  pinMode(interrupt_out, OUTPUT);
  digitalWrite(pf_out, HIGH);         // Make sure paper feed signal is inactive
  pinMode(pf_out, OUTPUT);
  if (ready_in != NO_READY_PIN)
    pinMode(ready_in, INPUT);
}

int BoydPrinter::println(char* s) {
  int i = 0;
  for (i=0; i < MAX_COLUMNS; i++) {
    if (s[i] == '\0') break;
    send_byte(s[i]);
  }
  // If not using /READY pin, then delay for printhead to return home
  if (ready_in == NO_READY_PIN)
    delay(FF_DELAY);

  return i;
}

void BoydPrinter::print(char c) {
  send_byte(c);
}

void BoydPrinter::printcr() {
  send_byte(0x0d);

  // If not using /READY pin, then delay for printhead to return home
  // Delay for 2X since only a partial line may have printed
  if (ready_in == NO_READY_PIN)
    delay(2 * FF_DELAY);
}

void BoydPrinter::printff() {
  unsigned long start_time;

  start_time = millis();
  if (ready_in != NO_READY_PIN) {
    // Wait for READY (but not forever)
    while (digitalRead(ready_in) == 1) {
      if ((millis() - start_time) > MAX_READY_WAIT) break;
    }
    // Wait a little after /READY active before making changes
    // The 80C49 firmware isn't quite "ready" when it activates /READY
    delayMicroseconds(READY_DELAY);
  }
  // Pull PF line low
  digitalWrite(pf_out, LOW);
  delayMicroseconds(PF_DELAY);  // Hold PF line active long enough for 80C49 to detect
  digitalWrite(pf_out, HIGH);

  // If not using /READY pin, then delay for printhead to return home
  if (ready_in == NO_READY_PIN)
    delay(FF_DELAY);
}

void BoydPrinter::cancelprint() {
// ASCII 0x18 CANCEL
  send_byte(0x18);

  // If not using /READY pin, then delay for printhead to return home
  if (ready_in == NO_READY_PIN)
    delay(FF_DELAY);
}

void BoydPrinter::send_byte(byte c) {
  unsigned long start_time;

  start_time = millis();
  if (ready_in != NO_READY_PIN) {
    // Wait for READY (but not forever)
    while (digitalRead(ready_in) == 1) {
      if ((millis() - start_time) > MAX_READY_WAIT) break;
    }
    // Wait a little after /READY active before making changes
    // The 80C49 firmware isn't quite "ready" when it activates /READY
    delayMicroseconds(READY_DELAY);
  }
  // Set output pins to data value
  digitalWrite(d0_out,     c  & 0x01);
  digitalWrite(d1_out, (c>>1) & 0x01);
  digitalWrite(d2_out, (c>>2) & 0x01);
  digitalWrite(d3_out, (c>>3) & 0x01);
  digitalWrite(d4_out, (c>>4) & 0x01);
  digitalWrite(d5_out, (c>>5) & 0x01);
  digitalWrite(d6_out, (c>>6) & 0x01);
  digitalWrite(d7_out, (c>>7) & 0x01);

  // Send interrupt
  digitalWrite(interrupt_out, LOW);
  delayMicroseconds(INT_DELAY);       // Hold active long enough for 80C49 to detect
  digitalWrite(interrupt_out, HIGH);

  // Hardcode a delay if not using /READY pin
  if (ready_in == NO_READY_PIN)
    delayMicroseconds(NO_READY_DELAY);
}
