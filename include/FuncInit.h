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