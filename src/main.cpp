/*
       @file       main.cpp
       @brief      Programa para pantalla Marlin con ESP32 + ILI9341 + SD

       @author     Jordi Gauchia

       @date       27/03/2022

       Pinout:
       ILI9341        MICRO SD       MARLIN
       --------------------------------------------------------------------
       VCC  3,3v      VCC  3,3v      VCC  5v
       GND  GND       GND  GND       GND  GND
       LED  GPIO33    CS   GPIO4     RX   GPIO16
       MISO GPIO27    MISO GPIO27    TX   GPIO17
       SCK  GPIO14    SCK  GPIO14
       MOSI GPIO13    MOSI GPIO13
       DC   GPIO15
       RST  GPIO32
       CS   GPIO2

       Librerías:
       ILI9341           :  https://github.com/Bodmer/TFT_eSPI
       AsyncTCP          :  https://github.com/me-no-dev/AsyncTCP
       ESAPsyncWebServer :  https://github.com/me-no-dev/ESPAsyncWebServer
       LVGL              :  https://github.com/lvgl/lvgl

SSID Portal captivo: ESP-WIFI-MANAGER
IP   Portal captivo: 192.168.4.1       
*/

#include "Arduino.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Ticker.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <SD.h>

#include <Vars.h>
#include <Init.h>

#include <WiFi_cfg.h>
#include <FuncVar.h>
#include <WebPage.h>
#include <FuncFiles.h>
#include <Web_server.h>
#include <FuncLVGL.h>

#include <File_screen.h>

WiFi_cfg wifi;
void setup() 
{
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
  #endif
  init_SPIFFS();

  lv_init(); 
  init_ili9341();
  touch_calibrate();

  init_SD();

  if(wifi.init()) 
  {
    configureWebServer();
    server.begin();
    // Prueba de lectura y renderizado de un GCode
    //read_GCode(SD,"/cube.gcode");
  }
  else 
    wifi.wifimanager();

  init_LVGL();
  show_file_screen();
}

void loop() 
{
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
  lv_task_handler ();
  xSemaphoreGive(xSemaphore);
  delay(5);
}

