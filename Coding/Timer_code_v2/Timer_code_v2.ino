

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

unsigned long debounceDelay = 50;
unsigned long lastDebounceTime[3] = {0, 0, 0};

//button click
unsigned long clickDelay = 50;
unsigned long lastClickedTime[3] = {0, 0, 0};
unsigned long lastReleasedTime[3] = {0, 0, 0};

//button long press
const int longPrsDelay = 1000;
const int incDelay = 800;
int incDelayCounter = 0;

//timer variables
const int autoSetDelay = 2500;
const int minTimerSec = 10;

int timerHr = 0;
int timerMin = 0;
int timerSec = 0;
int focusIntv = 2;
int restMin  = 0;
int restSec = 0;

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
int repLeft = 0;
boolean isResting = false;
boolean isRestIntvSet = false;

int timerPercent = 0;

//alarm tone
int alarmTone[] = {0, 70, 140, 1000};
int alarmIndex = 0;
int alarmCount = 0;
unsigned long alarmOnTime = 0;
boolean isAlarmOn = false;

//colors
const int blue[] = {10, 80, 80};
const int pink[] = {150, 10, 5};
const int purple[] = {40, 0, 50};
const int red[] = {150, 0, 0};

void setup() {
  pinMode(btnL, INPUT);
  pinMode(btnR, INPUT);
  pinMode(btnTop, INPUT);
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);
  pinMode(motorEn, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);

  isScreenOn = false;
}

void loop() {
  //run timer
  if (isCounting)
    tickTimer();

  if (isAlarmOn)
    checkAlarm();
  else
    noTone(buzzer);
  
  //Single-press inputs
  boolean btnL_pressed = isBtnClicked(btnL);
  boolean btnR_pressed = isBtnClicked(btnR);
  boolean btnTop_pressed = isBtnClicked(btnTop);
  boolean LnR_pressed = false;

  //checking how long the buttons has been pressed
  long btnL_dur = millis()-lastClickedTime[0];
  long btnR_dur = millis()-lastClickedTime[1];
  long btnTop_dur = millis()-lastClickedTime[2];

  //checking how long the buttons has been released
//  long btnL_durRel = millis()-lastReleasedTime[0];
//  long btnR_durRel = millis()-lastReleasedTime[1];

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
        }
        else {
          focusIntv = 0;
          restMin  = 0;
          restSec = 0;
        }
        selected = 0;
  
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
      else if (selected == 2 && !isTimeSet) {
        //wait 3s and exit if top button has been pressed once
        
        checkBtnClick();
      }
      else {
        incDelayCounter = 0;
      }
    }
    else { //timer has started
      if (btnTop_pressed) {
        pauseTimer();
      }
      else {
        checkBtnClick();
      }
    }
    
    
  }

  //-------------------------------------------------
  else { //screen off
//    Serial.println("Screen Off");

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
      if (btnL_pressed) {
        timerMode = false;
        showSetTime();
      }
      else if (btnR_pressed) {
        timerMode = true;
        showSetTime();
      }
    }
    else {
      if (btnL_pressed) {
        //set interval mode
        Serial.println("Left button is pressed!");
  
        timerMode = false;
        isTimeSet = false;
        updateScreen();
      }
      else if (btnR_pressed) {
        //set focus timer
        Serial.println("Right button is pressed!");
  
        timerMode = true;
        isTimeSet = false;
        updateScreen();
      }
      else if (LnR_pressed) {
        //check history
        Serial.println("Both left and right buttons are pressed!");
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
  unsigned long curTime = millis();
  boolean timesUp = (curTime-lastReleasedTime[0] > autoSetDelay) && 
                    (curTime-lastReleasedTime[1] >autoSetDelay) && 
                    (curTime-lastReleasedTime[2] > autoSetDelay) &&
                    (curTime-lastClickedTime[0] > autoSetDelay) && 
                    (curTime-lastClickedTime[1] > autoSetDelay) && 
                    (curTime-lastClickedTime[2] > autoSetDelay);
  
  if (timesUp) {
    isTimeSet = true;
    selected = 0;
    if (focusIntv > 0 && (restMin > 0 || restSec > 0) ) {
      isRestIntvSet = true;
    }
    else {
      isRestIntvSet = false;
    }
    updateScreen();
    Serial.println("Time Set!");
  }
}

void increaseDigit() {
  if (timerMode) {
    if (selected == 0 && timerSec < 59)
      timerSec += 1;
    else if (selected == 1 && timerMin < 59)
      timerMin += 1;
    else if (selected == 2 && timerHr < 23)
      timerHr += 1;
  }
  else {
    if (selected == 0 && focusIntv < 9)
      focusIntv += 1;
    else if (selected == 1 && restMin < 99)
      restMin += 1;
    else if (selected == 2 && restSec < 99)
      restSec += 1;
  }
}

void decreaseDigit() {
  if (timerMode) {
    if (selected == 0 && timerSec > 0)
      timerSec -= 1;
    else if (selected == 1 && timerMin > 0)
      timerMin -= 1;
    else if (selected == 2 && timerHr > 0)
      timerHr -= 1;
  }
  else {
    if (selected == 0 && focusIntv > 1)
      focusIntv -= 1;
    else if (selected == 1 && restMin > 0)
      restMin -= 1;
    else if (selected == 2 && restSec > 0)
      restSec -= 1;
  }
}

void updateScreen() {
  if (isTimeSet) {
    isScreenOn = false;
  }
  else {
    showSetTime();
  }
}

void screenOff() {
  
}

void showSetTime() {
  isScreenOn = true;
  if (timerMode) {
    Serial.print(timerHr);
    Serial.print(" : ");
    Serial.print(timerMin);
    Serial.print(" : ");
    Serial.println(timerSec);
  }
  else {
    Serial.print(focusIntv);
    Serial.print("   ");
    Serial.print(restMin);
    Serial.print(" : ");
    Serial.println(restSec);
  }
}

void changeSelection() {
  if (selected < 2)
    selected += 1;
}

void startTimer() {
  Serial.println("Timer started!");
  hasTimerStarted = true;
  selected = 2;

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
  Serial.println("Focus Timer");
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
  Serial.println("Rest Timer");
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
        restTimer(); 
        Serial.println("222222222");}
      else {
        startAlarm();
        stopTimer(); 
        Serial.println("333333333");}
    }
  }
  else {
    long millisIn1per, newTimerPercent;
    if (isResting){
      millisIn1per = restMillis/100;
      newTimerPercent = 100 - (expTime-millis())/millisIn1per;
    }
    else {
      millisIn1per = focusMillis/100;
      newTimerPercent = (expTime-millis())/millisIn1per;
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
  Serial.println("error");
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
    if (millis()-alarmOnTime > alarmTone[alarmIndex] + alarmCount*1000) {
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

void ledOn(int rgbArray[]) {
  analogWrite(redPin, rgbArray[0]);
  analogWrite(greenPin, rgbArray[1]);
  analogWrite(bluePin, rgbArray[2]);
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
