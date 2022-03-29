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
//  Función para inicializar el LCD
// **********************************************
void init_ili9341()
{
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.initDMA();
}