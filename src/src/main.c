#include <Arduino.h>

#define LED_BUILTIN 2

void setup(void) {
  // Initialization code here
  pinMode (LED_BUILTIN, OUTPUT);
}

void loop(void) {
  // Main code here
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
  delay(1000);                     // Wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off
  delay(1000);                     // Wait for a second
}
