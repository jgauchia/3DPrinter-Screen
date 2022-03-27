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

// **********************************************
//  Declaración timer y espera conexión WiFi
// **********************************************
unsigned long previousMillis = 0;
const long wifi_interval = 10000;  

// **********************************************
//  Declaración variables dirección Wifi
// **********************************************
IPAddress localIP;
IPAddress localGateway;
IPAddress subnet(255, 255, 0, 0);

// **********************************************
//  Declaración usuario y contraseña upload 
// **********************************************
const char* HTTP_USERNAME = "admin";
const char* HTTP_PASSWORD = "admin";

// **********************************************
//  Declaración tamaño máximo archivo upload
// **********************************************
const size_t MAX_FILESIZE = 1024 * 1024 * 15;
File UploadFile; 

// **********************************************
//  Declaración servidor WEB
// **********************************************
AsyncWebServer server(80);

// **********************************************
//  Declaración previa de funciones
// **********************************************
void init_WIFIMANAGER();
void WebnotFound(AsyncWebServerRequest *request);
String WebParser(const String& var);
String read_WiFi_config(fs::FS &fs, const char * path);
void write_Wifi_config(fs::FS &fs, const char * path, const char * message);
void init_SPIFFS();
bool init_WiFi();
void read_WIFICONFIG();
void init_SD();
String humanReadableSize(const size_t bytes);