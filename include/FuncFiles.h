/*
       @file       FuncFiles.h
       @brief      Funciones para tratamiento de archivos SPIFFS y SD

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Función para leer GCode
// **********************************************
String read_GCode(fs::FS &fs, const char *filename)
{
  File file = SD.open(filename);
  String fileContent;
  int x = 0;
  int y = 0;
  int z = 0;

  if(!file)
  {
    Serial.println("- Error al leer GCODE");
    return String();
  }  
  else
  {
    Serial.print("GCODE: ");
    Serial.println(filename);
    while(file.available())
    {
      fileContent = file.readStringUntil('\n');
      if (parse_GCODE(fileContent,"G") == 1 )
      {
        x = parse_GCODE(fileContent,"X");
        y = parse_GCODE(fileContent,"Y");
        z = parse_GCODE(fileContent,"Z");
      }
      if ( x != 0 && y != 0 )
      {
        Serial.print(x);
        Serial.print("   ");
        Serial.println(y);
      }
      if ( z!= 0)
        Serial.println(z);
    }
    Serial.println("EOF");
  }
  file.close();
  return fileContent;
}