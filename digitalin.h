/*************************************************************************************
   Digital Input reading with debounce
   diffrent modes to read (as botton or switch)
   does not block the main loop 
   Version: 1.3

   Arduino UNO

   Autor: Marcel Herzberg 02.2021
 *************************************************************************************/
#ifndef digitalin_h
#define digitalin_h

#include "Arduino.h"

class DigitalIn {
  public:
    /* Constructor *******************************************************************
      pins are used as INPUT_PULLUP
      reaktion on negative Edge
    */
    DigitalIn(int pin);

    /* Read pin as button *************************************************************
      returns the current state of the button
      1 represents one push
    */
    bool buttonIn();
    
    /* Read pin as switch *************************************************************
      returns the current state of the switch
      one push toggles the state
    */
    int buttonSwitch();
    
  protected:
    bool debounce();
    
  private:
    int _pin;
    bool sig1;
    bool sig2;
    bool edge;
    bool stat;
    unsigned long delayTime = 10; //ms to debounce 

    

};

#endif
