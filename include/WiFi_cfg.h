/*
       @file       WiFi_cfg.h
       @brief      Clase para la gestión de la conexión WiFi (user,pass....)

       @author     Jordi Gauchia

       @date       30/03/2022
*/

#ifndef WiFi_cfg_h
#define WiFi_cfg_h

#include "Arduino.h"
#include <SD.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WiFi_cfg
{       
    public:
        WiFi_cfg();
        bool init();
        void wifimanager();

    private:
        static String ssid;
        static String pass;
        static String ip;
        static String gateway;
        IPAddress localIP;
        IPAddress localGateway;
        IPAddress subnet;
        unsigned long previousMillis = 0;
        const long wifi_interval = 10000; 
        AsyncWebServer *server;
        static const char* ssidPath;
        static const char* passPath;
        static const char* ipPath;
        static const char* gatewayPath;
  
        String read_cfg(fs::FS &fs, const char * path);
        static void write_cfg(fs::FS &fs, const char * path, const char * message);  
};

#endif