#define redPin 9
#define greenPin 10
#define bluePin 11

void setup() {
  // put your setup code here, to run once:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(A0, INPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float total = 0;
  for (int ii=0; ii<10; ii++) {
    total += analogRead(A0);
  }
  float average = total/10;
//  Serial.print(analogRead(A0));
//  Serial.print("    ");
  Serial.println(average);

  if (average > 975) {
    Serial.println("1");
    ledOn(0,255,0);
  }
  else if (average > 870) {
    Serial.println("2");
    ledOn(150,150,0);
  }
  else if (average > 800) {
    Serial.println("3");
    ledOn(150,50,100);
  }
  else {
    Serial.println("4");
    ledOn(200,0,0); 
  }
}

void ledOff() {
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void ledOn(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
