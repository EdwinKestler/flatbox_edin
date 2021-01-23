//---------CLIENT Settings-------------
#define ORG "flatbox"
#define DEVICE_TYPE "ESP32"
#define DEVICE_ID "EDIN"

// Connection info.
const char* ssid = "MANAGER";
const char* password =  "M!n!g3r2019";
const char* mqttServer = "192.168.1.34";
const int mqttPort = 1883;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;                                                 //Variable de Identificacion de Cliente para servicio de MQTT
const char* channelName = "testing_tinyPico";

//---------Definicion de Topicos segun canal del mensaje---------------------
const char operationTopic[]   = "iot/edin/evnt/reponse/json";                                     //Topico al cual se publican las respuestas 
const char responseTopic[]  = "iot/edin/evnt/testing_tinyPico";                                              //Topico al cual se publica la identidad
const char configTopic[]    = "iot/edin/mgmt/manage";                                            //Topico al cual se publican los mensajes de estado
const char stillaliveTopic[]    = "iot/edin/device/update";                                         //Topico del cual se reciben las directrices de configuracion
const char rebootTopic[]    = "iot/edin/mgmt/initiate/device/reboot";                           //Topico del cual se reciben los mensajes de re-inicio

//--------Definicion de versionamiento-------------------------------------------

String FirmwareVersion= "V1.00";                                                                 //Variable de version de Firmware
String HardwareVersion= "V1.00";                                                                 //Variable de Version de Hardware

//--------Defeinicion de intervalos de tiempo paracontadores

unsigned long isalivemsg_interval           = 3600000;                                                    //Cada cuanto debe enviar un mensaje de que esta vivo

//******************************************************************************Contadores de tiempo
unsigned long lastPregunta_Millis;                                                                        //Variable para saber cuantos millis han pasado 
unsigned long last_Case_Status_Millis;                                                                    //variable para saber cunatos millis han pasado desde el cambio de case.

//--------Defeinicion de intervalos de tiempo paracontadores

unsigned long intervalo_Case_Status_Millis  = 30000UL;                                                    //cada cuanto imprime en que estado esta y si reciboi un mensaje
