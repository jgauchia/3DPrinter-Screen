/*
       @file       FuncVar.h
       @brief      Declaración de variables y funciones varias

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Función para formatear el tamaño del archivo
//  en B, Kb, Mb, Gb....
// **********************************************
String humanReadableSize(const size_t bytes)
{
    if (bytes < 1024) return String(bytes) + " B";
    else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " Kb";
    else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " Mb";
    else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " Gb";
}

// **********************************************
//  Función para parsear GCODE para vista previa
// **********************************************
long parse_GCODE(String data, char* c)
{
    int offset = data.indexOf(c);
    int offset1 = data.lastIndexOf(" ", offset + 1);
    return offset1 > 0 ? data.substring(offset + 1, offset + 1 + offset1 + 1).toInt() : data.substring(offset + 1).toInt();
}

// **********************************************
//  Función para calibrar TOUCH
// **********************************************
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (SPIFFS.exists(CALIBRATION_FILE)) 
  {
    if (REPEAT_CAL)
      SPIFFS.remove(CALIBRATION_FILE);
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) 
    tft.setTouch(calData);
  else 
  {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}