/* Example program to demonstrate BoydPrinter library functions. 
 * https://gitlab.com/Andy4495/BoydPrinter
 *  
 * 02/16/2018 - A.T. - Original
 * 
 */
#include "BoydPrinter.h"

BoydPrinter myPrinter(2, 3, 4, 5, 6, 7, 8, 9, 15, 16, 14);
//BoydPrinter myPrinter(2, 3, 4, 5, 6, 7, 8, 9, 15, 16);

void setup() {
  int ret;

  Serial.begin(9600);

  pinMode(10, INPUT_PULLUP);
  Serial.println("Waiting for Pin 10 low before starting...");

  while (digitalRead(10) == HIGH) {
    delay(1000);
  }
  Serial.println("Pin 10 low detected... Starting: ");

  ret = myPrinter.println("123456789ABCD");
  Serial.print("123456789ABCD: ");
  Serial.println(ret);
  
  myPrinter.printff();
  Serial.println("Form Feed");
  
  myPrinter.print('X');
  myPrinter.print('Y');
  myPrinter.print('Z');
  myPrinter.printcr();
  Serial.println("XYZ");
  
  myPrinter.print('Q');
  myPrinter.cancelprint();
  Serial.println("Cancelled.");

  // Printer ignores everything after 13th character. 
  ret = myPrinter.println("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  Serial.print("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ: ");
  Serial.println(ret);

  Serial.println("Done.");
}

void loop() {



}
