/*
       @file       FuncInit.h
       @brief      Funciones inicialización necesarias para el programa

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Función para inicializar almacen. SPIFFS
// **********************************************
void init_SPIFFS() 
{
  if (!SPIFFS.begin(true))
    Serial.println("SPIFFS no montado");
  else
    Serial.println("SPIFFS montado correctamente");
}

// **********************************************
//  Función para inicializar Wifi
// **********************************************
bool init_WiFi() 
{
  if(ssid=="" || ip=="")
  {
    Serial.println("SSID o IP no definida.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

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

// **********************************************
//  Función para leer configuración WiFi
// **********************************************
void read_WIFICONFIG()
{
  ssid = read_WiFi_config(SPIFFS, ssidPath);
  pass = read_WiFi_config(SPIFFS, passPath);
  ip = read_WiFi_config(SPIFFS, ipPath);
  gateway = read_WiFi_config(SPIFFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);
}

// **********************************************
//  Función para inicializar SD
// **********************************************
void init_SD()
{
    digitalWrite(SD_CS,HIGH);
    SPI.end();
    spiSD.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, spiSD, 100000000)) 
    { 
      Serial.println("Error al iniciar SD");
      return;
    }   
}

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