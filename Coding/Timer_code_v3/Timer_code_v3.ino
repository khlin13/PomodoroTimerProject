#include <EEPROM.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/AGENCYR20pt7b.h>
#include <Fonts/AGENCYR11pt7b.h>
#include "char_storage.h"
#include "gif_check_thin.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DIGIT_WIDTH 14
#define DIGIT_HEIGHT 32

const uint8_t DIGIT_X[] PROGMEM = {0, 20, 46, 66, 92, 112, 11};
const int DIGIT_Y PROGMEM = 56;

#define btnL 2
#define btnR 3
#define btnTop 8
#define motorPinA 4
#define motorPinB 5
#define motorEn 6
#define buzzer 7
#define redPin 9
#define greenPin 10
#define bluePin 11

//button debounce 
boolean btnState[3] = {false, false, false};
boolean lastBtnState[3] = {false, false, false};
boolean btnL_rec = false;
boolean btnR_rec = false;
boolean LnR_rec = true;
boolean LnR_released = true;

const int debounceDelay PROGMEM = 50;
unsigned long lastDebounceTime[3] = {0, 0, 0};

//button click
const int clickDelay PROGMEM = 50;
unsigned long lastClickedTime[3] = {0, 0, 0};
unsigned long lastReleasedTime[3] = {0, 0, 0};

//button long press
const int longPrsDelay PROGMEM = 1000;
const int incDelay PROGMEM = 800;
int incDelayCounter = 0;

//timer variables
const int autoSetDelay PROGMEM = 2500;
const int minTimerSec PROGMEM = 10;

byte timerHr, timerMin, timerSec, focusIntv, restMin, restSec;

byte selected = 0;
boolean isScreenOn = false;
boolean timerMode = false; //false->interval, true->timer
boolean isTimeSet = true;
boolean isCounting = false;
boolean hasTimerStarted = false;
boolean timerOverflow = false;

//counting
unsigned long focusMillis = 0;
unsigned long restMillis = 0;
unsigned long expTime = 0;
unsigned long timeLeft = 0;
byte repLeft = 0;
boolean isResting = false;
boolean isRestIntvSet = false;

byte timerPercent = 0;

//alarm tone
const uint8_t alarmTone[] PROGMEM = {0, 70, 140, 1000};
byte alarmIndex = 0;
byte alarmCount = 0;
unsigned long alarmOnTime = 0;
boolean isAlarmOn = false;
boolean isAlarmOn_ = false;

//colors
const uint8_t blue[] PROGMEM = {10, 80, 80};
const uint8_t pink[] PROGMEM = {150, 10, 5};
const uint8_t purple[] PROGMEM = {40, 0, 50};
const uint8_t red[] PROGMEM = {150, 0, 0};

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  pinMode(btnL, INPUT);
  pinMode(btnR, INPUT);
  pinMode(btnTop, INPUT);
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);
  pinMode(motorEn, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(115200);

  timerHr = EEPROM.read(0);
  timerMin = EEPROM.read(1);
  timerSec = EEPROM.read(2);
  focusIntv = EEPROM.read(3);
  restMin  = EEPROM.read(4);
  restSec = EEPROM.read(5);

  if (focusIntv > 0 && (restMin > 0 || restSec > 0) )
    isRestIntvSet = true;
  else
    isRestIntvSet = false;

  if(!Display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  else {
    //configure text
    Display.setTextSize(2);
    Display.setTextColor(WHITE, BLACK);
    //hello message
    Display.clearDisplay();
    Display.setCursor(36,28);
    Display.println((__FlashStringHelper*)str_hello);
    Display.display();
    delay(1600);
    //clear display
    Display.clearDisplay();
    Display.display();
    Display.setTextSize(1);
  }
}

void loop() {
  //run timer
  if (isCounting)
    tickTimer();

  if (isAlarmOn) {
    checkAlarm();
    isAlarmOn_ = true;
  }
  else {
    if (isAlarmOn != isAlarmOn_) {
      noTone(buzzer);
      ledOff();
      isAlarmOn_ = false;
    }
  }

  //Single-press inputs
  boolean btnL_pressed = isBtnClicked(btnL);
  boolean btnR_pressed = isBtnClicked(btnR);
  boolean btnTop_pressed = isBtnClicked(btnTop);
  boolean LnR_pressed = false;

  //checking how long the buttons has been pressed
  unsigned long btnL_dur = millis()-lastClickedTime[0];
  unsigned long btnR_dur = millis()-lastClickedTime[1];
  unsigned long btnTop_dur = millis()-lastClickedTime[2];

  //Long-press inputs
  boolean btnL_longPrs = (btnL_dur > longPrsDelay) * btnState[0];
  boolean btnR_longPrs = (btnR_dur > longPrsDelay) * btnState[1];
  boolean btnTop_longPrs = (btnTop_dur > longPrsDelay) * btnState[2];

  //checking if both L & R buttons are released
  if (btnState[0] && btnState[1]) {
    LnR_released = false;
  }
  else if (!btnState[0] && !btnState[1]) {
    LnR_released = true;
  }

  //distinguishing buttons commands: only L, only R, L+R
  if ((btnL_dur < clickDelay) && (btnR_dur < clickDelay)) { //when both buttons are pressed
    if (LnR_rec != true) { //making sure the click is recognized only once
      LnR_pressed = true;
      LnR_rec = true;
    }
  }
  else {
    LnR_rec = false;
    btnL_pressed = false;
    btnR_pressed = false;
  }

  if (LnR_released) {
    if (btnL_dur > clickDelay) {
      if (btnState[0] != btnL_rec) {
        btnL_rec = btnState[0];
        if (btnL_rec == true) {
          btnL_pressed = true;
        }
      }
    }
    
    if (btnR_dur > clickDelay) {
      if (btnState[1] != btnR_rec) {
        btnR_rec = btnState[1];
        if (btnR_rec == true) {
          btnR_pressed = true;
        }
      }
    }
  }
  //----------------------------------------------
  if(isScreenOn) {
    if (!hasTimerStarted) {
      if (LnR_pressed) {
        //reset
        if (timerMode) {
          timerHr = 0;
          timerMin = 0;
          timerSec = 0;
          selected = 3;
        }
        else {
          focusIntv = 2;
          restMin  = 0;
          restSec = 0;
          selected = 1;
        }
        
        updateScreen();
      }
      else if (btnL_pressed) {
        //decrese
        decreaseDigit();
  
        updateScreen();
      }
      else if (btnR_pressed) {
        //increase
        increaseDigit();
  
        updateScreen();
      }
      else if (btnL_longPrs) {
        if (incDelayCounter < incDelay) {
          incDelayCounter += 1;
        }
        else {
          incDelayCounter = 0;
          decreaseDigit();
          updateScreen();
        }
      }
      else if (btnR_longPrs) {
        if (incDelayCounter < incDelay) {
          incDelayCounter += 1;
        }
        else {
          incDelayCounter = 0;
          increaseDigit();
          updateScreen();
        }
      }
      else if (btnTop_pressed) { //next
        changeSelection();
  
        updateScreen();
      }
      else {
        if (!isTimeSet) {
          //wait and exit on the last selection
          if ((timerMode && selected == 1) || (!timerMode && selected == 3))  
            checkBtnClick();
        }
        else {
          checkBtnClick();
        }
        incDelayCounter = 0;
      }
    }
    else { //timer has started
      if (btnTop_pressed) {
        if (isCounting)
          pauseTimer();
        else
          resumeTimer();
      }
      else {
        checkBtnClick();
      }
    }
    
    
  }

  //-------------------------------------------------
  else { //screen off

    if (btnTop_pressed) {
      //start timer
      Serial.println("Top button is pressed!");

      if (isCounting) {
        pauseTimer();
      }
      else {
        if (isAlarmOn) {
          isAlarmOn = false;
          alarmCount = 0;
          alarmIndex = 0;
        }
        else if (timerHr == 0 && timerMin == 0 && timerSec < minTimerSec) {
          errorBeep();
        }
        else if (hasTimerStarted) {
          resumeTimer();
        }
        else {
          startTimer();
        }
      }
    }
    if (hasTimerStarted) {
      if (btnTop_longPrs) {
        stopTimer();
      }
      else if (btnL_pressed || btnR_pressed) {
        previewSetTime();
      }
    }
    else {
      if (LnR_pressed) {
        //preview set time
//        Serial.println("Both left and right buttons are pressed!");
        previewSetTime();
      }
      else if (btnL_pressed) {
        //set interval mode
//        Serial.println("Left button is pressed!");
  
        timerMode = false;
        isTimeSet = false;
        selected = 1;
        updateScreen();
      }
      else if (btnR_pressed) {
        //set focus timer
//        Serial.println("Right button is pressed!");
  
        timerMode = true;
        isTimeSet = false;
        selected = 3;
        updateScreen();
      }
    }
  }
}

//-------------------------------------------------------------------------

boolean isBtnClicked(int btnPin) {
  int index = -1;
  switch (btnPin) {
    case 2:
      index = 0;
      break;
    case 3:
      index = 1;
      break;
    case 8:
      index = 2;
      break;
  }

  if (index != -1) {
    int reading = digitalRead(btnPin);
  
    if (reading != lastBtnState[index]) {
      lastDebounceTime[index] = millis();
    }
  
    if (millis() - lastDebounceTime[index] > debounceDelay) {
      if (reading != btnState[index]) {
        btnState[index] = reading;
        if (btnState[index] == HIGH) {
          lastClickedTime[index] = millis();
          return true;
        }
        else {
          lastReleasedTime[index] = millis();
        }
      }
    }

    lastBtnState[index] = reading;
  }

  return false;
}

void checkBtnClick() {
  boolean timesUp = (millis()-lastReleasedTime[0] > autoSetDelay) && 
                    (millis()-lastReleasedTime[1] >autoSetDelay) && 
                    (millis()-lastReleasedTime[2] > autoSetDelay) &&
                    (millis()-lastClickedTime[0] > autoSetDelay) && 
                    (millis()-lastClickedTime[1] > autoSetDelay) && 
                    (millis()-lastClickedTime[2] > autoSetDelay);
  
  if (timesUp) {
    if (isTimeSet) {
      screenOff();
    }
    else {
      isTimeSet = true;
      
      if (focusIntv > 0 && (restMin > 0 || restSec > 0) ) {
        isRestIntvSet = true;
      }
      else {
        isRestIntvSet = false;
      }

      //save data
      EEPROM.write(0,timerHr);
      EEPROM.write(1,timerMin);
      EEPROM.write(2,timerSec);
      EEPROM.write(3,focusIntv);
      EEPROM.write(4,restMin);
      EEPROM.write(5,restSec);
      
      updateScreen();
    }
  }
}

void increaseDigit() {
  if (timerMode) {
    if (selected == 3 && timerSec < 59)
      timerSec += 1;
    else if (selected == 2 && timerMin < 59)
      timerMin += 1;
    else if (selected == 1 && timerHr < 23)
      timerHr += 1;
  }
  else {
    if (selected == 1 && focusIntv < 9)
      focusIntv += 1;
    else if (selected == 2 && restMin < 99)
      restMin += 1;
    else if (selected == 3 && restSec < 99)
      restSec += 1;
  }
}

void decreaseDigit() {
  if (timerMode) {
    if (selected == 3 && timerSec > 0)
      timerSec -= 1;
    else if (selected == 2 && timerMin > 0)
      timerMin -= 1;
    else if (selected == 1 && timerHr > 0)
      timerHr -= 1;
  }
  else {
    if (selected == 1 && focusIntv > 0)
      focusIntv -= 1;
    else if (selected == 2 && restMin > 0)
      restMin -= 1;
    else if (selected == 3 && restSec > 0)
      restSec -= 1;
  }
}

void updateScreen() {
  if (isTimeSet) {
    isScreenOn = false;

    if (selected != 0) {
      //bottom line
      Display.drawLine(0, 63, 128, 63, SSD1306_WHITE);
      
      //time set message
      animate_timeSet();
      delay(500);
      
      selected = 0;
    }
    
    //turn the screen off
    screenOff();
  }
  else {
    showSetTime();
  }
}

void screenOff() {
  isScreenOn = false;
  Display.clearDisplay();
  Display.display();
  Display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void showSetTime() {
  isScreenOn = true;
  Display.ssd1306_command(SSD1306_DISPLAYON);
  if (timerMode) {
    //-----> title
    Display.setFont();
    Display.setCursor(38,7);
    Display.println((__FlashStringHelper*)str_focus);

    //-----> set digit font
    Display.setFont(&AGENCYR20pt7b);

    //-----> colon
    Display.setCursor(38,49);
    Display.println(":");
    Display.setCursor(84,49);
    Display.println(":");

    //-----> hour
    int hr1 = timerHr/10;
    int hr2 = timerHr%10;
    dispDigit(0, hr1);
    dispDigit(1, hr2);

    //-----> minute
    int min1 = timerMin/10;
    int min2 = timerMin%10;
    dispDigit(2, min1);
    dispDigit(3, min2);

    //-----> second
    int sec1 = timerSec/10;
    int sec2 = timerSec%10;
    dispDigit(4, sec1);
    dispDigit(5, sec2);

    //-----> selection bar
    Display.drawLine(0, 63, 128, 63, SSD1306_BLACK);
    switch (selected) {
      case 1:
        Display.drawLine(3, 63, 33, 63, SSD1306_WHITE);
        break;
      case 2:
        Display.drawLine(49, 63, 78, 63, SSD1306_WHITE);
        break;
      case 3:
        Display.drawLine(95, 63, 125, 63, SSD1306_WHITE);
        break;
      default:
        break;
    }

    Display.display();
  }
  else {
    //-----> title
    Display.setFont();
    Display.setCursor(20,7);
    Display.println((__FlashStringHelper*)str_intv);

    //-----> set digit font
    Display.setFont(&AGENCYR20pt7b);

    //-----> colon & brackets
    Display.setCursor(84,49);
    Display.println(":");
    Display.drawLine(0,40,5,24,SSD1306_WHITE);
    Display.drawLine(0,40,5,56,SSD1306_WHITE);
    Display.drawLine(36,40,31,24,SSD1306_WHITE);
    Display.drawLine(36,40,31,56,SSD1306_WHITE);

    //-----> interval
    dispDigit(6, focusIntv);

    //-----> minute
    int min1 = restMin/10;
    int min2 = restMin%10;
    dispDigit(2, min1);
    dispDigit(3, min2);

    //-----> second
    int sec1 = restSec/10;
    int sec2 = restSec%10;
    dispDigit(4, sec1);
    dispDigit(5, sec2);

    //-----> selection bar
    Display.drawLine(0, 63, 128, 63, SSD1306_BLACK);
    switch (selected) {
      case 1:
        Display.drawLine(3, 63, 33, 63, SSD1306_WHITE);
        break;
      case 2:
        Display.drawLine(49, 63, 78, 63, SSD1306_WHITE);
        break;
      case 3:
        Display.drawLine(95, 63, 125, 63, SSD1306_WHITE);
        break;
      default:
        break;
    }

    Display.display();
  }
}

void previewSetTime() {
  isScreenOn = true;
  Display.ssd1306_command(SSD1306_DISPLAYON);

  //set font
  Display.setTextSize(1);
  
  //-----> focus time
  //hour
  Display.setFont(&AGENCYR11pt7b);
  Display.setCursor(60,38);
  Display.print(timerHr/10);
  Display.print(timerHr%10);
  Display.setFont();
  Display.println(char_H);
  //min
  Display.setFont(&AGENCYR11pt7b);
  Display.setCursor(60,60);
  Display.print(timerMin/10);
  Display.print(timerMin%10);
  Display.setFont();
  Display.println(char_M);
  //sec
  Display.setFont(&AGENCYR11pt7b);
  Display.setCursor(94,60);
  Display.print(timerSec/10);
  Display.print(timerSec%10);
  Display.setFont();
  Display.println(char_S);

  //-----> no of interval
  Display.drawBitmap(8,7,bitmap_interval,30,34,SSD1306_WHITE);
  if (focusIntv == 0) {
    Display.setFont();
    Display.setCursor(15,22);
    Display.println((__FlashStringHelper*)str_off);
  }
  else if (focusIntv == 1) {
    Display.drawBitmap(19,17,letter_P11pt,7,17,SSD1306_WHITE);
  }
  else {
    Display.setFont(&AGENCYR11pt7b);
    Display.setCursor(19,33);
    Display.println(focusIntv);
  }

  //-----> rest Time
  Display.setFont();
  Display.setCursor(9,49);
  Display.print(restMin/10);
  Display.print(restMin%10);
  Display.print(":");
  Display.print(restSec/10);
  Display.print(restSec%10);

  //-----> draw rectangle
  Display.drawRoundRect(0,0,46,64,10,SSD1306_WHITE);

  Display.display();
}

void changeSelection() {
  if (selected != 0) {
    if (timerMode) {
      if (selected > 1)
        selected -= 1;
    }
    else {
      if (selected < 3)
        selected += 1;
    }
  }
}

void startTimer() {
  Serial.println("Timer started!");
  hasTimerStarted = true;
  selected = 0;

  focusMillis = timerHr*3600000UL + timerMin*60000UL + timerSec*1000UL;
  restMillis = restMin*60000UL+restSec*1000UL;
  Serial.print("focus = ");
  Serial.println(focusMillis);
  Serial.print("rest = ");
  Serial.println(restMillis);
  if (focusIntv > 1 || !isRestIntvSet) {
    repLeft = focusIntv - 1;
    noticeBeep();
    focusTimer();
  }
  else {
    repLeft = 1;
    noticeBeep();
    restTimer();
  }
}

void focusTimer() {
//  Serial.println("Focus Timer");
  isCounting = true;
  isResting = false;
  
  expTime = millis() + focusMillis;
  checkOverflow();
  Serial.println(focusMillis);
  Serial.println(expTime);

  ledOff();

  //move clock to 100%
}

void restTimer() {
//  Serial.println("Rest Timer");
  isCounting = true;
  isResting = true;
  repLeft -= 1;

  expTime = millis() + restMillis;
  checkOverflow();
  ledOn(purple);

  //move clock to 0%
}

void resumeTimer() {
  Serial.println("Timer resumed!");
  shortBeep();
  isCounting = true;
  
  expTime = millis() + timeLeft;
  checkOverflow();
  Serial.println(timeLeft);

  if (isResting)
    ledOn(purple);
  else
    ledOff();
}

void pauseTimer() {
  Serial.println("Timer paused!");
  shortBeep();
  isCounting = false;

  timeLeft = expTime - millis();

  ledOn(pink);
}

void stopTimer() {
  Serial.println("Timer stopped!");
  hasTimerStarted = false;
  isCounting = false;
  selected = 0;

  ledOff();

  //move clock to 0%
}

void tickTimer() {
  if (timerOverflow) {
    checkOverflow();
  }
  
  if (millis() >= expTime && !timerOverflow) {
    Serial.println("Times up!");
    if (isResting) {
      noticeBeep();
      focusTimer();
    }
    else {
      if (repLeft > 0 && isRestIntvSet) {
        noticeBeep();
        restTimer(); }
      else {
        startAlarm();
        stopTimer(); }
    }
  }
  else {
    int newTimerPercent;
    if (isResting){
      newTimerPercent = 100 - (expTime-millis())/(restMillis/100);
    }
    else {
      newTimerPercent = (expTime-millis())/(focusMillis/100);
    }
    
//    Serial.println((expTime-millis())/60000);
        
    if (newTimerPercent != timerPercent) {
      timerPercent = newTimerPercent;
      Serial.println(timerPercent);

      //move clock hand
    }
  }
}

void errorBeep() {
//  Serial.println("error");
  for (int i=0; i<2; i++) {
    tone(buzzer, 600);
    delay(160);
    noTone(buzzer);
    delay(50);
  }
}

void noticeBeep() {
  ledOn(pink);
  tone(buzzer, 900);
  delay(500);
  noTone(buzzer);
  ledOff();
}

void shortBeep() {
  tone(buzzer, 2000);
  delay(50);
  noTone(buzzer);
}

void checkAlarm() {
  if (alarmCount < 5) {
    if (millis()-alarmOnTime > pgm_read_byte(&alarmTone[alarmIndex]) + alarmCount*1000) {
      if (alarmIndex < 2)
        ledOn(red);
      else
        ledOff();
        
      if (alarmIndex != 3){
        tone(buzzer,2400,30);
        }
      if (alarmIndex < 3)
        alarmIndex += 1;
      else {
        alarmIndex = 0;
        alarmCount += 1;
      }
    }
  }
  else {
    isAlarmOn = false;
    alarmCount = 0;
  }
}

void startAlarm() {
  isAlarmOn = true;
  alarmOnTime = millis();
}

void ledOn(uint8_t rgbArray[]) {
  analogWrite(redPin, pgm_read_byte(&rgbArray[0]));
  analogWrite(greenPin, pgm_read_byte(&rgbArray[1]));
  analogWrite(bluePin, pgm_read_byte(&rgbArray[2]));
}

void ledOff() {
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void checkOverflow() {
  if (expTime > millis())
    timerOverflow = false;
  else
    timerOverflow = true;
}

void dispDigit(int index, int value) {
  Display.fillRect(pgm_read_byte(&DIGIT_X[index])+1, DIGIT_Y-30, DIGIT_WIDTH, DIGIT_HEIGHT, SSD1306_BLACK);
  
  if (index == 6 && value == 1) {
    Display.drawBitmap(pgm_read_byte(&DIGIT_X[index]), DIGIT_Y-30, letter_P24pt, DIGIT_WIDTH, DIGIT_HEIGHT, SSD1306_WHITE);
  }
  else {
    Display.setCursor(pgm_read_byte(&DIGIT_X[index]), DIGIT_Y);
    Display.println(value);
  }
}

void animate_timeSet() {
  Display.clearDisplay();
  Display.drawBitmap(49,17,gif_check_thin_frame_01,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_02,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_03,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_04,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_05,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_06,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_07,30,30,SSD1306_WHITE);
  Display.display();
  delay(7);
  Display.drawBitmap(49,17,gif_check_thin_frame_08,30,30,SSD1306_WHITE);
  Display.display();
}
