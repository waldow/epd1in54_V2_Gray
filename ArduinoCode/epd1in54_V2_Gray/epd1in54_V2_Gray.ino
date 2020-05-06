/* orignally https://github.com/waveshare/e-Paper/tree/master/Arduino/epd1in54_V2
 *  with functionally of https://github.com/waveshare/e-Paper/tree/master/Arduino/epd1in54
 *  Added gray 4 color support (light gray,dark gray,black and white)
 */

#include <SPI.h>
#include "epd1in54_V2.h"
#include "epdpaint.h"
#include "imagedata.h"
#include "imagedataflower.h"

#define COLORED     0
#define UNCOLORED   1

/**
  * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
  * In this case, a smaller image buffer is allocated and you have to 
  * update a partial display several times.
  * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;
unsigned long time_start_ms;
unsigned long time_now_s;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (epd.Init() != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrameFull();

  paint.SetRotate(ROTATE_0);
  paint.SetWidth(200);
  paint.SetHeight(24);

  /* For simplicity, the arguments are explicit numerical coordinates */
  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Hello world!", &Font16, UNCOLORED);
  Serial.println("e-Paper SetFrameMemory");
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
    
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(30, 4, "e-Paper Demo", &Font16, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 30, paint.GetWidth(), paint.GetHeight());

  paint.SetWidth(64);
  paint.SetHeight(64);
  
  paint.Clear(UNCOLORED);
  paint.DrawRectangle(0, 0, 40, 50, COLORED);
  paint.DrawLine(0, 0, 40, 50, COLORED);
  paint.DrawLine(40, 0, 0, 50, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 16, 60, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawCircle(32, 32, 30, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 120, 60, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawFilledRectangle(0, 0, 40, 50, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 16, 130, paint.GetWidth(), paint.GetHeight());

  paint.Clear(UNCOLORED);
  paint.DrawFilledCircle(32, 32, 30, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 120, 130, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrameFull();

  delay(5000);
  epd.SetFrameMemory(IMAGE_DATA);
  epd.DisplayFrameFull();
  
  delay(5000);
   epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrameFull();
  delay(5000);

    epd.SetBox(00,00, 200, 200, 0x24,0x00); //default dark gray
  //epd.SetFrameMemory(IMAGE_DATA_FLOWER_DARK,0x24,false); // can load dark gray if needed
  epd.DisplayFrameFast();
  delay(1000);
  epd.SetFrameMemory(IMAGE_DATA_FLOWER_LIGHT,0x24,false);
  epd.DisplayFrameFast();
  delay(5000);
  epd.SetFrameMemory(IMAGE_DATA_FLOWER_WHITE,0x24,false); // 0=gray 1 = white  //f1
  epd.SetFrameMemory(IMAGE_DATA_FLOWER_BLACK,0x26,false);  //1= black
  epd.DisplayFrameFast();
  
 time_start_ms = millis();
 epd.Sleep();
  
}

void loop() {

  /*
   if (epd.Init() != 0) {
      Serial.print("e-Paper init failed");
     
  }else
  {
  // put your main code here, to run repeatedly:
  time_now_s = (millis() - time_start_ms) / 1000;
  char time_string[] = {'0', '0', ':', '0', '0', '\0'};
  time_string[0] = time_now_s / 60 / 10 + '0';
  time_string[1] = time_now_s / 60 % 10 + '0';
  time_string[3] = time_now_s % 60 / 10 + '0';
  time_string[4] = time_now_s % 60 % 10 + '0';

  paint.SetWidth(32);
  paint.SetHeight(96);
  paint.SetRotate(ROTATE_270);

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(0, 4, time_string, &Font24, COLORED);
  epd.SetFrameMemory(paint.GetImage(), 80, 72, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrameFast();
  epd.Sleep();
  
  }
  delay(500);
  */
}
