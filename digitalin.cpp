/*************************************************************************************
   Digital Input reading with debounce
   diffrent modes to read (as botton or switch)
   does not block the main loop 
   Version: 1.3

   Arduino UNO

   Autor: Marcel Herzberg 02.2021
 *************************************************************************************/
#include "digitalin.h"
#include "Arduino.h"
/* functions **************************************************************************/
DigitalIn::DigitalIn(int pin) {
  // Remember Hardware Setup
  _pin = pin;

  // Inital status

  edge = true; // 1 to 0 falling
  stat = false; // state when used as switch

  pinMode(pin, INPUT_PULLUP);
}
/* debounce input */
bool DigitalIn::debounce(void) {
  static unsigned long runTime;
  unsigned long currentTime = millis();

  sig1 = digitalRead(_pin);
  if (currentTime - runTime >= delayTime) { // delay to filter signals
    sig2 = digitalRead(_pin);
    runTime = currentTime;
  }
  if (sig1 == sig2) return true;
  else return false;
}

/* read digital input and return 1 in case of a negative edge */
bool DigitalIn::buttonIn(void) {
  if (debounce()) {
    /* in case of negative edge return 1 */
    if (sig1 != edge) {
      edge = sig1;
      if (sig1 == false) {
        return true;
      }
    }
  }
  return false;
}
/* read digital input and toggle in case of a negative edge */
/* needs the digital pin and a global int variable for the status */
int DigitalIn::buttonSwitch(void) {
  if (debounce()) {
    /* in case of negative edge toggle status */
    if (sig1 != edge) {
      edge = sig1;
      if (sig1 == false) {
        if (!stat) stat = true;
        else stat = false;
      }
    }
  }
  return stat;
}
