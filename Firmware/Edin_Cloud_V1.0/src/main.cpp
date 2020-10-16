/*
  Version 1.0 de firmware para guantes de Educacion Inclusiva
  HardWare:
    1)ESP32 WEMOS D1 MINI
    2)Botones 
    3)Motores piezo electricos lineales 
    puertos de conexion:
    Botones:
     Puerto ---- Wemos  -- Boton
                GPIO18  -- Menique
                GPIO04  -- Anular
                GPIO16  -- MEDIO
                GPIO17  -- INDICE
                GPIO05  -- PULGAR
    Debounce by Hardware ver esquematico en archivo Egalecad: ESP32_Shield.sch
    Motores:
      Puertos ----Wemos -- Motor
      CH1       GPIO25  -- Menique
      CH2       GPIO32  -- Anular
      CH3       GPIO02  -- Medio
      CH4       GPIO27  -- Indice
      CH5       GPIO26  -- Pulgar
      *************************Warning this design has a flaw in the debounce CIRCUIT ****** DONOT USE FOR PRPODUCTION 
      
*/
#include <Arduino.h>                                                                                      //----------------------------inicamos el framework. 
#include <WiFi.h>                                                                                         //----------------------------Libreria de Wifi
#include <PubSubClient.h>                                                                                 //----------------------------Libreria Cliente de MQTT
#include "settings.h"                                                                                     //----------------------------Libreira Local de Variables
#include <ArduinoJson.h>                                                                                  //----------------------------JSON Arduino Library

//*****************************************************************************Definicion de Variables Globales
String Sipaddrs = "000.000.000.000";
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;                                                 //Variable de Identificacion de Cliente para servicio de MQTT 
uint32_t chipId = 0;                                                                                      //variable donde se alamacena el identificador de cliente para el servicio de MQTT (OJO Este debe ser un identificador unico para cada dispositivo fisico sino se reiniciara el servidor MQTT)
int retry = 0;                                                                                            //variable donde se almacenan los intentos de conexion fallidos
//******************************************************************************Contadores de tiempo
unsigned long lastPregunta_Millis;                                                                        //Variable para saber cuantos millis han pasado 

//******************************************************************************Variables de mensaje de operaciones
enum Operaciones {
  UNDEF, 
  PLUS, 
  BY, 
  DIVIDED
  };

int idOperation       = 0;
int idModule          = 0;
int gloveCode         = 0;
int number1           = 0;
char operation[4];
int number2           = 0;
int result            = 0;
int answer            = 0;
char ACTION [10];
char topico [32];
 boolean isvalid = false;

//******************************************************************************FSM Settings
bool mesgrecv = false;

#define STATE_IDLE                    0
#define STATE_PREGUNTA                1
#define STATE_TRANSMIT_RESPUESTA      2
#define STATE_UPDATE                  3
#define STATE_TRANSMIT_ALARM_UPDATE   4
#define STATE_TRANSMIT_DEVICE_UPDATE  5
#define STATE_ENSENAR                 6
int fsm_state;

//----------------------------------------------------------------------Funcion remota para mandar a dormir el esp despues de enviar un RFID
void massageUpdate (byte* payloadrsp) {
  //Calcule el espacio necesario para:
  // +1 objeto de 3 miembros {"servidor_MQTT": "IP_MQtt", "servidor_NTP":"IP_NTP", "SSID":"nuevo_SSID"}
  // +2 objetos de 1 miembro {"intervalo_de_vida":3600000},{"reintentos_de_conexion":10}
  const int capacity = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> edin_config_json;                                                 //Se establece un Buffer de 1o suficientemente gande para almacenar los menasajes JSON
  DeserializationError err = deserializeJson(edin_config_json, payloadrsp);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    return;
  }
  strlcpy(mqtt_server, edin_config_json["servidor_MQTT"] | "192.168.9.1", 32);
  Serial.println(edin_config_json["servicdor_MQTT"].as<char *>());
  mqtt_port =     edin_config_json["mqtt_port"] | 8083;
  Serial.println(edin_config_json["mqtt_port"].as<int>());
}

//----------------------------------------------------------------------Funcion remota para mandar a dormir el esp despues de enviar un RFID
void massageRecieved (byte* payloadrsp) {
  //Calcule el espacio necesario para:
  // +1 objeto de 3 miembros {"servidor_MQTT": "IP_MQtt", "servidor_NTP":"IP_NTP", "SSID":"nuevo_SSID"}
  // +2 objetos de 1 miembro {"intervalo_de_vida":3600000},{"reintentos_de_conexion":10}
  const int capacity = JSON_OBJECT_SIZE(14);
  StaticJsonDocument<capacity> edin_operation_json;                                                 //Se establece un Buffer de 1o suficientemente gande para almacenar los menasajes JSON

  //char payloadrsp[] = {"idOperation":22,"idModule":5,"gloveCode":null,"number1":10,"operation":"+","number2":10,"result":20,"answer":null,"ACTION":"Preguntar","topic":"all","created_at":"2020-10-15T23:06:29.000Z","updated_at":"2020-10-15T23:06:29.000Z"};


  DeserializationError err = deserializeJson(edin_operation_json, payloadrsp);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    return;
  }

  idOperation = edin_operation_json["idOperation"]  | 0 ;
  idModule    = edin_operation_json["idModule"]     | 1;
  gloveCode   = edin_operation_json["gloveCode"]    | chipId;
  number1     = edin_operation_json["number1"]      | 0;
  strlcpy(operation, edin_operation_json["operation"]  | "+", 4);
  number2     = edin_operation_json["number2"]         | 0;
  result      = edin_operation_json["result"]          | 0;
  answer      = edin_operation_json["answer"]          | 0;
  strlcpy(ACTION, edin_operation_json["ACTION"] | "pregunta", 10);
  strlcpy(topico, edin_operation_json["topic"]  | "all",32);
  
  String mensajede_operacion =  String(number1)+ String(operation) + String(number2);
  Serial.print("laoperacion es: ");
  Serial.println(mensajede_operacion);
  mesgrecv = true;  
}

//********************************************************************************CALLBACK********************************************************************************************
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print(F("Se recibio mensaje del Topico: "));                                                      //Imprimir un mensaje seÃ±alando sobre que topico se recibio un mensaje
  Serial.println(topic);                                                                                  //Imprimir el Topico
  //testing ptinting the message.
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
  }
  
  if (strcmp (rebootTopic, topic) == 0) {                                                                 //verificar si el topico conicide con el Topico rebootTopic[] definido en el archivo settings.h local
    Serial.println(F("Rebooting in 5 seconds..."));                                                                    //imprimir mensaje de Aviso sobre reinicio remoto de unidad.
    delay(5000);
    ESP.restart();                                                                                          //Emitir comando de reinicio para ESP32
  }
  
  if (strcmp (updateTopic, topic) == 0) {                                                                 //verificar si el topico conicide con el Topico updateTopic[] definido en el archivo settings.h local
    massageUpdate(message);                                                                                //enviar a la funcion handleUpdate el contenido del mensaje para su parseo.
  } 

  if (strcmp (responseTopic, topic) == 0) {                                                                 //verificar si el topico conicide con el Topico updateTopic[] definido en el archivo settings.h local
    massageRecieved(message);                                                                                //enviar a la funcion handleUpdate el contenido del mensaje para su parseo.
  }
}
//----------------------------------------------------------------------definicion de Cliente WIFI para ESP8266 y cliente de publicacion y subcripcion
WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, callback, espClient);

//*****************************************************************************Funcion de Conexion a Servicio de MQTT
void mqttConnect() {
  if (!!!client.connected()) {                                                                            //Verificar si el cliente se encunetra conectado al servicio
    Serial.print(F("connecting MQTT client to: "));                                                       //Si no se encuentra conectado imprimir un mensake de error y de reconexion al servicio
    Serial.println(mqtt_server);                                                                            //Imprimir la direccion del servidor a donde se esta intentado conectar 
    char charBuf[30];
    String CID (clientId + chipId);
    CID.toCharArray(charBuf, 30);
    Serial.println(CID);  
    while (!!!client.connect(charBuf, "edin", "edin")) {                                                                    //Si no se encuentra conectado al servicio intentar la conexion con las credenciales Clientid, Metodo de autenticacion y el Tokeno password
      Serial.print(F("."));
      delay(500);                                                                                          //imprimir una serie de puntos mientras se da la conexion al servicio
      if (retry > 10){
        Serial.println(F("Intentos fallidos de conexion a MQTT excedidos! este nodo se reiniciara en 5 segundos"));
        retry = 0;
        delay(5000);
        ESP.restart();
      }
      retry++;
    }
  }
  Serial.println(F("conexion a MQtt Realizada con exito!"));                                              //dejar un espacio en la terminal para diferenciar los mensajes.
}


//*****************************************************************************Funcion encargada de subscribir el nodo a los servicio de administracion remota y de notificar los para metros configurables al mismo
void initManagedDevice() {
  if (client.subscribe(responseTopic)) {                                                             //Subscribir el nodo al servicio de mensajeria de respuesta
    Serial.println(F("subscribe to responses OK"));                                                       //si se logro la sibscripcion entonces imprimir un mensaje de exito
  }
  else {
    Serial.println(F("subscribe to responses FAILED"));                                                   //Si no se logra la subcripcion imprimir un mensaje de error
  }
  
  if (client.subscribe(rebootTopic)) {                                                                    //Subscribir el nodo al servicio de mensajeria de reinicio remoto
    Serial.println(F("subscribe to reboot OK"));                                                          //si se logro la sibscripcion entonces imprimir un mensaje de exito
  }
  else {
    Serial.println(F("subscribe to reboot FAILED"));                                                      //Si no se logra la subcripcion imprimir un mensaje de error                
  }
  
  if (client.subscribe(updateTopic)) {                                                                    //Subscribir el nodo al servicio de mensajeria de reinicio remoto
    Serial.println(F("subscribe to update OK"));                                                          //si se logro la sibscripcion entonces imprimir un mensaje de exito
  }
  else {
    Serial.println(F("subscribe to update FAILED"));                                                      //Si no se logra la subcripcion imprimir un mensaje de error         
  }
  
  const int capacity = JSON_OBJECT_SIZE(10);
  StaticJsonDocument<capacity> edin_json_config_doc;
  // create an object
  JsonObject object = edin_json_config_doc.to<JsonObject>();

  object["AliveUpdate:hrs"]   = isalivemsg_interval/3600000;
  object["MqttServer"]        = mqtt_server;
  char buf[16];
  sprintf(buf, "IP:%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
  Serial.println(String(buf));
  object["MqttPort"]          = mqtt_port;
  object["gloveCode"]         = chipId ;
  object["IpNodo"]            = String (buf);
  object["ClientID"]          = String(clientId);
    
  String output;
  size_t n = serializeJson(object, output);                                                                  //SAve CPU cycles by calculatinf the size.

  Serial.println(F("publishing device manageTopic metadata:"));
  Serial.println(output);

  if (!client.connected()) {
    mqttConnect();
  }
  if (client.publish(manageTopic, output.c_str(), n)) {
    Serial.println(F("device Publish ok"));
  }else {
    Serial.println(F("device Publish failed:"));
  }
}


//*****************************************************************************Inico de Wifi*****************************************************************************************
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Sipaddrs= WiFi.localIP().toString();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//********************************************************************************handle update**************************************************************************************

//**********************************************************************************Get Chip ID*********************************************************************************
void GloveID(){
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  
  Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getSdkVersion(), ESP.getChipRevision());
	Serial.printf("This chip has %d cores\n", ESP.getFlashChipSpeed());
  Serial.print("Chip ID: "); Serial.println(chipId);
  
	delay(3000);
}

//**********************************************************************************SETUP*****************************************************************************************
void setup() {
  Serial.begin(115200);                                                                                   //iniciamos la salida de consola para desplegar mensajes de sistema.
  //iniciamos desplegando informacion sobre el chip y la version de firmware. 
  Serial.println(F("")); 
  Serial.println(F("Inicializacion de programa de boton con identificacion RFID;"));
  Serial.println(F("Parametros de ambiente de funcionamiento:"));
  Serial.print(F("            CHIPID: "));
  GloveID();
  Serial.print(F("            HARDWARE: "));
  Serial.println(HardwareVersion);
  Serial.print(F("            FIRMWARE: "));
  Serial.println(FirmwareVersion);
  //si hubiera un servidot de NTP.
  /*
  Serial.print(F("            Servidor de NTP: "));
  Serial.println(ntpServerName);
  */
  Serial.print(F("            Servidor de MQTT: "));
  Serial.println(mqtt_server);
  Serial.print(F("            Client ID: "));
  Serial.println(chipId); 
  delay(500);
  setup_wifi();
  //--------------------------------------------------------------------------Connectando a servicio de MQTT
  Serial.println(F("Connecting to mqtt sevrer"));
  mqttConnect();                                                                                          //Conectamos al servicio de Mqtt con las credenciales provistas en el archivo "settings.h"
  Serial.println(F("Mqtt Connection Done!, sending Device Data"));
  //--------------------------------------------------------------------------Enviando datos de primera conexion
  initManagedDevice();                                                                                    //inciamos la administracion remota desde Bluemix
  Serial.println(F("Finalizing Setup"));                                                                  //enviamos un mensaje de depuracion
  fsm_state = STATE_IDLE; //inciar el estado del la maquina de stado finito
  // put your setup code here, to run once:
}

//**********************************************************************************LOOP*****************************************************************************************
void is_the_question_valid (int numero1, int numero2, String operador){
  if (numero1 < 1000000 && numero2 <1000000){
    
    enum Operaciones Signo ;

    if(operador == "+")
      Signo = PLUS;
    if(operador == "*")
      Signo = BY;
    if(operador == "/")  
      Signo = DIVIDED;
    else Signo = UNDEF;

    int respuesta_valida = 0;

    switch (Signo)
    {      
      case UNDEF: break;
      case PLUS:
        respuesta_valida = numero1 + numero2;
        if (respuesta_valida < 1000000) isvalid = true;
      break;

      case BY:
        respuesta_valida = numero1 * numero2;
        if (respuesta_valida < 100000) isvalid = true;
        break;
      case DIVIDED:
        respuesta_valida = numero1 / numero2;
        if (respuesta_valida < 1000000) isvalid = true;
        break;
    }
  }else isvalid = false;
}

void translate_question_to_vibartions(){
  if (isvalid == true){
    Serial.println(F("we can process a valid transacrion:"));
  }else
  Serial.println(F("no valid treansaction to process"));
  
}

void publishoperation_respuesta(int idoperacion, int idguante, int repuesta){
  //String response = ("{    "idOperation":12,    "gloveCode":"1941238-1458400",    "answer":2 })";
  const int capacity = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<capacity> edin_json_response_doc;
  // create an object
  JsonObject object = edin_json_response_doc.to<JsonObject>();

  object["idOperation"]   = idoperacion;
  object["gloveCode"]     = idguante;
  object["answer"]        = repuesta;
      
  String output;
  size_t n = serializeJson(object, output);                                                                  //SAve CPU cycles by calculatinf the size.

  Serial.println(F("publishing device response to server:"));
  Serial.println(output);

  if (!client.connected()) {
    mqttConnect();
  }
  if (client.publish(responseTopic, output.c_str(), n)) {
    Serial.println(F("device Publish ok"));
  }else {
    Serial.println(F("device Publish failed:"));
  }


}

void loop() {
  // put your main code here, to run repeatedly:
  //Iniciaremos una Maquina de estados por mediode un chip eso evitara que la rutina pueda quedarse en un loop continuo.
  switch(fsm_state){                                                                                      //Iniciamos el switch case

  case STATE_IDLE:                                                                                        //Que debe hacer la maquina cuando esta en estado de IDLE
  //verificar que el cliente de Conexion al servicio se encuentre conectado
  if (!client.connected()) {
      mqttConnect();
  }

  #if defined (Izquierdo)
    if ( mesgrecv == true){
      if (millis() - lastPregunta_Millis > intervalo_entre_preguntas){
        lastPregunta_Millis = millis();
        Serial.println("guante Izquierdo recibio mensaje");
        fsm_state = STATE_PREGUNTA;
      }
    }
  #else

  #endif

  client.loop();
  break;

  case STATE_PREGUNTA:
  //Get the Question 
  //Validate the question
  //translate the question to vibrations
  //send to state awating the answear 
  Serial.println(F("Pregunta recibida:"));
  is_the_question_valid(number1, number2, String(operation));
  translate_question_to_vibartions();
  mesgrecv = false;
  fsm_state = STATE_TRANSMIT_RESPUESTA; 
  break;

  case STATE_TRANSMIT_RESPUESTA: 
  //Si se presiono el boton
  //Check connection
  //Send the data
  Serial.println(F("BOTON DATA SENT"));
  publishoperation_respuesta( idOperation, gloveCode, answer);
  fsm_state = STATE_IDLE;
  break; 
  }
}