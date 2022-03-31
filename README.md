# IceNav-v3
ESP32 3D Printer Screen 
Programa para pantalla Marlin


       Pinout:
       ILI9341        MICRO SD       MARLIN
       --------------------------------------------------------------------
       VCC  3,3v      VCC  3,3v      VCC  5v
       GND  GND       GND  GND       GND  GND
       LED  GPIO33    CS   GPIO4     RX   GPIO16
       MISO GPIO27    MISO GPIO27    TX   GPIO17
       SCK  GPIO14    SCK  GPIO14
       MOSI GPIO13    MOSI GPIO13
       DC   GPIO15
       RST  GPIO32
       CS   GPIO2

       Librerías:
       ILI9341           :  https://github.com/Bodmer/TFT_eSPI
       AsyncTCP          :  https://github.com/me-no-dev/AsyncTCP
       ESAPsyncWebServer :  https://github.com/me-no-dev/ESPAsyncWebServer
       LVGL              :  https://github.com/lvgl/lvgl


SSID Portal captivo: ESP-WIFI-MANAGER
IP   Portal captivo: 192.168.4.1