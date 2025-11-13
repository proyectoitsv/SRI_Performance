#include <Arduino.h>
void setup(void) {
  pinMode(2, OUTPUT);
}
void loop(void) {
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  delay(1000);
}