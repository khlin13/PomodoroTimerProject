#define redPin 9
#define greenPin 10
#define bluePin 11

int blue[] = {10, 80, 80};
int pink[] = {150, 10, 5};
int purple[] = {80, 0, 100};
int red[] = {150, 0, 0};

void setup() {
  // put your setup code here, to run once:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);

  ledOn(pink);
}

void loop() {
  
}

void ledOff() {
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void ledOn(int rgbArray[]) {
  analogWrite(redPin, rgbArray[0]);
  analogWrite(greenPin, rgbArray[1]);
  analogWrite(bluePin, rgbArray[2]);
}
