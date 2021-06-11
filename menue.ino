/*************************************************************************************
   Menue Steuerung für Nockenschleifmaschine
   Version: 1.4

   Arduino UNO

   Autor: Marcel Herzberg 02.2021
 *************************************************************************************/
#include "menue.h"


/* defaults */
#define SPACEVALUE=5

/* menue class */
menue::menue(int lcd_width, int lcd_hight, LiquidCrystal_I2C *display, RotaryEncoder *encoder)
{
	_lcd = display;
	_encoder = encoder;
	_num_items = 0;
	_lcd_width = lcd_width;
	_lcd_hight = lcd_hight;
	_curr_menue = 0;
	_curr_page = 0;
	_lcd_pos_value = lcd_width - SPACEVALUE;
	
}

void menue::set_space(int new_space)
{
	this->_lcd_pos_value = this->_lcd_width - new_space;
}



/* reduces encoder position between 0 and number of menues */
/* returns the current menue */
int chooseMenue(int newPos) {
  if ((newPos > numMenues) || (newPos < 0)) {
    encoder.setPosition(currentMenue);
    return currentMenue;
  }
  return newPos;
}
/* refresh the display with current information */
void showMenue() {
  char buf[20];
  int numMenue, i;
  // calc first line menue
  numMenue = (getPage() * lcdRow) + 1;
  // refresh the output
  lcd.clear();
  for (i = 0; i < lcdRow; i++) { // for each lcdRow
    if (numMenue > numMenues) break;
    sprintf(buf, "%d %s", numMenue, menue[numMenue]->title);
    lcd.setCursor(0, i);
    lcd.print(buf);
    lcd.setCursor(posValue, i);
    printValue(numMenue);
    //lcd.print(menue[numMenue]->value, menue[numMenue]->decimals);
    numMenue++;
  }
}
void printValue(int numMenue) {
  char buf[10];
  dtostrf(menue[numMenue]->value, spaceValue, decimals, buf);
  lcd.print(buf);
}

/* calculate the cursor position by using the current Menue as reference */
/* returns the current Cursor */
int calcCursor() {
  return (int)((currentMenue - (currentPage * lcdRow)) - 1);
}

/* calc Page of Lcd */
int getPage() {
  return (int)((currentMenue - 1) / lcdRow);
}

/* refresh cursor position to current menue */
void refreshCursor() {
  int i;
  static int lastPage;
  lcd.blink();

  if (!currentMenue) {
    lcd.home();
    currentCursor = 0;
    currentPage = 0;
    lastPage = 0;
    lcd.noBlink();
  }
  else {
    /* check page change */
    currentPage = getPage();
    if (currentPage != lastPage) {
      showMenue();
      lastPage = currentPage;
    }
    /* calc current cursor position */
    currentCursor = calcCursor();
    /* set cursor position */
    lcd.setCursor(0, currentCursor);
  }
}

/* change encoder position to current value */
void changeEncoderPos() {
  lcd.setCursor(posValue, currentCursor);
  currentValue = (int)(menue[currentMenue]->value / menue[currentMenue]->scale);
  encoder.setPosition(currentValue);
}

/* refresh the value when changing it (translate from encoder) */
void refreshValue(int newPos) {
  menue[currentMenue]->value = (float)(newPos * menue[currentMenue]->scale);
  lcd.setCursor(posValue, currentCursor);
  printValue(currentMenue);
  lcd.setCursor(posValue, currentCursor);
}

/* simple refresh value on lcd (only for first page)*/
void simpleRefreshValue(int posMenue) {
  lcd.setCursor(posValue, posMenue - 1);
  printValue(posMenue);
}

/* reset menue to start position*/
void resetMenue() {
  encoder.setPosition(0);
  currentMenue = 0;
  level = 0;
  messOn = 0;
  showMenue();
  refreshCursor();
}

/* reset menue after amount of time to start position
  does not block the main loop
  returns true if time has reached */
void delayResetMenue(int delayTime) {
  if (millis() - messStartTime >= delayTime) {
    // reset menue
    resetMenue();
  }
}

/* show message  Attention! needs alot of time
    needs a reset of Menue after it*/
void showMess(const char *message1, const char *message2) {

  Serial.print(message1);
  Serial.print(' ');
  Serial.print(message2);
  Serial.print('\n');
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.print(message1);
  lcd.setCursor(0, 1);
  lcd.print(message2);

  messOn = 1;
  messStartTime = millis();
}
