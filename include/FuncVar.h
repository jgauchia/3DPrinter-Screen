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