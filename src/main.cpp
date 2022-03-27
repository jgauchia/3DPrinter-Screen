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
*/

#include "Arduino.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <SD.h>

#include <upload_htm.h>
#include <Vars.h>
#include <CaptivePortal.h>
#include <FuncFiles.h>
#include <FuncInit.h>

#define SERIAL_DEBUG 1

/* format bytes as KB, MB or GB string */
String humanReadableSize(const size_t bytes) {
    if (bytes < 1024) return String(bytes) + " B";
    else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
    else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
    else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

void setup() 
{
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
  #endif
  init_SPIFFS();
  read_WIFICONFIG();

  if(init_WiFi()) 
  {
    init_SD();

    static const char* MIMETYPE_HTML{"text/html"};
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
    {
        AsyncWebServerResponse *response = request->beginResponse_P(200, MIMETYPE_HTML, upload_htm, upload_htm_len);
        response->addHeader("Server", "ESP Async Web Server");
        request->send(response);
    });

    // preflight cors check
    server.on("/", HTTP_OPTIONS, [](AsyncWebServerRequest * request)
    {
        AsyncWebServerResponse* response = request->beginResponse(204);
        response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization, FileSize");
        response->addHeader("Access-Control-Allow-Credentials", "true");
        request->send(response);
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest * request)
    {
        if (request->authenticate(HTTP_USERNAME, HTTP_PASSWORD))
            request->send(200);
        else {
            request->send(401);
            request->client()->close();
        }
    },
    [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final)
    {
        if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
            request->send(401);
            request->client()->close();
            return;
        }


        static unsigned long startTimer;
        if (!index) {
            startTimer = millis();
            const char* FILESIZE_HEADER{"FileSize"};

            Serial.printf("UPLOAD: Receiving: '%s'\n", filename.c_str());

            if (!request->hasHeader(FILESIZE_HEADER)) {
                request->send(400, MIMETYPE_HTML, "No filesize header present!");
                request->client()->close();
                Serial.printf("UPLOAD: Aborted upload because missing filesize header.\n");
                return;
            }

            Serial.printf("UPLOAD: fileSize: %s\n", request->header(FILESIZE_HEADER));

            if (request->header(FILESIZE_HEADER).toInt() >= MAX_FILESIZE) {
                request->send(400, MIMETYPE_HTML,
                              "Too large. (" + humanReadableSize(request->header(FILESIZE_HEADER).toInt()) +
                              ") Max size is " + humanReadableSize(MAX_FILESIZE) + ".");

                request->client()->close();
                Serial.printf("UPLOAD: Aborted upload because filesize limit.\n");
                return;
            }
                if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         // Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE);  // Open the file for writing in SPIFFS (create it, if doesn't exist)
    filename = String();
        }

        if (!final)
        if(UploadFile) UploadFile.write(data, len);

        //Store or do something with the data...
        //Serial.printf("file: '%s' received %i bytes\ttotal: %i\n", filename.c_str(), len, index + len);

        if (final)
        {

         if(UploadFile)          // If the file was successfully created                             
           UploadFile.close();   // Close the file again
            Serial.printf("UPLOAD: Done. Received %i bytes in %.2fs which is %.2f kB/s.\n",
                          index + len,
                          (millis() - startTimer) / 1000.0,
                          1.0 * (index + len) / (millis() - startTimer));
        }
    });

    server.onNotFound([](AsyncWebServerRequest * request)
    {
        Serial.printf("NOT_FOUND: ");
        if (request->method() == HTTP_GET)
            Serial.printf("GET");
        else if (request->method() == HTTP_POST)
            Serial.printf("POST");
        else if (request->method() == HTTP_DELETE)
            Serial.printf("DELETE");
        else if (request->method() == HTTP_PUT)
            Serial.printf("PUT");
        else if (request->method() == HTTP_PATCH)
            Serial.printf("PATCH");
        else if (request->method() == HTTP_HEAD)
            Serial.printf("HEAD");
        else if (request->method() == HTTP_OPTIONS)
            Serial.printf("OPTIONS");
        else
            Serial.printf("UNKNOWN");

        Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
        request->send(404, MIMETYPE_HTML, "404 - Not found.");
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();

    Serial.print("Upload files at ");
    Serial.println(WiFi.localIP());
  }
  else 
    init_WIFIMANAGER();
}

void loop() 
{
    delay(1000);
}

