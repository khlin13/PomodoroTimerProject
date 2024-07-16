#include <Adafruit_SSD1306.h>
#include <splash.h>

#define potPin A1

int btn1 = 2;
int btn2 = 3;
int motorA = 4;
int motorB = 5;
int motorEn = 6;

int potPos = 0;
int Kp = 10;
int Ki = 0;
int Kd = 10;
int lastErr = 0;
int intgErr = 0;

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
    moveMotor(100);
  }
  else if (btn2_sts == 1) {
    moveMotor(0);
  }
  else {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);
  }

  
}

void moveMotor(int percent) {
  int curPos = analogRead(potPin);
  Serial.println(curPos);

  int targetPos = map(percent,0,100,0,1023);
  int curErr = targetPos - curPos;
  int pidVal = Kp*curErr + Ki*intgErr + Kd*(curErr-lastErr);

  int spd = min(pidVal, 1000);
  if (spd > 0) {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
  }
  else {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
  }

  int reducer = map(spd,0,1000,40,0);
  delay(15);
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);
  delay(10+reducer);

  if (curErr*lastErr <= 0) {
    intgErr = 0;
  }
  else {
    intgErr += curErr;
  }
  lastErr = curErr;
}

void rotate(int spd) {
  
}
