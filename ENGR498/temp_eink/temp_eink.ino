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

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/********************************************************************/

void setup() {
  sensors.begin();
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("e-Paper init and clear");
}

void loop() {
  // fromat temp
  char temperature_val[10];
  sensors.requestTemperatures();  // Send the command to get temperature readings
  int val = sensors.getTempFByIndex(0);
  itoa(val, temperature_val, 10);

  // format time
  char timer_string[10];
  itoa(timerAlarm, timer_string, 10);

  epd.LDirInit();
  epd.Clear();

  paint.SetWidth(200);
  paint.SetHeight(40);
  // Temp box
  Serial.println("e-Paper paint");
  paint.Clear(COLORED);
  paint.DrawStringAt(15, 14, "Temperature:", &Font20, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  // Temp Value
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 14, temperature_val, &Font24, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 50, paint.GetWidth(), paint.GetHeight());

  // Timer box
  paint.Clear(COLORED);
  paint.DrawStringAt(15, 14, "Time:", &Font20, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());

  // Timer Value
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 14, timer_string, &Font20, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 150, paint.GetWidth(), paint.GetHeight());

  // Alarms
  paint.SetWidth(50);
  paint.SetHeight(50);

  // Temp Alarm
  if (val >= 80) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    //paint.DrawLine(0, 0, 50, 50, COLORED);
    //paint.DrawLine(50, 0, 0, 50, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 150, 45, paint.GetWidth(), paint.GetHeight());
  } else {
    //paint.Clear(UNCOLORED);
    //paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    //epd.SetFrameMemory(paint.GetImage(), 150, 45, paint.GetWidth(), paint.GetHeight());
  }

  // Time alarm
  if (timerAlarm >= 20) {
    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 50, 50, COLORED);
    //paint.DrawLine(0, 0, 50, 50, COLORED);
    //paint.DrawLine(50, 0, 0, 50, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 150, 145, paint.GetWidth(), paint.GetHeight());
  }


  epd.DisplayFrame();
  delay(10000);
  timerAlarm += 10;

  Serial.println("e-Paper clear and goto sleep");
  epd.HDirInit();
  //epd.Clear();
  epd.Sleep();
}
