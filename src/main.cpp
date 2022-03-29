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

SSID Portal captivo: ESP-WIFI-MANAGER
IP   Portal captivo: 192.168.4.1       
*/

#include "Arduino.h"
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <SD.h>

#define SERIAL_DEBUG 1
#define FIRMWARE_VERSION "v0.0.1"

#include <Vars.h>
#include <FuncVar.h>
#include <WebPage.h>
#include <FuncFiles.h>
#include <CaptivePortal.h>
#include <FileServer.h>
#include <FuncInit.h>

void setup() 
{
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
  #endif
  init_SPIFFS();
  read_WIFICONFIG(); 
  init_ili9341();
  init_SD();

  if(init_WiFi()) 
  {

    configureWebServer();
    server.begin();

    // Prueba de lectura y renderizado de un GCode
    read_GCode(SD,"/cube.gcode");
  }
  else 
    init_WIFIMANAGER();
}

void loop() 
{
}

