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


unsigned long time_start_ms;
unsigned long time_now_s;

int timerAlarm = 0;

#define COLORED 0
#define UNCOLORED 1
#define ONE_WIRE_BUS 2

//ALARM THRESHOLDS
#define BL_BOUND 15
#define T_LOW 75
#define T_HIGH 80
#define TIME_BOUND 20


// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/********************************************************************/

unsigned long startMillis;
unsigned long currentMillis;

void setup() {
  sensors.begin();
  // put your setup code here, to run once:
  Serial.begin(115200);
  startMillis = millis();
  currentMillis = millis();
}

void loop() {
  //Serial.println(currentMillis - startMillis);
  
  // Get battery life
  int analog_value = analogRead(A0);
  float voltage = analog_value * (5.0 / 1023.0);
  int bl_percent = voltage/5 * 100;
  // format battery life
  char battery_life_text[] = "Battery: ";
  char battery_life_value[10];
  itoa(bl_percent, battery_life_value, 10);
  char * battery_life_string = new char[strlen(battery_life_text)+strlen(battery_life_value)+1];
  strcpy(battery_life_string,battery_life_text);
  strcat(battery_life_string,battery_life_value);

  // Get temperature 
  sensors.requestTemperatures();  // Send the command to get temperature readings
  int val = sensors.getTempFByIndex(0);
  // Format temperature
  char temp_text[] = "Temp: ";
  char temp_value[10];
  itoa(val, temp_value, 10);
  char * temp_string = new char[strlen(temp_text)+strlen(temp_value)+1];
  strcpy(temp_string,temp_text);
  strcat(temp_string,temp_value);


  // Get time
  char time_text[] = "Time: ";
  char time_value[10];
  itoa(timerAlarm, time_value, 10);
  char * time_string = new char[strlen(time_text)+strlen(time_value)+1];
  strcpy(time_string,time_text);
  strcat(time_string,time_value);

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
  if (timerAlarm >= TIME_BOUND) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 133, 0, paint.GetWidth(), paint.GetHeight());
  }

  epd.DisplayFrame();
  delay(10000);

  epd.HDirInit();
  //epd.Clear();
  epd.Sleep();
  currentMillis = millis();
}
















