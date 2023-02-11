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
  epd.LDirInit();
  epd.Clear();

  paint.SetWidth(200);
  paint.SetHeight(40);
  // Battery life box  
  paint.Clear(COLORED);
  paint.DrawStringAt(15, 14, "Rotate:", &Font20, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  // Battery life value
  
  epd.DisplayFrame();
  delay(10000);

  Serial.println("e-Paper clear and goto sleep");
  epd.HDirInit();
  //epd.Clear();
  epd.Sleep();
}
