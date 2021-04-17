#include <YATFT.h>
#include <util/yacam.h>
#include <util/yacodec.h>
#include <util/yasrl.h>
#include "ov7670_regs.h"
JPEG_DECODE  jpeg_decode;
YATFT  tft(0);
INTRFC ifc;
CAM    cam;
CODEC  codec;
SRL    srl;
#define IMG_SizeX    320
#define IMG_SizeY    240
uint8_t   mode = 0;
uint8_t   last_mode = 0;
uint8_t   start_capt = 0;
uint16_t  err;
void setup()
{
   // initialize the serial port
   Serial.begin(115200);
   // initialize the display
   tft.begin();
   tft.SetColor(BRIGHTRED);
   tft.ClearDevice();
}
void loop()
{
   // put your main code here, to run repeatedly:
   if (Serial.available())
   {
       uint8_t temp = Serial.read();
       switch (temp)
       {
           case 0x10: // Send single Photo
                mode = 1;
                start_capt = 1;
                if (last_mode != mode && last_mode != 2) {
                    tft.begin();
                    tft.SetRGB();               // Switch to RGB mode
                    cam.CamInit(&OV7670_QVGA[0][0]);
                    cam.CamVideoPreview(0, 0, 1, true);
                    codec.JPEGInit();
                    codec.JPEGSetRegs(IMG_SizeX, IMG_SizeY);
                    delay(1000);
                }
                break;
           case 0x20: // Continuous send Photo
                mode = 2;
                start_capt = 2;
                if (last_mode != mode && last_mode != 1) {
                    tft.begin();
                    tft.SetRGB();               // Switch to RGB mode
                    cam.CamInit(&OV7670_QVGA[0][0]);
                    cam.CamVideoPreview(0, 0, 1, true);
                    codec.JPEGInit();
                    codec.JPEGSetRegs(IMG_SizeX, IMG_SizeY);
                }
                break;
           case 0x30: // Receive single Photo
                mode = 3;
                start_capt = 3;
                if (last_mode != mode && last_mode != 4) {
                    tft.begin();
                    tft.SetYUV();               // Switch to YUV mode
                }
                break;
           case 0x40: // Continuous receive Photo
                mode = 4;
                start_capt = 4;
                if (last_mode != mode && last_mode != 3) {
                    tft.begin();
                    tft.SetYUV();               // Switch to YUV mode
                }
                break;
           default:
                break;
       }
   }
   if (mode == 1) // Send single Photo
   {
       if (start_capt == 1)
       {
           start_capt = 0;
           last_mode = mode;
           mode = 0;
           CamCapture();
       }
   }
   else if (mode == 2) // Continuous send Photo
   {
       while (1)
       {
           uint8_t temp = Serial.read();
           if (start_capt == 2)
           {
               start_capt = 0;
           }        
           if (temp == 0x21) // Stop ?
           {
               start_capt = 0;
               last_mode = mode;
               mode = 0;
               break;
           }
           if (CamCapture()) continue;
       }
   }
   else if (mode == 3) // Receive single Photo
   {
       if (start_capt == 3)
       {
           //Start capture
           start_capt = 0;
           last_mode = mode;
           mode = 0;
           Serial.print("!");
           srl.JPEGReadFromSerial(&jpeg_decode,  // jpeg decode structure
                                             0,  // x0 (left)
                                             0,  // y0 (top)
                                   GetMaxX()+1,  // x1 (right)
                                   GetMaxY()+1,  // y1 (bottom)
                                       32000); // max image size
           }
   }
   else if (mode == 4) // Continuous receive Photo
   {
       uint8_t temp = Serial.read();
       while (1)
       {
           if (start_capt == 4)
           {
               //Start capture
               start_capt = 0;
           }
           if (temp == 0x41) // Stop ?
           {
               start_capt = 0;
               last_mode = mode;
               mode = 0;
               break;
           }
           Serial.print("!");
           srl.JPEGReadFromSerial(&jpeg_decode,  // jpeg decode structure
                                             0,  // x0 (left)
                                             0,  // y0 (top)
                                   GetMaxX()+1,  // x1 (right)
                                   GetMaxY()+1,  // y1 (bottom)
                                        32000); // max image size
       }
   }
}
uint8_t  CamCapture(void)
{
   uint8_t temp = 0xff, temp_last = 0;
   bool is_header = false;
   uint32_t length = 0;
   length = codec.JPEGStart();
   uint32_t en_jpeg_address = ifc.rdReg32(0x414)<<2;
   int k = 0;
   if ((length >= 0x5FFFF) | (length == 0))
   {
       start_capt = 2;
       return 1;
   }
   temp = ifc.GetMem(en_jpeg_address+k);
   k++;
   length --;
   while ( length-- )
   {
       temp_last = temp;
       temp = ifc.GetMem(en_jpeg_address+k);
       k++;
       if (is_header == true)
       {
           Serial.write(temp);
       }
       else if ((temp == 0xD8) & (temp_last == 0xFF))
       {
           is_header = true;
           Serial.write(temp_last);
           Serial.write(temp);
       }
       if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
           break;
   }
   is_header = false;
   return 0;
}
