/*
       @file       Vars.h
       @brief      Declaración de variables y elementos usados en el programa

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Definición pines serial Marlin
// **********************************************
#define MARLIN_TX  16
#define MARLIN_RX  17
HardwareSerial *Marlin = &Serial2;

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