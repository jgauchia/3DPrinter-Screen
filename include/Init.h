/*
       @file       Init.h
       @brief      Inicialización 

       @author     Jordi Gauchia

       @date       03/04/2022
*/

// **********************************************
//  Definición pines microSD
// **********************************************
#define SD_CS   4
#define SD_MISO 27
#define SD_MOSI 13
#define SD_CLK  14

// **********************************************
//  Declaración para la microSD
// **********************************************
SPIClass spiSD(HSPI);

// **********************************************
//  Declaración para TFT ILI9341
// **********************************************
TFT_eSPI tft = TFT_eSPI();

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
    if (!SD.begin(SD_CS, spiSD, 16000000)) 
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
  tft.setRotation(3);
  tft.initDMA();
}
