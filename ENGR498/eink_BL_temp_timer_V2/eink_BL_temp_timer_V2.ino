#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <stdio.h>
#include <OneWire.h>
#include <DallasTemperature.h>

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);

#define COLORED 0
#define UNCOLORED 1
#define ONE_WIRE_BUS 2

//ALARM THRESHOLDS
#define BL_BOUND 15   // Battery life alarm 
#define T_LOW 75      // Temp alarm low end
#define T_HIGH 80     // Temp alarm high end
#define TIME_BOUND 30 // Timer alarm


// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/********************************************************************/
// Time trackers
unsigned long startTime;
unsigned long elapsedTime = 0;
unsigned long savedTime = elapsedTime;

// constants won't change. They're used here to set pin numbers:
//TODO: Change to defines
const int buttonPin = 3;  // the number of the pushbutton pin
const int ledPin = 4;    // the number of the LED pin
const int sleepTime = 20; 

// Variables will change:
// Button
int ledState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin
// Timer
int state = 0;
unsigned long timer = 0;
unsigned long timerElapsedTime = 0;
int countTime = 0;

// the following variables are unsigned longs because the time, measured in milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

long display_counter = 100000;
long sleep_counter = 0;
long count_sleep = 0;

char battery_life_text[] = "Battery: ";
char battery_life_value[10];
char temp_text[] = "Temp: ";
char temp_value[10];
char time_text[] = "Time: ";
char time_value[10];

char * battery_life_string = new char[strlen(battery_life_text)+strlen(battery_life_value)+1];
char * temp_string = new char[strlen(temp_text)+strlen(temp_value)+1];
char * time_string = new char[strlen(time_text)+strlen(time_value)+1];

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  sensors.begin();
  Serial.begin(115200);
  display();
}

// Note takes 8 seconds arduino time to refresh screen
void loop() {
  printElapsedTime();  
  checkButton(); // Starts or stops timer. 

  if (elapsedTime % 20 == 0){ //change to sleepTime which = whatever we want
    display();
  }
}

void display(){
  // Get battery life
  int analog_value = analogRead(A0);
  float voltage = analog_value * (5.0 / 1023.0);
  int bl_percent = voltage/5 * 100;
  // format battery life
  itoa(bl_percent, battery_life_value, 10);
  strcpy(battery_life_string,battery_life_text);
  strcat(battery_life_string,battery_life_value);

  // Get temperature 
  sensors.requestTemperatures();  // Send the command to get temperature readings
  int val = sensors.getTempFByIndex(0);
  // Format temperature
  itoa(val, temp_value, 10);
  strcpy(temp_string,temp_text);
  strcat(temp_string,temp_value);

  // Get time
  itoa(timerElapsedTime, time_value, 10);
  strcpy(time_string,time_text);
  strcat(time_string,time_value);

  // Display Clearing
  epd.LDirInit();
  epd.Clear();

  //Display formatting
  paint.SetRotate(ROTATE_270);
  paint.SetWidth(40);
  paint.SetHeight(200);

  // Battery life box
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(15, 14, battery_life_string, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());

  // Temp life box
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(15, 14, temp_string, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 66, 0, paint.GetWidth(), paint.GetHeight());

  // Time box
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(15, 14, time_string, &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 133, 0, paint.GetWidth(), paint.GetHeight());
  
  //Alarms
  paint.SetWidth(50);
  paint.SetHeight(50);
  
  // BL alarm
  if (bl_percent <= BL_BOUND) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  }

  // Temp alarm
  if (val >= T_HIGH || val <= T_LOW) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 66, 0, paint.GetWidth(), paint.GetHeight());
  }

  // Time Alarm
  if (timerElapsedTime >= TIME_BOUND) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 133, 0, paint.GetWidth(), paint.GetHeight());
  }

  epd.DisplayFrame();
  epd.Sleep();
}

void checkButton(){
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  // Starts timer or not based off reading
  if (state == 0 && ledState == LOW) {    
    Serial.println("Timer Started!");
    timer = millis();    
    state = 1;
    display();
  } 
  // button unpressed, waits for next press to stop
  if (state == 1 && ledState == LOW) {
    state = 2;
  }
  // button pressed again, stops timer and displays
  if (state == 2 && ledState == HIGH) {
    Serial.println("Timer Stopped!");
    timer = 0;
    timerElapsedTime = 0;
    state = 3;
    display();
  }
  // unpress and wait for next button press. ie. restarts timer
  if (state == 3 && ledState == HIGH) {
    state = 0;
  }
}

void printElapsedTime(){
  elapsedTime = (millis() - startTime) / 1000; // in seconds
  if (savedTime != elapsedTime){
    // timer on, count time
    if (state == 2){
      timerElapsedTime = savedTime - (timer/1000);
      Serial.print(timerElapsedTime);
    }
    //Serial.print(savedTime);
    Serial.print(" ");
    Serial.print(millis());
    Serial.println();
    savedTime = elapsedTime;
    
  }
}