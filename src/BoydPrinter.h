/* Library to control Seiko MTP-102 thermal printer.
   Uses Boyd Dimension Calculator as a control interface.
   The Arduino code sends commands to an 80C49 processor
   in the calculator which in turn controls the printer.
   https://gitlab.com/Andy4495/BoydPrinter


   Interface requires 10 digital out pins and 1 digital input pin.

*/
#ifndef BOYD_PRINTER
#define BOYD_PRINTER

#include <Arduino.h>

// Maximum time to wait for /READY signal (ms)
#define MAX_READY_WAIT 1000
// Time (us) to wait after /READY signal before changing data lines or sending INT
#define READY_DELAY    200
// Number of columns per print line
#define MAX_COLUMNS    13
// Time to hold /INT active (us)
#define INT_DELAY      10
// Time to hold Form Feed (PF) signal active (us)
#define PF_DELAY       200

// Additional delays when not using /READY pin:
// Delay between characters sent to printer (us)
#define NO_READY_DELAY 700
// Delay for printhead to return home after form feed (ms)
#define FF_DELAY      1800

// If not using /READY pin
#define NO_READY_PIN   255

class BoydPrinter {
public:
  BoydPrinter(byte D0, byte D1, byte D2, byte D3, byte D4, byte D5, byte D6,
              byte D7, byte int_pin, byte PF_pin, byte ready_pin = NO_READY_PIN);
  int println(const char*);
  void print(char);
  void printcr();
  void printff();
  void cancelprint();

private:
  byte d0_out, d1_out, d2_out, d3_out, d4_out, d5_out, d6_out, d7_out;
  byte interrupt_out;
  byte pf_out;
  byte ready_in;
  void send_byte(byte c);
};
#endif
