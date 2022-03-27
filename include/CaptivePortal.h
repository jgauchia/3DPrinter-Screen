/*
       @file       CaptivePortal.h
       @brief      Declaración variables y elementos usados para el WifiManager

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Declaración parámetros portal WifiManager
// **********************************************
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

// **********************************************
//  Declaración variables para almacenar valores
//  portal WifiManager
// **********************************************
String ssid;
String pass;
String ip;
String gateway;

// **********************************************
//  Declaración archivos SPIFF para guardar las
//  credenciales WifiManager
// **********************************************
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

// **********************************************
//  Función para iniciar portal WiFiManager
// **********************************************
void init_WIFIMANAGER()
{
    Serial.println("Modo AP (Access Point)");
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("Dirección IP AP: ");
    Serial.println(IP); 

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/wifimanager.html", "text/html");});
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
    {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST valor SSID
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID: ");
            Serial.println(ssid);
            write_Wifi_config(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST valor PASS
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password: ");
            Serial.println(pass);
            write_Wifi_config(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST valor IP
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP: ");
            Serial.println(ip);
            write_Wifi_config(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST valor gateway
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway: ");
            Serial.println(gateway);
            write_Wifi_config(SPIFFS, gatewayPath, gateway.c_str());
          }
        }
      }
      request->send(200, "text/plain", "Configurado. El ESP se reiniciará, Conectar a dirección IP: " + ip);
      delay(3000);
      ESP.restart();
    });
    server.begin();
}