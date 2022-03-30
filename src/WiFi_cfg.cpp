/*
       @file       WiFi_cfg.cpp
       @brief      Clase para la gestión de la conexión WiFi (user,pass....)

       @author     Jordi Gauchia

       @date       30/03/2022
*/

#include "WiFi_cfg.h"

String WiFi_cfg::ssid    = "";
String WiFi_cfg::pass    = "";
String WiFi_cfg::ip      = "";
String WiFi_cfg::gateway = "";
const char* WiFi_cfg::ssidPath = "/ssid.txt";
const char* WiFi_cfg::passPath = "/pass.txt";
const char* WiFi_cfg::ipPath = "/ip.txt";
const char* WiFi_cfg::gatewayPath = "/gateway.txt";

WiFi_cfg::WiFi_cfg()
{
  server = new AsyncWebServer(80);
}

bool WiFi_cfg::init()
{
  WiFi_cfg::ssid = WiFi_cfg::read_cfg(SPIFFS, ssidPath);
  WiFi_cfg::pass = WiFi_cfg::read_cfg(SPIFFS, passPath);
  WiFi_cfg::ip =  WiFi_cfg::read_cfg(SPIFFS, ipPath);
  WiFi_cfg::gateway = WiFi_cfg::read_cfg(SPIFFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);

  if (ssid == "" || ip == "")
  {
    Serial.println("SSID o IP no definida.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip);
  localGateway.fromString(gateway);
  subnet.fromString("255.255.0.0");

  if (!WiFi.config(localIP, localGateway, subnet))
  {
    Serial.println("Error configurando WiFi");
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Conectando a red WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) 
  {
    currentMillis = millis();
    if (currentMillis - previousMillis >= wifi_interval)
    {
      Serial.println("Error de conexión");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}

void WiFi_cfg::wifimanager()
{
  Serial.println("Modo AP (Access Point)");
  WiFi.softAP("ESP-WIFI-MANAGER", NULL);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Dirección IP AP: ");
  Serial.println(IP); 

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/wifimanager.html", "text/html");});
  server->serveStatic("/", SPIFFS, "/");
    
  server->on("/", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        // HTTP POST valor SSID
        if (p->name() == "ssid")
        {
          ssid = p->value().c_str();
          Serial.print("SSID: ");
          Serial.println(ssid.c_str());
          WiFi_cfg::write_cfg(SPIFFS, ssidPath, ssid.c_str());
        }
          // HTTP POST valor PASS
        if (p->name() == "pass") 
        {
          pass = p->value().c_str();
          Serial.print("Password: ");
          Serial.println(pass.c_str());
          WiFi_cfg::write_cfg(SPIFFS, passPath, pass.c_str());
        }
        // HTTP POST valor IP
        if (p->name() == "ip")
        {
          ip = p->value().c_str();
          Serial.print("IP: ");
          Serial.println(ip.c_str());
          WiFi_cfg::write_cfg(SPIFFS, ipPath, ip.c_str());
        }
        // HTTP POST valor gateway
        if (p->name() == "gateway") 
        {
          gateway = p->value().c_str();
          Serial.print("Gateway: ");
          Serial.println(gateway.c_str());
          WiFi_cfg::write_cfg(SPIFFS, gatewayPath, gateway.c_str());
        }
      }
    }
    request->send(200, "text/plain", "Configurado. El ESP se reiniciará, Conectar a dirección IP: " + ip);
    delay(3000);
    ESP.restart();
    });
    server->begin();
}

String WiFi_cfg::read_cfg(fs::FS &fs, const char * path)
{
  Serial.printf("Leyendo Archivo: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory())
  {
    Serial.println("- Error al abrir el archivo");
    return String();
  }
  
  String fileContent;
  while(file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

void WiFi_cfg::write_cfg(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Grabando Archivo: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file)
  {
    Serial.println("- Error al grabar el archivo");
    return;
  }
  if(file.print(message))
  {
    Serial.println("- Archivo grabado");
  }
  else 
  {
    Serial.println("- Archivo no grabado");
  }
}
