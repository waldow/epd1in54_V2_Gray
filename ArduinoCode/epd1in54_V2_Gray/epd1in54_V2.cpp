/* orignally https://github.com/waveshare/e-Paper/tree/master/Arduino/epd1in54_V2
 *  with functionally of https://github.com/waveshare/e-Paper/tree/master/Arduino/epd1in54
 *  Added gray 4 color support (light gray,dark gray,black and white)
 */
 
/*****************************************************************************
* | File      	:   epd1in54_V2.cpp
* | Author      :   Waveshare team
* | Function    :   1.54inch e-paper V2
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-24
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include <stdlib.h>
#include "epd1in54_V2.h"

Epd::~Epd()
{
};

Epd::Epd()
{
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command)
{
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data)
{
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void)
{
    while(DigitalRead(busy_pin) == 1) {      //LOW: idle, HIGH: busy
        DelayMs(5);
         //Serial.print("e-Paper WaitUntilIdle");
    }
    //DelayMs(200);
}

int Epd::Init(void)
{
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0) {
        return -1;
    }
    /* EPD hardware init start */
    Reset();

    WaitUntilIdle();
    SendCommand(0x12);  //SWRESET
    WaitUntilIdle();

    SendCommand(0x01); //Driver output control
    SendData(0xC7);
    SendData(0x00);
    //SendData(0x01);
    SendData(0x0); //change direction was 1 
      
    SendCommand(0x11); //data entry mode
    SendData(0x01);

    SendCommand(0x44); //set Ram-X address start/end position
    SendData(0x00);
    SendData(0x18);    //0x0C-->(18+1)*8=200

    SendCommand(0x45); //set Ram-Y address start/end position
    SendData(0xC7);   //0xC7-->(199+1)=200
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);

    SendCommand(0x3C); //BorderWavefrom
    SendData(0x01);

    SendCommand(0x18);
    SendData(0x80);

    SendCommand(0x22); // //Load Temperature and waveform setting.
    SendData(0XB1);
    SendCommand(0x20);

    SendCommand(0x4E);   // set RAM x address count to 0;
    SendData(0x00);
    SendCommand(0x4F);   // set RAM y address count to 0X199;
    SendData(0xC7);
    SendData(0x00);
    WaitUntilIdle();
    /* EPD hardware init end */

    return 0;
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void)
{
   DigitalWrite(reset_pin, LOW);                //module reset    
    DelayMs(5);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(5);

    return;
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
    DigitalWrite(reset_pin, LOW);                //module reset
    DelayMs(10);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
}

void Epd::Clear(void)
{
    int w, h;
    w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    h = EPD_HEIGHT;
    SendCommand(0x24);
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            SendData(0xff);
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xF7);
    SendCommand(0x20);
    WaitUntilIdle();
}

void Epd::Display(const unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(pgm_read_byte(&frame_buffer[i + j * w]));
            }
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xF7);
    SendCommand(0x20);
    WaitUntilIdle();
}

void Epd::DisplayPartBaseImage(const unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }

        SendCommand(0x26);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xFF);
    SendCommand(0x20);
    WaitUntilIdle();
}

void Epd::DisplayPart(const unsigned char* frame_buffer)
{
    int w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    int h = EPD_HEIGHT;

    if (frame_buffer != NULL) {
        SendCommand(0x24);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                SendData(frame_buffer[i + j * w]);
            }
        }
    }

    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xFF);
    SendCommand(0x20);
    WaitUntilIdle();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5.
 *          You can use Epd::Init() to awaken
 */
void Epd::Sleep()
{
    SendCommand(0x10); //enter deep sleep
    SendData(0x01);
    DelayMs(200);

    DigitalWrite(reset_pin, LOW);
}

/* Adding missing functions from V1   */
 void Epd::SetFrameMemory(
    const unsigned char* image_buffer,
    int x,
    int y,
    int image_width,
    int image_height
) {
    int x_end;
    int y_end;
    unsigned char byte1;
    
    if (
       
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= this->width) {
        x_end = this->width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= this->height) {
        y_end = this->height - 1;
    } else {
        y_end = y + image_height - 1;
    }
    SetMemoryArea(x, y, x_end, y_end);
    /* set the frame memory line by line */
    for (int j = y; j <= y_end; j++) {
        
         SetMemoryPointer(x, j);
        for (uint8_t command = 0x24; true; command = 0x26)
        {
           SendCommand(command);
           for (int i = 0 ; i < image_width / 8; i++) {
             byte1 =  image_buffer[i + (j-y) * (image_width / 8)];
            if(command == 0x26)
              SendData(~byte1);
            else  
              SendData((byte1));
        }
        if(command == 0x26) break;
      }
    }
} 

void Epd::SetFrameMemory(const unsigned char* image_buffer) {
  unsigned char byte1;
  
    SetMemoryArea(0, 0, this->width - 1, this->height - 1);
 
    /* set the frame memory line by line */
    for (int j = 0; j <= this->height - 1; j++) {
        
        SetMemoryPointer(0, j);
        for (uint8_t command = 0x24; true; command = 0x26)
        {
           SendCommand(command);
            
           for (int i = 0 ; i < this->width / 8; i++) {
            byte1 =  image_buffer[i + j * (this->width / 8)];
            if(command == 0x26)
              SendData(~byte1);
            else  
              SendData((byte1));
              
         
        }
        if(command == 0x26) break;
      }
    }
}
void Epd::SetFrameMemory(const unsigned char* image_buffer,unsigned char frame,bool invert) {
  unsigned char byte1;
  
    SetMemoryArea(0, 0, this->width - 1, this->height - 1);
 
    /* set the frame memory line by line */
    for (int j = 0; j <= this->height - 1; j++) {
        
        SetMemoryPointer(0, j);
        
           SendCommand(frame);
            
           for (int i = 0 ; i < this->width / 8; i++) {
            byte1 =  image_buffer[i + j * (this->width / 8)];
            if(invert)
              SendData(~byte1);
            else  
              SendData((byte1));
           }
    }
}

void Epd::SetBox(
        int x,
        int y,
        int image_width,
        int image_height,
        unsigned char frame,
        unsigned char pattern
    
    )
    {
  int x_end;
    int y_end;
    
    if (
       
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= this->width) {
        x_end = this->width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= this->height) {
        y_end = this->height - 1;
    } else {
        y_end = y + image_height - 1;
    }
       
    SetMemoryArea(x, y, x_end, y_end);
  
         

    
      for (int j = y; j < y_end; j++) {
          SetMemoryPointer(x, j);
          SendCommand(frame);
          for (int i = 0; i < image_width / 8; i++) {
            SendData(pattern);
         }
      }
  
      
    }
    
void Epd::ClearFrameMemory(unsigned char color) {
   SetMemoryArea(0, 0, this->width - 1, this->height - 1);

    for (int j = 0; j < this->height; j++) {
        SetMemoryPointer(0, j);
         SendCommand(0x24);
        for (int i = 0; i < this->width / 8; i++) {
           SendData(color);
        }
    }
    
    for (int j = 0; j < this->height; j++) {
         SetMemoryPointer(0, j);
         SendCommand(0x26);
        for (int i = 0; i < this->width / 8; i++) {
            SendData(~color);
        }
    }
}

void Epd::DisplayFrameFull(void) {
    //DISPLAY REFRESH
    SendCommand(0x22);
    SendData(0xF7);
    SendCommand(0x20);
    WaitUntilIdle();
}


void Epd::DisplayFrameFast(void) {
    //DISPLAY REFRESH
     SendCommand(0x22);
    SendData(0xFF);
    SendCommand(0x20);
    WaitUntilIdle();
}

void Epd::SetMemoryArea(int x_start, int y_start, int x_end, int y_end) {
    SendCommand(0x44);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x_start >> 3) & 0xFF);
    SendData((x_end >> 3) & 0xFF);
    SendCommand(0x45);
    SendData(y_start & 0xFF);
    SendData((y_start >> 8) & 0xFF);
    SendData(y_end & 0xFF);
    SendData((y_end >> 8) & 0xFF);
}

void Epd::SetMemoryPointer(int x, int y) {
    SendCommand(0x4E);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x >> 3) & 0xFF);
    SendCommand(0x4F);
    SendData(y & 0xFF);
    SendData((y >> 8) & 0xFF);
  //  WaitUntilIdle();
}

/* END OF FILE */
