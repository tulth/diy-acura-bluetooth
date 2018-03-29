#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
// #include <core_pins.h>

#define LED_PIN 13

extern "C" int main(void)
{
  setup();
  while (1) {
    loop();
  }
}

void setup() {
  // initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
}



void loop() {
  digitalWrite(LED_PIN, HIGH);   // set the LED on
  delay(200); 
  digitalWrite(LED_PIN, LOW);    // set the LED off
  delay(200);
}
