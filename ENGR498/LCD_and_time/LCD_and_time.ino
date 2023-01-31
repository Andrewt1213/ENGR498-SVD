/*
Print time Elapsed on LCD
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

unsigned long startTime;
unsigned long endTime;
unsigned long duration;

int state = 0;
int buttonState = 0; 

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("TMR:"); 
}

void loop() {
  // TODO: Fix rollover
  buttonStuff();
  // Prints current count as it goes
  if (state == 1 || state == 2) {
    lcd.setCursor(5, 0);
    formatTime((millis() - startTime));
  }
  lcd.setCursor(0, 1);
  lcd.print("TMP:");
  //formatTime((millis() - startTime));
}

unsigned long buttonStuff() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // S0 Begin timer when button first pressed and no timerRunning yet
  if (state == 0 && buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
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
    digitalWrite(ledPin, LOW);
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
  unsigned long day = floor(milliseconds/86400000);
  unsigned long dayR = milliseconds%86400000;

  unsigned long hour = floor(dayR/3600000);
  unsigned long hourR = dayR%3600000;

  unsigned long min = floor(hourR/60000);
  unsigned long minR = hourR%60000;

  unsigned long sec = floor(minR/1000);
  //clearLCD(0, 1);
  
  // print the number of seconds since reset:
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

