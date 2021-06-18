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

#define VERSION=18

/* defaults */
#define SPACEVALUE=5
#define DECIMALS=2

/* menue class*/
class menue{
private:
	LiquidCrystal_I2C *_lcd;
	RotaryEncoder *_encoder;
	DigitalIn *_button;
	menue_item **_items;
	int _num_items
	int _lcd_row;
	int _lcd_col;
	int _curr_menue;
	int _curr_page;
	int _curr_cursor;
	bool _level;
	int _lcd_pos_value;
	
	void setSpace(int new_space);
	void setLevel();
	void changeItem(int new_pos);
	void changeValue(int new_pos);
	void refreshValue();
	void refreshLcd();
	void printValue();
	/* refresh cursor position to current menue */
	void refreshCursor();
	/* calculate the cursor position by using the current Menue as reference */
	/* returns the current Cursor row */
	int calcCursor();
	/* calc Page of Lcd */
	int calcPage();

public:
	/* constructor */
	menue(int lcd_width, int lcd_hight, LiquidCrystal_I2C *display, RotaryEncoder *encoder);

	void initItem(int index, menueItem new_item);

	void run();

	void reset()
	

class menueItem : public menue {
private:
	menue *_menue;
	char *_title;
	float _value;
	float _scale;
	
public:
	/* constructor */
	menue_item(menue *menue, char *title, float value, float scale);

	char *getTitle();
	
	void setValue(float newValue);

	float getValue();

	int getScaledValue();

	void setScaledValue(int value);
};

#endif
