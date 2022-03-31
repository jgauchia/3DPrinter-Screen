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
//  Declaración para el TFT ILI9341 + LVGL
// **********************************************
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];
TFT_eSPI tft = TFT_eSPI();

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
bool shouldReboot = false; 

// **********************************************
//  Timer Interrupciones ESP32
// **********************************************
Ticker tick; 
SemaphoreHandle_t xSemaphore = NULL;
static void lv_tick_handler(void)
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}

// **********************************************
//  Declaración previa de funciones
// **********************************************
// void init_WIFIMANAGER();
void WebnotFound(AsyncWebServerRequest *request);
String WebParser(const String& var);
void init_SPIFFS();
void init_SD();
String humanReadableSize(const size_t bytes);
void rebootESP(String message);
String listFiles(bool ishtml = false);
bool checkUserWebAuth(AsyncWebServerRequest * request);
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void configureWebServer();
void init_ili9341();
String read_GCode(fs::FS &fs, const char *filename);
long parse_GCODE(String data, char* c);
void init_LVGL();
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data );
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p );
void touch_calibrate();
static void lv_tick_handler(void);