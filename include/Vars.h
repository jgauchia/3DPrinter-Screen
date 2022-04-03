/*
       @file       Vars.h
       @brief      Declaración de variables y elementos usados en el programa

       @author     Jordi Gauchia

       @date       27/03/2022
*/

#define SERIAL_DEBUG 1
#define FIRMWARE_VERSION "v0.0.1"
#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false
#define LVGL_TICK_PERIOD 10

// **********************************************
//  Definición pines serial Marlin
// **********************************************
#define MARLIN_TX  16
#define MARLIN_RX  17
HardwareSerial *Marlin = &Serial2;

// **********************************************
//  Declaración usuario y contraseña upload 
// **********************************************
const char* HTTP_USERNAME = "admin";
const char* HTTP_PASSWORD = "admin";

// **********************************************
//  Declaración para el TFT ILI9341 + LVGL
// **********************************************
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];


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
void WebnotFound(AsyncWebServerRequest *request);
String WebParser(const String& var);
String humanReadableSize(const size_t bytes);
void rebootESP(String message);
String listFiles(bool ishtml = false);
bool checkUserWebAuth(AsyncWebServerRequest * request);
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void configureWebServer();
String read_GCode(fs::FS &fs, const char *filename);
long parse_GCODE(String data, char* c);
void init_LVGL();
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data );
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p );
void touch_calibrate();
static void lv_tick_handler(void);
