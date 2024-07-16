#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Fonts/swisop124pt7b.h>
#include <Fonts/AGENCYR20pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

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
