#include <Adafruit_SSD1306.h>
#include <splash.h>

#define potPin A1

int btn1 = 2;
int btn2 = 3;
int motorA = 4;
int motorB = 5;
int motorEn = 6;
int buzzer = 7;

int potPos = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(motorEn, OUTPUT);
  pinMode(potPin, INPUT);
  Serial.begin(9600);

  analogWrite(motorEn, 255);
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);
  potPos = analogRead(potPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  int btn1_sts = digitalRead(btn1);
  int btn2_sts = digitalRead(btn2);
  
//  Serial.print(btn1_sts);
//  Serial.print("  ");
//  Serial.println(btn2_sts);

  if (btn1_sts == 1) {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
    Serial.println("btn1");
//    delay(35);
//    digitalWrite(motorA, LOW);
//    digitalWrite(motorB, LOW);
//    delay(10);
  }
  else if (btn2_sts == 1) {
    digitalWrite(motorA, HIGH);
    digitalWrite(motorB, LOW);
    Serial.println("btn2");
//    delay(15);
//    digitalWrite(motorA, LOW);
//    digitalWrite(motorB, LOW);
//    delay(10);
  }
  else {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);
  }

  potPos = analogRead(potPin);
//  Serial.println(potPos);
}

void moveMotor(int deg) {
  
}
