#include <SPI.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <stdio.h>

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);


unsigned long time_start_ms;
unsigned long time_now_s;
#define COLORED     0
#define UNCOLORED   1

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("e-Paper init and clear");
  epd.LDirInit();
  epd.Clear();

  paint.SetWidth(200);
  paint.SetHeight(40);

  Serial.println("e-Paper paint");
  paint.Clear(COLORED);
  paint.DrawStringAt(15, 14, "Temperature:", &Font20, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 14, "70C", &Font24, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 50, paint.GetWidth(), paint.GetHeight());

  paint.Clear(COLORED);
  paint.DrawStringAt(15, 14, "Time:", &Font20, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 100, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 14, "11 minutes", &Font20, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 150, paint.GetWidth(), paint.GetHeight());


  epd.DisplayFrame();
  delay(30000);


  Serial.println("e-Paper clear and goto sleep");
  epd.HDirInit();
  //epd.Clear();
  epd.Sleep();
}

void loop()
{

}
