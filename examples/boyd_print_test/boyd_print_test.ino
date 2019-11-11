/* Example program to demonstrate BoydPrinter library functions.
 * https://github.com/Andy4495/BoydPrinter
 *
 * 02/16/2018 - A.T. - Original
 * 03/04/2018 - A.T. - Updated with PF and Serial input example
 *
 */
#include "BoydPrinter.h"

BoydPrinter myPrinter(2, 3, 4, 5, 6, 7, 8, 9, 17, 15, 16);
//BoydPrinter myPrinter(2, 3, 4, 5, 6, 7, 8, 9, 17, 15);    // Skip last parameter if not using /READY signal

// PF input button --> Not required, but supported if used
// Digital 18 == A4
#define PF_INPUT_BUTTON 18
#define PF_BUTTON_REPEAT_DELAY 500
unsigned long last_pf_time = 0;


// how much serial data we expect before a newline
const unsigned int MAX_INPUT = 50;

char input_line[MAX_INPUT];

void setup() {
  int ret;

  pinMode(PF_INPUT_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  delay(5000);
  Serial.println("MTP102 Thermal Printer Ready...");

  ret = myPrinter.println("123456789ABCD");
  Serial.print("123456789ABCD: ");
  Serial.print(ret);
  Serial.println(" characters printed. ");

  myPrinter.printff();
  Serial.println("Form Feed");

  myPrinter.print('X');
  myPrinter.print('Y');
  myPrinter.print('Z');
  myPrinter.printcr();
  Serial.println("XYZ sent one character at a time.");

  myPrinter.print('Q');
  myPrinter.cancelprint();
  Serial.println("Cancelled.");

  // Printer ignores everything after 13th character.
  ret = myPrinter.println("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  Serial.print("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ: ");
  Serial.print(ret);
  Serial.println(" characters printed.");

  Serial.println(" ");
  Serial.println("Now accepting serial input....");

}

void loop() {

  // if serial data available, process it
  while (Serial.available () > 0)
    processIncomingByte (Serial.read ());

  if (digitalRead(PF_INPUT_BUTTON) == LOW) {
    if ((millis() - last_pf_time) > PF_BUTTON_REPEAT_DELAY) {
      last_pf_time = millis();
      myPrinter.printff();
      Serial.println("Form Feed");
    }
  }
}

// here to process incoming serial data after a terminator received
void process_data (const char * data)
{
  myPrinter.println(data);
  Serial.println(data);
}

void processIncomingByte (const byte inByte)
{
  static unsigned int input_pos = 0;

  switch (inByte)
  {
    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte
      input_line[13] = '\0';  // Adding terminator just in case... not really needed.
      process_data (input_line);
      // reset buffer for next time
      input_pos = 0;
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1)) {
        input_line [input_pos++] = inByte;
      }
      break;
  }
}
