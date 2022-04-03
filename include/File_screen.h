/*
       @file       File_screen.h
       @brief      Pantalla ficheros GCODE

       @author     Jordi Gauchia

       @date       03/04/2022
*/

#ifndef File_screen_h
#define File_screen_h

#include <lvgl.h>
#include <Arduino.h>
#include <SD.h>

// **********************************************
//  Declaración funciones
// **********************************************
void event_file_handler(lv_event_t * e);
void create_directory_listing();
void show_file_screen();

#endif