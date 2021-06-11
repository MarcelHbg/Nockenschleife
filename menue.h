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

/* menue class*/
class menue{
private:
	LiquidCrystal_I2C *_lcd;
	RotaryEncoder *_encoder;
	menue_item *_items[16];
	int _num_items
	int _lcd_width;
	int _lcd_hight
	int _curr_menue;
	int _curr_page;
	int _lcd_pos_value;

public:
	/* constructor */
	menue(int lcd_width, int lcd_hight, LiquidCrystal_I2C *display, RotaryEncoder *encoder);
	/* set space of value on lcd */
	void set_space(int new_space);
	/* return current menue item index */
	int get_menue();
	/* set new menue item */
	void set_menue(int new_pos);
	/* refresh lcd output commpletly */
	void refresh_lcd();
	
	

class menue_item : public menue {
private:
	menue *_menue;
	char *_title;
	float _value;
	float _scale;

	float value_to_float();
	
public:
	/* constructor */
	menue_item(menue *menue, char *title, float value, float scale);
	/* set encoder position to value of item */
	set_encoder();
	/* refresh lcd output only value when changing */
	refesh_lcd_value();
};


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
