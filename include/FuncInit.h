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