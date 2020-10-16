/*
    Aqui definimos las variables de configuracion de servicio como los Topicos, usuarios y direcciones para conectar nuestro dispositivo
*/
#include <WString.h>                                                                            //include the String library

//---------CLIENT Settings-------------
#define ORG "flatbox"
#define DEVICE_TYPE "ESP32"
#define DEVICE_ID "EDIN"

#define Izquierdo
//#define Derecho

//---------Define coneection retry to modules
#define FAILTRESHOLD 150

//---------Definicion de Topicos segun canal del mensaje---------------------
const char publishTopic[]   = "iot/edin/evnt/reponse/json";                                     //Topico al cual se publican las respuestas 
const char responseTopic[]  = "iot/edin/register";                                              //Topico al cual se publica la identidad
const char manageTopic[]    = "iot/edin/mgmt/manage";                                           //Topico al cual se publican los mensajes de estado
const char updateTopic[]    = "iot/edin/device/update";                                         //Topico del cual se reciben las directrices de configuracion
const char rebootTopic[]    = "iot/edin/mgmt/initiate/device/reboot";                           //Topico del cual se reciben los mensajes de re-inicio

//--------Definicion de Variables de conectividad a Wifi-----------------------

char mqtt_server []  =   "192.168.1.9";                                                     //Variable que contien la direccion Ip del servidor mqtt
short  mqtt_port      =   1883;
const char* ssid    =   "MANAGER";                                                         //Variable que contiene el SSID del servicio Wifi al cual nos estamos comunicando
const char* password=   "M!n!g3r2019";                                                     //Password del servicio de wifi al que nos vamos a conectar

//--------Definicion de versionamiento-------------------------------------------

String FirmwareVersion= "V1.00";                                                                 //Variable de version de Firmware
String HardwareVersion= "V1.00";                                                                 //Variable de Version de Hardware

//--------Defeinicion de intervalos de tiempo paracontadores

unsigned long isalivemsg_interval            = 3600000;                                                    //Cada cuanto debe enviar un mensaje de que esta vivo
unsigned long waitfor_response_interval     = 60000UL;                                                    //Cuanto debe esperar antes de botar una operacion y enviar un mensaje de noanswear
unsigned long intervalo_entre_preguntas     = 10000UL;                                                    //Cuanto debe esperar antes de receibir la siguiente pregunta
