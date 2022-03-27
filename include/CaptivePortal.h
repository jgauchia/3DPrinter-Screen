/*
       @file       CaptivePortal.h
       @brief      Declaración variables y elementos usados para el WifiManager

       @author     Jordi Gauchia

       @date       27/03/2022
*/

// **********************************************
//  Declaración parámetros portal WifiManager
// **********************************************
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

// **********************************************
//  Declaración variables para almacenar valores
//  portal WifiManager
// **********************************************
String ssid;
String pass;
String ip;
String gateway;

// **********************************************
//  Declaración archivos SPIFF para guardar las
//  credenciales WifiManager
// **********************************************
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

// **********************************************
//  Declaración archivos SPIFF para guardar las
//  credenciales WifiManager
// **********************************************