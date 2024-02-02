#include "led.h"

int led =  LED_BUILTIN;

void setupPins() {
  pinMode(led, OUTPUT);      // set the LED pin mode
}