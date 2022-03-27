/*
       @file       FuncFiles.h
       @brief      Funciones para tratamiento de archivos SPIFFS y SD

       @author     Jordi Gauchia

       @date       27/03/2022
*/


// **********************************************
//  Función para leer configuracion SPIFFS del
//  WifiManager
// **********************************************
String read_WiFi_config(fs::FS &fs, const char * path)
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

// **********************************************
//  Función para grabar configuracion SPIFFS del
//  WifiManager
// **********************************************
void write_Wifi_config(fs::FS &fs, const char * path, const char * message)
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