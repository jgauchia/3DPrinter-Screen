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

// **********************************************
//  Función para reiniciar ESP
// **********************************************
void rebootESP(String message) 
{
  Serial.print("Rebooting ESP32: "); Serial.println(message);
  ESP.restart();
}

// **********************************************
//  Función para listar archivos SD en la Web
// **********************************************
String listFiles(bool ishtml) 
{
  String returnText = "";
  Serial.println("Listing files stored on SD");
  File root = SD.open("/");
  File foundfile = root.openNextFile();
  if (ishtml) 
  {
    returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
  }
  while (foundfile) 
  {
    if (ishtml)
    {
      returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'download\')\">Download</button>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";
    } 
    else
    {
      returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
    }
    foundfile = root.openNextFile();
  }
  if (ishtml)
  {
    returnText += "</table>";
  }
  root.close();
  foundfile.close();
  return returnText;
}

// **********************************************
//  Función para autentificarse en la Web
// **********************************************
bool checkUserWebAuth(AsyncWebServerRequest * request)
{
  bool isAuthenticated = false;
  if (request->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) 
  {
    Serial.println("is authenticated via username and password");
    isAuthenticated = true;
  }
  return isAuthenticated;
}

// **********************************************
//  Función para subir archivos a la Web
// **********************************************
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index) 
  {
    String logmessage = "Upload Start: " + String(filename);
    request->_tempFile = SD.open("/" + filename, "w");
    Serial.println(logmessage);
  }

  if (len)
    request->_tempFile.write(data, len);
      
    if (final)
    {
      String logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
      request->_tempFile.close();
      Serial.println(logmessage);
      request->redirect("/");
    }
} 
 
// **********************************************
//  Función para crear servicio Web
// **********************************************
void configureWebServer() 
{
  server.onNotFound(WebnotFound);
  server.onFileUpload(handleUpload);
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->requestAuthentication();
    request->send(401);
  });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);
    request->send_P(401, "text/html", logout_html, WebParser);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();

    if (checkUserWebAuth(request)) 
    {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send_P(200, "text/html", index_html, WebParser);
    } 
    else 
    {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });

  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();

    if (checkUserWebAuth(request)) 
    {
      request->send(200, "text/html", reboot_html);
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      shouldReboot = true;
    }
    else
    {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });

  server.on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) 
    {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(200, "text/plain", listFiles(true));
    }
    else
    {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });

  server.on("/file", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request))
    {
      logmessage += " Auth: Success";
      Serial.println(logmessage);

      if (request->hasParam("name") && request->hasParam("action")) 
      {
        const char *fileName = request->getParam("name")->value().c_str();
        const char *fileAction = request->getParam("action")->value().c_str();

        logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url() + "?name=" + String(fileName) + "&action=" + String(fileAction);

        if (!SD.exists(fileName)) 
        {
          Serial.println(logmessage + " ERROR: file does not exist");
          request->send(400, "text/plain", "ERROR: file does not exist");
        }
        else
        {
          Serial.println(logmessage + " file exists");
          if (strcmp(fileAction, "download") == 0) 
          {
            logmessage += " downloaded";
            request->send(SD, fileName, "application/octet-stream");
          }
          else if (strcmp(fileAction, "delete") == 0) 
          {
            logmessage += " deleted";
            SD.remove(fileName);
            request->send(200, "text/plain", "Deleted File: " + String(fileName));
          } 
          else
          {
            logmessage += " ERROR: invalid action param supplied";
            request->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
          Serial.println(logmessage);
        }
      } 
      else
      {
        request->send(400, "text/plain", "ERROR: name and action params required");
      }
    } 
    else 
    {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
}
