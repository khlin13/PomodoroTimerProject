int btn1 = 2;
int btn2 = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(btn1));
  Serial.print("  ");
  Serial.println(digitalRead(btn2));
}
