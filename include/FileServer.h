/*
       @file       FileServer.h
       @brief      Declaración de variables y funciones usadas en el File Server

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Función para mensaje de error si no se 
//  encuentra la Web del FileServer
// **********************************************
void WebnotFound(AsyncWebServerRequest *request) 
{
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);
  request->send(404, "text/plain", "Not found");
}

// **********************************************
//  Función para reemplazar variables dentro
//  del archivo HTML de la Web
// **********************************************
String WebParser(const String& var)
{
  if (var == "FIRMWARE") 
    return FIRMWARE_VERSION;
  if (var == "FREESD") 
    return humanReadableSize((SD.totalBytes() - SD.usedBytes()));
  if (var == "USEDSD") 
    return humanReadableSize(SD.usedBytes());
  if (var == "TOTALSD") 
    return humanReadableSize(SD.totalBytes());
}