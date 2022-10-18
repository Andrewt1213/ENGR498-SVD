/*
Print time Elapsed after button press and temp on LCD 
*/

// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"

// Temperature initialize
#define DHTPIN 8 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

const int timeAlarmLedPin = 9;
const int tempAlarmLedPin = 10;

unsigned long startTime;
unsigned long endTime;
unsigned long duration;

// timer LED will turn on after this duration
unsigned long DURATION_ALARM = 5; // 5 seconds = 5 ms
float TEMPERATURE_ALARM_MAX = 82.00; // In degree F
float TEMPERATURE_ALARM_MIN = 78.00; // In degree F

// Time fomating variables
unsigned long day;
unsigned long dayR;
unsigned long hour;
unsigned long hourR;
unsigned long min;
unsigned long minR;
unsigned long sec;

// checkButton Variables
int state = 0;
int buttonState = 0; 

// x will keep track of when to take temp
int x = 0;
float temp = 0.0;

void setup() {
  //Serial.begin(9600);
  dht.begin();
  // initialize the LED pins as an output:
  pinMode(timeAlarmLedPin, OUTPUT);
  pinMode(tempAlarmLedPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("TMR:"); 
  
}

void loop() {
  // TODO: Fix rollover
  timerCSC();
  // read temp every 2 seconds
  temperatureCSC();
}

unsigned long checkButton() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // S0 Begin timer when button first pressed and no timerRunning yet
  if (state == 0 && buttonState == HIGH) {
    startTime = millis();
    clearLCD(5,0);
    state = 1;
  } 
  // button unpressed, waits for next press to stop
  if (state == 1 && buttonState == LOW) {
    state = 2;
  }
  // button pressed again, stops timer and displays
  if (state == 2 && buttonState == HIGH) {
    endTime = millis();
    duration = endTime - startTime;
    clearLCD(5, 0);
    lcd.setCursor(5, 0);
    formatTime(duration);
    state = 3;
  }
  // unpress and wait for next button press. ie. restarts timer
  if (state == 3 && buttonState == LOW) {
    state = 0;
  }
}

void clearLCD(int col, int row) {
  lcd.setCursor(col, row);
  lcd.print("                ");
  
}

void formatTime(unsigned long milliseconds) {
  // TODO: Change so these variables are updated instead of set
  day = floor(milliseconds/86400000);
  dayR = milliseconds%86400000;

  hour = floor(dayR/3600000);
  hourR = dayR%3600000;

  min = floor(hourR/60000);
  minR = hourR%60000;

  sec = floor(minR/1000);
  
  // print the time elapsed between button presses:
  lcd.print(day);
  lcd.print("d");
  lcd.print(hour);
  lcd.print("h");
  lcd.print(min);
  lcd.print("m");
  lcd.print(sec);
  lcd.print("s");
  lcd.print("        ");

}

void timerCSC() {
  checkButton();
  // Prints current count as it goes
  if (state == 1 || state == 2) {
    lcd.setCursor(5, 0);
    formatTime((millis() - startTime));
    //turn LED on if time past threshold
    if (sec >= DURATION_ALARM) {
      digitalWrite(timeAlarmLedPin, HIGH);//turn LED on if time past threshold
    } else {
      digitalWrite(timeAlarmLedPin, LOW);
    }
  }
}

void temperatureCSC(){
  lcd.setCursor(0, 1);
  lcd.print("TMP: ");
  if (x < 100) {
    x = x + 1;
  }
  else {
    temp = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(temp)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    lcd.print(temp);
    lcd.print(" F");
    if (temp >= TEMPERATURE_ALARM_MAX || temp <= TEMPERATURE_ALARM_MIN) {
      digitalWrite(tempAlarmLedPin, HIGH);//turn LED on if time past threshold
    } else {
      digitalWrite(tempAlarmLedPin, LOW);
    }
    x = 0;
  }
}
