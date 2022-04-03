/*
       @file       File_screen.cpp
       @brief      Pantalla ficheros GCODE

       @author     Jordi Gauchia

       @date       03/04/2022
*/

#include <File_screen.h>

static lv_obj_t * FileButton = NULL;
static lv_obj_t * FileList;

// **********************************************
//  Evento al pulsar el nombre del archivo.
// **********************************************
void event_file_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        Serial.println(lv_list_get_btn_text(FileList, obj));

        if(FileButton == obj) 
            FileButton = NULL;
        else 
            FileButton = obj;
        
        lv_obj_t * parent = lv_obj_get_parent(obj);
        uint32_t i;
        for(i = 0; i < lv_obj_get_child_cnt(parent); i++)
        {
            lv_obj_t * child = lv_obj_get_child(parent, i);
            if(child == FileButton)
            {
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else 
            {
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}

// **********************************************
//  Crea lista de archivos
// **********************************************
void create_directory_listing()
{
  int file_count = 0;
  File root = SD.open("/");

  FileList = lv_list_create(lv_scr_act());
  lv_obj_set_size(FileList, 320, 180);

  while (true)
  {
    File entry =  root.openNextFile();
    if (! entry)
      break;
    
    lv_obj_t * btn = lv_list_add_btn(FileList, LV_SYMBOL_FILE, entry.name());
    lv_obj_add_event_cb(btn, event_file_handler, LV_EVENT_CLICKED, NULL);

    file_count++;
    entry.close();
  }
}

// **********************************************
//  Crea lista de archivos
// **********************************************
void show_file_screen()
{
    create_directory_listing();
}