/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "ledDisplay.hpp" // own header
#include "XYMatrix.h"     // convert x, y to index into LED array.
#include "MatrixFontCommon.h"

#include "FastLED.h"
#include "FX.h"

//CRGBPalette16 currentPalette;
//TBlendType    currentBlending;

//extern CRGBPalette16 myRedWhiteBluePalette;
//extern const TProgmemPalette16 IRAM_ATTR myRedWhiteBluePalette_p;

//#include "palettes.h"

#define LED_ROW     8u
#define LED_COL     32u
#define NUM_LEDS    LED_ROW * LED_COL
#define DATA_PIN    13 
#define BRIGHTNESS  25
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER GRB

#define LED_DISPLAY_TASK_STACK_SIZE   4096

typedef struct
{
   uint8_t x;   /* data */
   uint8_t y;
   CRGB colour;
   const bitmap_font *font;
   const char* string;
}text_t;



static CRGB gLeds[NUM_LEDS];
static TaskHandle_t gLedHandle = NULL;
static QueueHandle_t gLedQueue;

static void LedDisplayTask(void *pvParameters); 
static esp_err_t WriteText(text_t*);


/**
 * @brief Write Text to Display
 * 
 * @param pText 
 */
static esp_err_t WriteText(text_t *pText)
{
   esp_err_t err= ESP_OK;
   /*
    * Check x, y value fall within matrix
    */
   if ((pText->x > LED_COL) || (pText->y > LED_ROW))
   {
      err = ESP_ERR_INVALID_ARG;
   }

   if (ESP_OK == err)
   {
      /*
       * For each character in the string
       */
      for(uint8_t c = 0; c < strlen(pText->string); c++)
      {
         /*
          * For each row in char
          */
         for (uint8_t xchr = c * pText->font->Width ;  xchr <  (c * pText->font->Width) + pText->font->Width ; xchr++)
         {
            uint8_t xPxl = xchr + pText->x;
            
             
            if (xPxl > LED_COL){
               //printf("x position too large \n");
               break;
            }
  
            /*
             * For each column in char
             */
            for(uint8_t ychr = 0; ychr < pText->font->Height; ychr++)
            {
               uint8_t yPxl = ychr + pText->y;
               
               if (yPxl > LED_ROW){
                  //printf("y position too large \n");
                  break;
               }
               //printf("x = %d, y = %d index = %d", xchr + pText->x, ychr + pText->y, XY(xchr + pText->x,ychr + pText->y));
         
               if (getBitmapFontPixelAtXY(pText->string[c], xchr % pText->font->Width, ychr, pText->font))
               {
                  //printf(" x\n");
                  gLeds[255u - XY(xPxl, yPxl)] = pText->colour;
               }
               else
               {
                  //printf("\n");
               }
            }
         }
      }
   } 

   return err;  
}



static void LedDisplayTask(void *pvParameters) 
{
 
   ledDisplayQNFO_t ledDisplayNFo;
   text_t myTest;
   
   myTest.colour = CRGB::BlueViolet;
   myTest.font = fontLookup(font5x7);
   myTest.string = "203451";
   myTest.x = 2;
   myTest.y = 4;

    printf("LED Display Task Started \n");
    printf("------------------------\n\n");

    printf("Green \n");
    gLeds[0] = CRGB::Green;
    FastLED.show();

    vTaskDelay(pdMS_TO_TICKS(500));

    printf("Red \n");
    gLeds[0] = CRGB::Red;
    FastLED.show();


    vTaskDelay(pdMS_TO_TICKS(500));

    printf("Blue \n");
    gLeds[0] = CRGB::Blue;
    FastLED.show();

   for(;;){
   for (uint8_t i= 0; i<32;i++)
   {
      myTest.x = i;
      WriteText(&myTest);
      FastLED.show();
      //vTaskDelay(pdMS_TO_TICKS(2));
      FastLED.clear();
      vTaskDelay(pdMS_TO_TICKS(100));
   }
   for (uint8_t i = 32; i>1;i--)
   {
      myTest.x = i;
      WriteText(&myTest);
      FastLED.show();
      //vTaskDelay(pdMS_TO_TICKS(2));
      FastLED.clear();
      vTaskDelay(pdMS_TO_TICKS(100));
   }}
   

    while(1)
    {
         if  (pdPASS ==  xQueueReceive(gLedQueue, &ledDisplayNFo,(TickType_t) pdMS_TO_TICKS(1000)) )
         {
            //printf("LED Display Q Received\n");
         }else
         {
            //printf("LED Display Q TO\n");
         }
    }
}

/**
 * @brief Kicks off the LED Display task
 * 
 */
void initLEDDisplay() 
{
    printf("Init LED Display \n\r");
  
    // The WS2812 family uses the RMT driver
    FastLED.addLeds<LED_TYPE, DATA_PIN>(gLeds, NUM_LEDS);

    //vTaskDelay(pdMS_TO_TICKS(1000));

    FastLED.clear();
    FastLED.show();

    //vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Set Power\n");
    // I have a 2A power supply, although it's 12v
    FastLED.setMaxPowerInVoltsAndMilliamps(5,500);

    /*
     * gLED Queue will be the interface into LED Q
     */
    gLedQueue = xQueueCreate( 10, sizeof( struct AMessage * ) );
    
    /*
     * Stat LED task
     */
    xTaskCreatePinnedToCore(&LedDisplayTask, "LED DISP", LED_DISPLAY_TASK_STACK_SIZE, NULL, 5, &gLedHandle, 0);
}

#define LED_QUEUE_WAIT_TIME pdMS_TO_TICKS(100)
/**
 * @brief 
 * 
 * @param ledMsg 
 * @return esp_err_t 
 */
esp_err_t PushLEDMsg(ledDisplayQNFO_t* ledMsg)
{
  esp_err_t err = ESP_OK;

  xQueueSend(gLedQueue, ledMsg, LED_QUEUE_WAIT_TIME); 

  return err;
}
