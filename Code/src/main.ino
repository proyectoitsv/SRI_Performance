#include <Arduino.h>

#define LED_BUILTIN 2

void setup() {
  // Initialization code here
  pinMode (LED_BUILTIN, OUTPUT);
}

void loop() {
  // Main code here
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
  delay(500);                     // Wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off
  delay(500);                     // Wait for a second
}
