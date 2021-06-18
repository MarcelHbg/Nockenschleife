/*************************************************************************************
   Steuerung Nockenschleifmaschine
   Version: 1.6

   Arduino UNO

   Autor: Marcel Herzberg 04.2021
 *************************************************************************************/
#include "config.h"
#include <TimerOne.h>

/* variables **************************************************************************/
// constant axis factor calc
const unsigned long stepsPerRevC = driverStepsC * gearC; // steps for one revolution of the workpiece
const float feedPerStepZ = (threadPitchZ / (driverStepsZ * gearZ)); // mm per Step linear movement
/* adjustable factors */
unsigned long stepsPerFeedC;
unsigned int stepSpeedC; //speed in steps per second
// Steps Z
unsigned long currStepsZ;
unsigned long aimStepsZ;
unsigned long feedStepsZ;
unsigned long feedOutStepsZ;
unsigned long fastFeedStepsZ;
// check parameters
volatile unsigned long stepsC = 0; // counter of steps for C
//unsigned long stepsCCopy = 0;
float init_value = curr.getValue(); // to save current dia before start feeding
bool cOn = false;
bool zTouched = false;   // check if z is initialized
bool abortAF = false;    // check abort of Autofeed
bool bfState = false;    // check if Z if feeding back
//modes
unsigned int mode = 0;
// register Map -> first byte = PD, second byte = PB
const unsigned int regMap[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5};
unsigned int regStepC; // register bit StepC PIN

unsigned long mess_start_time = 0;
bool mess_on = false;
/**************************************************************************************/
/* reset menue after amount of time to start position
  does not block the main loop
  returns true if time has reached */
void delayResetMenue(unsigned int delay_time) {
  if (millis() - mess_start_time >= delay_time) {
    // reset menue
    menue.reset();
	mess_on = false;
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

  mess_on = true;
  mess_start_time = millis();
}
/* show message an reset programm*/
void messReset(const char *message1, const char *message2) {
  /* show Message */
  showMess(message1, message2);
  digitalWrite(LED_BUILTIN, LOW);
  /* reset mode */
  mode = 0;
}

void setup()
{
  //Serial.begin(115200);
  //Serial.print("Nockenschleifmaschine V1.6");
  //Serial.print('\n');

  /* display initialize */
  //lcd.begin(lcdCol, lcdRow); //normal
  lcd.init();                //I2C
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nockenschleifer");
  lcd.setCursor(0, 1);
  lcd.print("Version 1.6");
  delay(1000);
  
  menue.reset();

  /* C axis initialize */
  pinMode(pinStepAxisC, OUTPUT);
  pinMode(pinDirAxisC, OUTPUT);
  digitalWrite(pinDirAxisC, directionC);
  regStepC = regMap[pinStepAxisC];
  Timer1.initialize(speedCtime()); //speedCtime()


  /* Z axis initialize */
  axisZ.setPinsInverted(directionZ, false, false);
  axisZ.setMaxSpeed(1000); // Steps per second
  axisZ.setAcceleration(20000);
  axisZ.setMinPulseWidth(30);
  axisZ.setSpeed(500); // Steps per second
  fastFeedStepsZ = calcStepsZ(fastFeedZ);

  /* PINS initialize */
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //Serial.print("Bereit");
  //Serial.print('\n');

}

void loop() {
  //SerialChangeMode(); // just for testing
  if (zTouched && mode != 3) ledBlink(1000);
  /* reset when message is shown after amount of millis */
  if (mess_on) delayResetMenue(3000);
  /* run diffrent modes */
  switch (mode) {
    default:  /* Menue mode (standard) */
      /* main funktion */
      menue.run();
      /* switch C axis */
      if (C_on.buttonIn()) {
        if (cOn) stopCmess();
        else startC();
      }
      /* switch to TouchOff mode */
      if (touchOff.buttonIn()) {
        if (cOn) showMess("Error!!", "C abschalten!");
        else {
          mode = 2;
          initTouchOff();
        }
      }
      /* switch to AutoFeed mode => blocked Err message */
      if (autoStart.buttonIn()){
        if (!cOn) showMess("Error!!", "C einschalten!");
        else if (!zTouched) showMess("Error!!", "Z nicht angetastet");
        else {
          mode = 3;
          initAutoFeed();
        }
      }
      break;

    case 1: /* fast TouchOff mode */
      /* main funktion */
      runTouchOff(fastFeedStepsZ);
      /* check slow touch off */
      if (C_on.buttonIn()) {
        mode = 2;
        resetTouchOff();
        showMess("Eilgang", "deaktiviert");
      }
      /* check abort */
      if (touchOff.buttonIn()) messReset("Antasten", "abgebrochen");
      break;

    case 2: /* TouchOff mode */
      /* main funktion */
      runTouchOff(feedStepsZ);
      /* fast touch OFF */
      if (C_on.buttonIn()) {
        mode = 1;
        resetTouchOff();
        showMess("Eilgang", "aktiviert");
        //Serial.print("FeedZ: ");
        //Serial.print(fastFeedStepsZ);
        //Serial.print('\n');
      }
      /* check abort */
      if (touchOff.buttonIn()) messReset("Antasten", "abgebrochen");
      break;

    case 3: /* AutoFeed mode */
      /* main funktion */
      runAutoFeed();
      /* check abort */
      if (autoStart.buttonIn() && !bfState) {
        mode = 4;
        showMess("Autofeed", "pausiert");
      }
      break;

    case 4: /* AutoFeed pause mode */
      /* main funktion */
      if (abortAF) backFeed("abgebrochen"); // backfeed has to repeated frequently
      else {
        /* check commit */
        if (choose.buttonIn()) {
          mode = 3;
          showMess("Autofeed", "fortgesetzt");
        }
      }
      /* check abort */
      if (autoStart.buttonIn()) {
        abortAF = true; // set abort to run loop for backfeed
      }
      break;
  }
} // Main LOOP

/* funktion for main loop theese has to be as fast as possible */
/*-----------------------------------------------------------------------------------*/
/* Autofeed mode funcs */
/*-----------------------------------------------------------------------------------*/
void initAutoFeed() {
  stepsC = 0; // reset steps of C
  curr.setValue(init_value); //reset current dia to orig
  menue.refreshValue(RUNNINGVALUE);
  /*calc all used Parameters */
  currStepsZ = calcStepsZ(curr.getValue()); // curr Diameter in steps
  aimStepsZ = calcStepsZ(aim.getValue()); // aim Diameter in steps
  feedStepsZ = calcStepsZ(feedZ.getValue()); // feed in steps
  feedOutStepsZ = calcStepsZ(feedOutZ.getValue()); // feedOut in steps
  stepsPerFeedC = stepsPerRevC * (int)(rotationsC.getValue()); // amount of steps between feeds
  /* setup */
  axisZ.setCurrentPosition(currStepsZ); //set motor position
  digitalWrite(LED_BUILTIN, HIGH); // signal for auto mode on
  /* message */
  showMess("Autofeed", "eingeschaltet");
  //Serial.print("feedStepsZ: ");
  //Serial.print(feedStepsZ);
  //Serial.print('\n');
  //Serial.print("aimStepsZ: ");
  //Serial.print(aimStepsZ);
  //Serial.print('\n');
  //Serial.print("currStepsZ: ");
  //Serial.print(currStepsZ);
  //Serial.print('\n');


}

void runAutoFeed() {
  // check finish values
  if (currStepsZ == aimStepsZ) { // reach aim diameter
    if (stepsC >= (stepsPerRevC * finishRotC.getValue())) { // free grinding
      backFeed("abgeschlossen");
    }
  }
  else if (currStepsZ < aimStepsZ) backFeed("Werte fehler"); // Error value

  else if ((currStepsZ - feedStepsZ) < aimStepsZ) { // check overfeed
    int intLastFeed = currStepsZ - aimStepsZ; // calc last feed
    feeding(intLastFeed);
  }
  else {
    feeding(feedStepsZ);
  }
}

void backFeed(const char* message) {
  static bool bf = false;
  if (!bf) {
    axisZ.move(feedOutStepsZ); // set backfeeding aim
    bf = true;
  }
  else if (!bfState) {
    bf = false;
    abortAF = false;
    endAutoFeed(message);
  }
  bfState = axisZ.run();
}

void endAutoFeed(const char* message) {
  zTouched = 0;
  curr.setValue(init_value); // reset current dia
  menue.refreshValue(RUNNINGVALUE);
  digitalWrite(LED_BUILTIN, LOW);
  stopC();
  messReset("Autofeed", message);
}

void feeding(int steps) {
  static bool freshValue = true;   // check if fresh value is shown
  // after an amount of revolution
  if (stepsC >= (stepsPerFeedC)) {
    axisZ.move(-steps); //set new target pos (relative)
    //Serial.print(-steps);
    //Serial.print('\n');
    freshValue = false;
  }
  if (axisZ.run()) stepsC = 0;
  /* if position reached refresh value just one time */
  if (!(axisZ.distanceToGo()) && freshValue == false) {
    unsigned long stepPos = axisZ.currentPosition();
    //Serial.print(stepPos);
    //Serial.print('\n');
    if (stepPos != currStepsZ - feedStepsZ) ledBlink(100); // error
    currStepsZ = stepPos;
    curr.setValue(currStepsZ * feedPerStepZ);
    menue.refreshValue(RUNNINGVALUE);
    freshValue = true;
  }
}
/*-----------------------------------------------------------------------------------*/
/* Run C funcs loop independent just 1 call*/
/*-----------------------------------------------------------------------------------*/
unsigned long speedCtime() {
  float revPerSec = (speedC.getValue() / 60.0); // revolution per second
  stepSpeedC = (unsigned int)(revPerSec * stepsPerRevC); // in Hz (steps per second)
  float yHz = ((float)(stepSpeedC) / 1000000);
  return ((1 / yHz) / 2);
}

void startC() {
  /* start wave signal for step independent of loop */
  Timer1.attachInterrupt(runC, speedCtime());
  cOn = true;
  
  /* message */
  showMess("Achse C", "eingeschaltet");
  //Serial.print("speedC: ");
  //Serial.print(stepSpeedC);
  //Serial.print('\n');
  //Serial.print("timeC: ");
  //Serial.print(speedCtime());
  //Serial.print('\n');
  delay(300);
}

void stopC() {
  Timer1.detachInterrupt();
  PORTB &= ~(1 << regStepC); // set Pin Low
  cOn = false;
}

void stopCmess() {
  stopC();
  showMess("Achse C", "ausgeschaltet");
}

/* callback function for interrupt fast as possible
  build wave signal */
void runC() {
  if (PINB & (1 << regStepC)) { // if high
    PORTB ^= (1 << regStepC); // invert pin
    stepsC++;
  }
  else { // if low
    PORTB ^= (1 << regStepC); // invert pin
  }
}

/*-----------------------------------------------------------------------------------*/
/* Touch off mode funcs */
/*-----------------------------------------------------------------------------------*/
void initTouchOff() {
  zTouched = false; // reset z status
  digitalWrite(LED_BUILTIN, LOW);
  feedStepsZ = calcStepsZ(feedZ.getValue());
  resetTouchOff();
  /* message */
  showMess("Antasten", "eingeschaltet");
  //Serial.print("FeedZ: ");
  //Serial.print(feedStepsZ);
  //Serial.print('\n');

}

void resetTouchOff(){
  encoder.setPosition(0); // reset encoder
  axisZ.setCurrentPosition(0); //reset motor position
}

void runTouchOff(unsigned long feedSteps) {
  // read encoder
  encoder.tick();
  int touchPos = encoder.getPosition();
  // run to new position
  axisZ.moveTo(touchPos * feedSteps);
  axisZ.run();
  // end if choosed
  if (choose.buttonIn()) {
    zTouched = true; // set status
    resetTouchOff();
    messReset("Antasten", "abgeschlossen");
  }
}
/*-----------------------------------------------------------------------------------*/
/* help funcs */
/*-----------------------------------------------------------------------------------*/

long calcStepsZ(float value) {
  return (unsigned long)(value / feedPerStepZ); // calculate steps of Z
}

void ledBlink(unsigned int delayTime) {
  static unsigned long runTime;
  static bool ledState = false;  // for builtinLED
  unsigned long currentTime = millis();

  if (currentTime - runTime >= delayTime) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    runTime = currentTime;
  }
}
/*
void SerialChangeMode() {
  if (Serial.available() > 0) {
    long num = readNumber(1);
    if (!num) return;
    else {
      mode = num;
      Serial.print("Mode: ");
      Serial.print(mode);
      Serial.print('\n');
      if (mode == 3) {
        startC();
        initAutoFeed();
      }
      if (mode == 2) initTouchOff();
    }

  }
}
long readNumber(int numDigits) {
  char buffer[numDigits + 1];
  buffer[numDigits] = '\0';
  Serial.readBytes(buffer, numDigits);
  return atol(buffer);
}
*/
