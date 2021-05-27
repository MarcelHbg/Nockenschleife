/*************************************************************************************
   Menue Steuerung für Nockenschleifmaschine
   Version: 1.4

   Arduino UNO

   Autor: Marcel Herzberg 02.2021
 *************************************************************************************/
#ifndef menue_h
#define menue_h

/* Basic Cofigurations */

/***************************************************/
/* Menue Line */
typedef struct { // Description of one Line
  const char *title;           // title
  float value;           // important Value
  float scale;         
  //unsigned int decimals;          
}Menue_t;

/* variables **************************************************************************/

int currentCursor = 0; // position of cursor
int currentMenue = 0;
int currentValue = 0;
int currentPage = 0; // page on the Lcd screen
bool level = 0;
bool messOn = 0;  // is true when a message is shown on lcd
unsigned long messStartTime;
/**************************************************************************************/

/* functions **************************************************************************/
/* reduces encoder position between 0 and number of menues */
/* returns the current menue */
int chooseMenue(int newPos);
/* refresh the display with current information */
void showMenue();
/* refresh cursor position to current menue */
void refreshCursor();
/* change encoder position to current value */
void changeEncoderPos();
/* refresh the value when changing it (translate from encoder) */
void refreshValue(int newPos);
/* simple refresh value on lcd (only for first page)*/
void simpleRefreshValue(int posMenue);
/* reset menue to start position*/
void resetMenue();
/* reset menue after amount of time to start position
    does not block the main loop */
void delayResetMenue(int delayTime);
/* show message  Attention! needs alot of time 
    needs a reset of Menue after it*/
void showMess(const char *message1, const char *message2);
/* calculate the Value to mm float value */
float calcFloatValue(int numMenue);
/**************************************************************************************/
#endif
