/*************************************************************************************
   Lcd menue
   originaly for camshaft grinding machine
   
   steerable with encoder + button
   
   using the LiquidCrystal_I2C and RotaryEncoder library

   tested on Arduino Nano

   Autor: Marcel Herzberg
   
   latest changes 06.2021
 *************************************************************************************/
#include "menue.h"

#define ITEM(a) this->_items[(a) - 1]

/* ++++++++++++++ menue class */
Menue::Menue(int lcd_col, int lcd_row, LiquidCrystal_I2C *display, RotaryEncoder *encoder, DigitalIn *button)
{
	_lcd = display;
	_encoder = encoder;
	_button = button;
	memset(_items, 0x0, MAXITEMS*sizeof(MenueItem*));
	_lcd_col = lcd_col;
	_lcd_row = lcd_row;
	_curr_menue = 0;
	_curr_page = 0;
	_curr_cursor = 0;
	_level = false;
	_lcd_pos_value = lcd_col - SPACEVALUE;
	_num_items = 0;
}

void Menue::initItem(int index, MenueItem *new_item)
{
	//if (index >= MAXITEMS) throw "overflow";
	this->_items[index] = new_item;
  this->_num_items++;
}

void Menue::run()
{
  /* read encoder during runtime */
	this->_encoder->tick();
	int new_pos = this->_encoder->getPosition();

  /* level 0 menue */
  if (!this->_level) {
	if (this->_button->buttonIn()) {
		setLevel();
	}
    changeItem(new_pos);
  }
  
  /* level 1 change values */
  else {
    /* check finish */
    if (this->_button->buttonIn()) {
      reset(); // go to home
    }
    /* block negative range */
    if (new_pos < 1) {
      this->_encoder->setPosition(1);
      new_pos = 1;
    }
    changeValue(new_pos);
  }
}

/* reset menue to start position*/
void Menue::reset()
{
  this->_encoder->setPosition(0);
  this->_curr_menue = 0;
  this->_level = 0;
  refreshLcd();
}

void Menue::refreshValue(int index)
{
	int cursor = calcCursor(index);
	this->_lcd->setCursor(this->_lcd_pos_value, cursor);
	printValue(index);
	this->_lcd->setCursor(this->_lcd_pos_value, cursor);
}

/* private: */

void Menue::setSpace(int new_space)
{
	this->_lcd_pos_value = this->_lcd_col - new_space;
}

void Menue::setLevel()
{
	if (this->_curr_menue) { // just set if menue not at home (!= 0)
		this->_lcd->setCursor(this->_lcd_pos_value, this->_curr_cursor);
		this->_encoder->setPosition(ITEM(this->_curr_menue)->getScaledValue());
		this->_level = 1;
	}
}

void Menue::changeItem(int new_pos)
{
	if ((new_pos > this->_num_items) || (new_pos < 0)) { // no change
		this->_encoder->setPosition(this->_curr_menue);
	}
	else {
		if (new_pos != this->_curr_menue) { // changed
			this->_curr_menue = new_pos;
			refreshCursor();
		}
	}
}

void Menue::changeValue(int new_pos)
{
	if (new_pos != ITEM(this->_curr_menue)->getScaledValue()) { // continue after change
      ITEM(this->_curr_menue)->setScaledValue(new_pos);
      refreshValue(this->_curr_menue);
    }
}

void Menue::refreshLcd()
{
	char title[20];
	int num_menue, i;
	// calc first line menue
	num_menue = (calcPage() * this->_lcd_row) + 1;
	// refresh the output
	this->_lcd->clear();
	for (i = 0; i < this->_lcd_row; i++) { // for each lcdRow
		if (num_menue > this->_num_items) break;
		sprintf(title, "%d %s", num_menue,ITEM(num_menue)->getTitle());
		this->_lcd->setCursor(0, i);
		this->_lcd->print(title);
		this->_lcd->setCursor(this->_lcd_pos_value, i);
		printValue(num_menue);
		num_menue++;
	}
	this->_lcd->home();
}

void Menue::printValue(int index)
{
	int space_val = this->_lcd_col - this->_lcd_pos_value;
	char value[space_val + 1];
	sprintf(value, "%SPACEVALUE.DECIMALSf", ITEM(index)->getValue());
	this->_lcd->print(value);
}

/* refresh cursor position to current menue */
void Menue::refreshCursor() 
{
  static int lastPage;
  this->_lcd->blink();

  if (!this->_curr_menue) {
    this->_lcd->home();
	this->_lcd->noBlink();
    this->_curr_cursor = 0;
    this->_curr_page = 0;
    lastPage = 0;
  }
  else {
    /* check page change */
    this->_curr_page = calcPage();
    if (this->_curr_page != lastPage) {
      refreshLcd();
      lastPage = this->_curr_page;
    }
    /* calc current cursor position */
    this->_curr_cursor = calcCursor(this->_curr_menue);
    /* set cursor position */
    this->_lcd->setCursor(0, this->_curr_cursor);
  }
}

/* calculate the cursor position by using the current Menue as reference */
/* returns the current Cursor row */
int Menue::calcCursor(int index)
{
  return ((index - (this->_curr_page * this->_lcd_row)) - 1);
}

/* calc Page of Lcd */
int Menue::calcPage()
{
  return (int)((this->_curr_menue - 1) / this->_lcd_row);
}

/* ----------- menue class */

/* +++++++++++ menue item class */
MenueItem::MenueItem(Menue *menue, const char *title, float value, float scale, int index)
{
	menue->initItem(index, this);
	_menue = menue;
	_title = title;
	_value = value;
	_scale = scale;
}

const char *MenueItem::getTitle()
{
	return this->_title;
}

void MenueItem::setValue(float newValue)
{
	//if (newValue >= 100) this->_menue->setSpace(6);
	this->_value = newValue;
}

float MenueItem::getValue()
{
	return this->_value;
}

int MenueItem::getScaledValue()
{
	return (int)(this->_value / this->_scale);
}

void MenueItem::setScaledValue(int value)
{
	this->_value = (float)(value * this->_scale);
}

/* ------------- menue item class */
