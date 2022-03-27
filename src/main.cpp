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
#include <WifiManager.h>
#include <FuncInit.h>
#include <FuncFiles.h>

#define SERIAL_DEBUG 1

/****************************************************************************

       POC to upload a file to esp32 with authorization

       Browse to your esp32 to upload files

 *****************************************************************************/

const char* HTTP_USERNAME = "admin";
const char* HTTP_PASSWORD = "admin";
const size_t MAX_FILESIZE = 1024 * 1024 * 15;
 File UploadFile; 

AsyncWebServer server(80); 

/* format bytes as KB, MB or GB string */
String humanReadableSize(const size_t bytes) {
    if (bytes < 1024) return String(bytes) + " B";
    else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
    else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
    else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

void setup() {
  
#ifdef DEBUG
    Serial.begin(115200);
#endif

  init_SPIFFS();

  // Load values saved in SPIFFS
  ssid = read_WiFi_config(SPIFFS, ssidPath);
  pass = read_WiFi_config(SPIFFS, passPath);
  ip = read_WiFi_config(SPIFFS, ipPath);
  gateway = read_WiFi_config(SPIFFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);

if(init_WiFi()) {

  digitalWrite(4,HIGH);
    SPI.end();
   spiSD.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
   if (!SD.begin(SD_CS, spiSD, 100000000)) 
   { 
    Serial.println("initialization failed!");
    return;
  }

    // WiFi.mode(WIFI_STA);
    // WiFi.setSleep(false);
    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Serial.printf("Connecting to WIFI_SSID %s with PSK %s...\n", WIFI_SSID, WIFI_PASSWORD);
    // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Serial.printf("WiFi Failed!\n");
    //     return;
    // }

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
    else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            write_Wifi_config(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            write_Wifi_config(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            write_Wifi_config(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            write_Wifi_config(SPIFFS, gatewayPath, gateway.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(3000);
      ESP.restart();
    });
        server.begin();
  }
}

void loop() {
    delay(1000);
}

