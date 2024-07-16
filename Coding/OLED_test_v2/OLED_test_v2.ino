#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/AGENCYR20pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DIGIT_WIDTH 14
#define DIGIT_HEIGHT 32

const int DIGIT_X[] PROGMEM = {0, 20, 46, 66, 92, 112};
const int DIGIT_Y PROGMEM = 56;
const int COLON_X[] PROGMEM = {38,84};
const int COLON_Y PROGMEM = 49;

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

int debounceDelay = 50;
unsigned long lastDebounceTime[3] = {0, 0, 0};

//button click
int clickDelay = 50;
unsigned long lastClickedTime[3] = {0, 0, 0};
unsigned long lastReleasedTime[3] = {0, 0, 0};

//button long press
const int longPrsDelay PROGMEM = 1000;
const int incDelay PROGMEM = 800;
int incDelayCounter = 0;

//timer variables
const int autoSetDelay PROGMEM = 2500;
const int minTimerSec PROGMEM = 10;

byte timerHr = 0;
byte timerMin = 0;
byte timerSec = 0;
byte focusIntv = 2;
byte restMin  = 0;
byte restSec = 0;

byte selected = 0;
boolean isScreenOn = false;
boolean timerMode = false; //false->interval, true->timer
boolean isTimeSet = false;
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
int alarmTone[] = {0, 70, 140, 1000};
byte alarmIndex = 0;
byte alarmCount = 0;
unsigned long alarmOnTime = 0;
boolean isAlarmOn = false;

//colors
const int blue[] PROGMEM = {10, 80, 80};
const int pink[] PROGMEM = {150, 10, 5};
const int purple[] PROGMEM = {40, 0, 50};
const int red[] PROGMEM = {150, 0, 0};


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if(!Display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  Display.clearDisplay();
  Display.display();
  delay(500);

  //---------> title
  Display.setFont();
  Display.setTextSize(1);
  Display.setTextColor(WHITE, BLACK);
  Display.setCursor(50,7);
  Display.println("Focus");
  Display.display();

  //----------> set font
  Display.setFont(&AGENCYR20pt7b);
  Display.setTextSize(1);
  Display.setTextColor(WHITE, BLACK);

  //---------> colon
  Display.setCursor(38,49);
  Display.println(":");
  Display.setCursor(84,49);
  Display.println(":");
  Display.display();

  Display.setCursor(0, 56);
  Display.println("1");
  Display.setCursor(20, 56);
  Display.println("2");
  Display.display();
  
/*
//---------------> hour
  for (int i=1; i<10; i++) {
    Display.fillRect(1,26,14,32,SSD1306_BLACK);
    
    Display.setCursor(0, 56);
    Display.println(i);
    Display.display(); 
    delay(100);
  }

  for (int i=0; i<10; i++) {
    Display.fillRect(21,26,14,32,SSD1306_BLACK);
  
    Display.setCursor(20, 56);
    Display.println(i);
    Display.display(); 
    delay(100);
  }

//--------------> minute
  for (int i=0; i<10; i++) {
    Display.fillRect(47,26,14,32,SSD1306_BLACK);
    Display.setCursor(46, 56);
    Display.println(i);
    Display.display(); 
    delay(100);
  }

  for (int i=0; i<10; i++) {
    Display.fillRect(67,26,14,32,SSD1306_BLACK);
    Display.setCursor(66, 56);
    Display.println(i);
    Display.display(); 
    delay(100);
  }

//-----------------> second
  for (int i=0; i<10; i++) {
    Display.fillRect(93,26,14,32,SSD1306_BLACK);
    Display.setCursor(92, 56);
    Display.println(i);
    Display.display(); 
    delay(100);
  }

  for (int i=0; i<10; i++) {
    Display.fillRect(113,26,14,32,SSD1306_BLACK);
    Display.setCursor(112, 56);
    Display.println(i);
    Display.display(); 
    delay(100);
  }
  Display.ssd1306_command(SSD1306_DISPLAYOFF);
  delay(1000);
  Display.ssd1306_command(SSD1306_DISPLAYON);*/
}

void loop() {
  
}
