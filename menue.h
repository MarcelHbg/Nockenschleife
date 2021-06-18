/*************************************************************************************
   Lcd menue
   originaly for camshaft grinding machine
   
   steerable with encoder + button
   
   using the LiquidCrystal_I2C and RotaryEncoder library

   tested on Arduino Nano

   Autor: Marcel Herzberg
   
   latest changes 06.2021
 *************************************************************************************/
#ifndef menue_h
#define menue_h

#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "digitalin.h"

#define VERSION 18

/* defaults */
#define SPACEVALUE 5
#define DECIMALS 2
#define MAXITEMS 16

class MenueItem;
/* Menue class*/
class Menue{
private:
  LiquidCrystal_I2C*_lcd = 0x0;
  RotaryEncoder *_encoder;
  DigitalIn *_button;
  int _lcd_row;
  int _lcd_col;
  int _curr_menue;
  int _curr_page;
  int _curr_cursor;
  bool _level;
  int _lcd_pos_value;
  int _num_items;
  MenueItem *_items[MAXITEMS];

  void setSpace(int new_space);
  void setLevel();
  void changeItem(int new_pos);
  void changeValue(int new_pos);
  
  void refreshLcd();
  void printValue(int index);
  /* refresh cursor position to current menue */
  void refreshCursor();
  /* calculate the cursor position by using the current Menue as reference */
  /* returns the current Cursor row */
  int calcCursor(int index);
  /* calc Page of Lcd */
  int calcPage();
  
public:
	/* constructor */
	Menue(int lcd_col, int lcd_row, LiquidCrystal_I2C *display, RotaryEncoder *encoder, DigitalIn *button);

	void initItem(int index, MenueItem *new_item);

	void run();

	void reset();
 
  void refreshValue(int index);
};

class MenueItem{
private:
  Menue *_menue;
  const char *_title;
  float _value;
  float _scale;
  
public:
  /* constructor */
  MenueItem(Menue *menue, const char *title, float value, float scale, int index);

  const char *getTitle();
  
  void setValue(float newValue);

  float getValue();

  int getScaledValue();

  void setScaledValue(int value);
};

#endif
