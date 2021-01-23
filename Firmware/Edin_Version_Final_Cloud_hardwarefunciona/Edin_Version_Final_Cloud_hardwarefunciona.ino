/**Attached interrupts
  *GPIO27   --->    SW_PUSH_MENIQUE
  *GPIO15   --->    SW_PUSH_ANULAR
  *GPIO14   --->    SW_PUSH_MEDIO
  *GPIO04   --->    SW_PUSH_INDICE
  ***********
  *PWM MOTORS 
  * GPIO23    --->    MTR_5
  * GPIO19    --->    MTR_4
  * GPIO18    --->    MTR_3
  * GPIO05    --->    MTR_2
  * GPIO32    --->    MTR_1
  * GPIO33    --->    ENABLE
  * 
   */
  

// PubNub MQTT example using ESP32.
#include <WiFi.h>
#include <ArduinoJson.h>                                                                                  //----------------------------JSON Arduino Library
#include <PubSubClient.h>
#include <TinyPICO.h>
#include "settings.h"

TinyPICO tp = TinyPICO();

//*****************************************************************************Definicion de Variables Globales
String Sipaddrs = "000.000.000.000";
uint32_t chipId = 0;                                                                                      //variable donde se alamacena el identificador de cliente para el servicio de MQTT (OJO Este debe ser un identificador unico para cada dispositivo fisico sino se reiniciara el servidor MQTT)
int retry = 0;                                                                                            //variable donde se almacenan los intentos de conexion fallidos
char id_de_operacion[20];

//******************************************************************************FSM Settings
bool mesgrecv = false;
static enum {STATE_IDLE, STATE_PREGUNTA, STATE_TRANSMIT_RESPUESTA} fsm_state = STATE_IDLE;

//******************************************************************************Variables de mensaje desde concentrador MQTT para motores. 

int motor_channel, motor_time, motor_effect;

int idOperation       = 0;
int idModule          = 0;
int gloveCode         = 0;
int answer            = 0;

//******************************************************************************Variables de Botones. 
struct boton {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  volatile bool pressed;
};

boton btn_4 = {4 ,0, false};
boton btn_3 = {14, 0, false};
boton btn_2 = {15, 0, false};
boton btn_1 = {27, 0, false};

void IRAM_ATTR isr_btn1(){
  btn_1.numberKeyPresses += 1;
  btn_1.pressed = true;
}

void IRAM_ATTR isr_btn2(){
  btn_2.numberKeyPresses += 1;
  btn_2.pressed = true;
}

void IRAM_ATTR isr_btn3(){
  btn_3.numberKeyPresses += 1;
  btn_3.pressed = true;
}

void IRAM_ATTR isr_btn4(){
  btn_4.numberKeyPresses += 1;
  btn_4.pressed = true;
}


//*****************************************************************************Variables de Motores

// Pin para inicio y frenado de Motores
int enable1Pin = 33; 
volatile bool EnablePinState;

 
// Propiedades de PWM que funcionan ocn driver y motores. 
const int freq = 3000;
const int resolution = 8;

uint16_t  dutyCyclefull = 255;
uint16_t  dutyCycleHalf = 95;
uint16_t  dutyCycleLow = 15;
uint16_t  initDutyCycle = 0;

uint16_t  clickTime = 0;

//******************************************************************************Estructura de puertos a Motores 
struct vmotor {
  const uint8_t motorPin;     //Puerto fisico al que se encuntra cableado el puerto de PWM del Chip DRV2603
  const uint8_t pwmChannel;  //Canal logico de comunicaciones para el puerto
};

//Mapa de asignacion de puertos y canales para PWM ver esquematico para confirmar puertos.

vmotor MTR_5 = {23, 4}; //GPIO23 canal 4
vmotor MTR_4 = {19, 3}; //GPIO19 canal 3
vmotor MTR_3 = {18, 2}; //GPIO18 canal 2
vmotor MTR_2 = {5,  1}; //GPIO5 canal 1
vmotor MTR_1 = {32, 0}; //GPIO32 canal 0

//*******************************************************************************Funcion de PWM para puerto 32 y 33 del ESP32
void aPinMode(int pinNum, int pinDir) {
  // Enable GPIO32 or 33 as output. 
  if (pinNum == 32 || pinNum == 33) {
    uint64_t gpioBitMask = (pinNum == 32) ? 1ULL<<GPIO_NUM_32 : 1ULL<<GPIO_NUM_33;
    gpio_mode_t gpioMode = (pinDir == OUTPUT) ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = gpioMode;
    io_conf.pin_bit_mask = gpioBitMask;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
  } else pinMode(pinNum, pinDir);
}


void operation (byte* payloadrsp){
  Serial.println(F("mensaje recibido de topico operacion"));
  tp.DotStar_SetPixelColor( 0xFFC900 );
  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> edin_operation_json;
  
  DeserializationError err = deserializeJson(edin_operation_json, payloadrsp);
  
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    return;
  }

  strlcpy(id_de_operacion, edin_operation_json["idOperation"] | "idOperation", 20);
  
  Serial.print("idOperation: ");
  Serial.println(id_de_operacion);

  tp.DotStar_Clear();
  fsm_state = STATE_PREGUNTA;       
}

void callback(char* topic, byte* payload, unsigned int length) {
  String payload_buff;
  for (int i=0;i<length;i++) {
    payload_buff = payload_buff+String((char)payload[i]);
  }
  Serial.println(payload_buff); // Print out messages.
  
  if (strcmp (rebootTopic, topic) == 0) {                                                                 //verificar si el topico conicide con el Topico rebootTopic[] definido en el archivo settings.h local
    Serial.println(F("Rebooting in 5 seconds..."));                                                                    //imprimir mensaje de Aviso sobre reinicio remoto de unidad.
    delay(5000);
    ESP.restart();                                                                                          //Emitir comando de reinicio para ESP32
  }
  
  if (strcmp (operationTopic, topic) == 0) {                                                                 //verificar si el topico conicide con el Topico updateTopic[] definido en el archivo settings.h local
    operation(payload);                                                                                //enviar a la funcion handleUpdate el contenido del mensaje para su parseo.
  }   
}


WiFiClient MQTTclient;
PubSubClient client(mqttServer, mqttPort, callback, MQTTclient);


long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect(clientId,"edin","edin")) {
    client.subscribe(rebootTopic); // Subscribe to channel.
    client.subscribe(operationTopic); // Subscribe to channel.    
  }
  return client.connected();
}

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
void configuration_json (){
  const int capacity = JSON_OBJECT_SIZE(10);
  StaticJsonDocument<capacity> edin_json_config_doc;
  // create an object
  JsonObject object = edin_json_config_doc.to<JsonObject>();

  object["AliveUpdate:hrs"]   = isalivemsg_interval/3600000;
  object["MqttServer"]        = mqttServer;
  char buf[16];
  sprintf(buf, "IP:%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
  Serial.println(String(buf));
  object["MqttPort"]          = mqttPort;
  object["gloveCode"]         = chipId ;
  object["IpNodo"]            = String (buf);
  object["ClientID"]          = String(clientId);
    
  String output;
  size_t n = serializeJson(object, output);                                                                  //SAve CPU cycles by calculatinf the size.
  Serial.println(F("publishing device manageTopic metadata:"));
  Serial.println(output);
  if (!client.connected()) {
    reconnect();
  }
  if (client.publish(responseTopic, output.c_str(), n)) {
    Serial.println(F("device Publish ok"));
  }else {
    Serial.println(F("device Publish failed:"));
  }

}

void setup() {
  Serial.begin(115200);
  //Setting up Button 1
  Serial.print("Setting pin button:");
  pinMode(btn_1.PIN, INPUT_PULLUP);
  attachInterrupt(btn_1.PIN, isr_btn1, CHANGE);
  //Setting up Button #2
  Serial.print("Setting pin button:");
  Serial.println(btn_2.PIN);
  pinMode(btn_2.PIN, INPUT_PULLUP);
  attachInterrupt(btn_2.PIN, isr_btn2, FALLING);
  //Setting up Button #3
  Serial.print("Setting pin button:");
  Serial.println(btn_3.PIN);
  pinMode(btn_3.PIN, INPUT_PULLUP);
  attachInterrupt(btn_3.PIN, isr_btn3, FALLING);
  //Button #4
  Serial.print("Setting pin button:");
  Serial.println(btn_4.PIN);
  pinMode(btn_4.PIN, INPUT_PULLUP);
  attachInterrupt(btn_4.PIN, isr_btn4, FALLING);
  
  tp.DotStar_SetPixelColor( 255, 128, 0 ); //<===================================================saber que color?
  //-------------------------------------------------------------------testing the setting
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
  Serial.println(mqttServer);
  Serial.print(F("            Client ID: "));
  Serial.println(chipId); 
  delay(500);
  //-----------------------------------------------------------------------incializando puertos fisicos
  // SET ENABLE PIN AND PRINT ITS STATE
  Serial.print("Setting Enable PIN to:");
  Serial.println(enable1Pin);
  pinMode (enable1Pin, OUTPUT);
  digitalWrite(enable1Pin, LOW);
  Serial.print("Setting Enable PIN to:");
  EnablePinState = digitalRead(enable1Pin);
  Serial.println(EnablePinState);
  delay(100);

  //SET PWM PIN AND CONFIGURE INITIAL PARAMETERS:
  
  Serial.print("Setting PWM PIN TO:");
  Serial.println(MTR_5.motorPin);
  ledcAttachPin(MTR_5.motorPin, MTR_5.pwmChannel);
  delay(100);
  Serial.println(MTR_4.motorPin);
  ledcAttachPin(MTR_4.motorPin, MTR_4.pwmChannel);
  delay(100);
  Serial.println(MTR_3.motorPin);
  ledcAttachPin(MTR_3.motorPin, MTR_3.pwmChannel);
  delay(100);
  Serial.println(MTR_2.motorPin);
  ledcAttachPin(MTR_2.motorPin, MTR_2.pwmChannel);
  delay(100);
  Serial.println(MTR_1.motorPin);
  ledcAttachPin(MTR_1.motorPin, MTR_1.pwmChannel);
  delay(100);

  // attach the channel to the GPIO to be controlled
  Serial.print("Setting PWM PIN CHANNEL TO:");
  Serial.println(MTR_5.pwmChannel);
  Serial.print("Setting PWM PIN CHANNEL TO:");
  Serial.println(MTR_4.pwmChannel);
  Serial.print("Setting PWM PIN CHANNEL TO:");
  Serial.println(MTR_3.pwmChannel);
  Serial.print("Setting PWM PIN CHANNEL TO:");
  Serial.println(MTR_2.pwmChannel);
  Serial.print("Setting PWM PIN CHANNEL TO:");
  Serial.println(MTR_1.pwmChannel);
  Serial.print("Setting PWM PIN FREQ TO:");
  Serial.println(freq);
  Serial.print("Setting PWM PIN RESOLUTION TO:");
  Serial.println(resolution);
  ledcSetup(MTR_5.pwmChannel, freq, resolution);
  delay(50); 
  ledcSetup(MTR_4.pwmChannel, freq, resolution);
  delay(50); 
  ledcSetup(MTR_3.pwmChannel, freq, resolution);
  delay(50); 
  ledcSetup(MTR_2.pwmChannel, freq, resolution);
  delay(50); 
  ledcSetup(MTR_1.pwmChannel, freq, resolution);
  delay(50);  
  //SET THE INITIAL DUTY CICLY TO 0 
  Serial.print("Setting PWM Initial dutycycle TO:");
  Serial.println(initDutyCycle);
  ledcWrite(MTR_5.pwmChannel, initDutyCycle);
  delay(50); 
  ledcWrite(MTR_4.pwmChannel, initDutyCycle);
  delay(50); 
  ledcWrite(MTR_3.pwmChannel, initDutyCycle);
  delay(50); 
  ledcWrite(MTR_2.pwmChannel, initDutyCycle);
  delay(50);
  ledcWrite(MTR_1.pwmChannel, initDutyCycle);
  delay(50);
    
  //-----------------------------------------------------------------------Conecting to WIFI
  Serial.println("Attempting to connect...");
  WiFi.begin(ssid, password); // Connect to WiFi.
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Couldn't connect to WiFi.");
      while(1) delay(100);
  }
  lastReconnectAttempt = 0;
  Serial.println(F("Finalizing Setup"));                                                                  //enviamos un mensaje de depuracion
  configuration_json();
  tp.DotStar_Clear();
  fsm_state = STATE_IDLE; //inciar el estado del la maquina de stado finito
  // put your setup code here, to run once:
}

//----------------------------------------------------------------------------------------------------//Fundiones de Motores
void enableMotors(int start_enableMotor_pin){
  tp.DotStar_SetPixelColor( 255, 255, 255   );
  Serial.println("enable pin HIGH");
  digitalWrite(start_enableMotor_pin, HIGH);
}

void breakMotors(int stop_enableMotor_pin) {
  tp.DotStar_SetPixelColor( 255,0, 255   );
  Serial.println("enable pin LOW");
  digitalWrite(stop_enableMotor_pin, LOW); 
}

void rampUp(int up_motor_channel){
  enableMotors(enable1Pin);
  tp.DotStar_SetPixelColor( 0, 255, 255   );
  for(int dutyCycle = 190; dutyCycle <= 255; dutyCycle++){
    // changing the LED brightness with PWM
    Serial.print("Set dutyCycle:");
    Serial.println(dutyCycle);
    // changing the LED brightness with PWM
    ledcWrite(up_motor_channel, dutyCycle);
    delay(10);
  }
  breakMotors(enable1Pin);     
}

void rampDown(int down_motor_channel){
  enableMotors(enable1Pin);
  tp.DotStar_SetPixelColor( 255, 255, 0  );
  for(int dutyCycle = 255; dutyCycle >= 100; dutyCycle--){
    // changing the LED brightness with PWM
    Serial.print("Set dutyCycle:");
    Serial.println(dutyCycle);
    // changing the LED brightness with PWM
    ledcWrite(down_motor_channel, dutyCycle);
    delay(10);
  }
  breakMotors(enable1Pin);    
}

void motorClick(int motor_channel, int motor_time){
  enableMotors(enable1Pin);
  Serial.print("Click Motor channel:");
  Serial.println(motor_channel);
  Serial.print(" for time in ms:");
  Serial.println(motor_time);
  delay(100);
  ledcWrite(motor_channel, dutyCyclefull);
  delay(motor_time);
  ledcWrite(motor_channel, dutyCycleHalf);
  breakMotors(enable1Pin);
}

void motorPWM_0(int motor_channel){
  Serial.print("PWM  to 0 for Motor channel:");
  Serial.println(motor_channel);
  enableMotors(enable1Pin);
  ledcWrite(motor_channel, initDutyCycle);
  delay(100);
  breakMotors(enable1Pin);
}

void findemensaje(){
  enableMotors(enable1Pin);
  ledcWrite(MTR_4.pwmChannel, dutyCycleHalf);
  ledcWrite(MTR_3.pwmChannel, dutyCycleHalf);
  ledcWrite(MTR_2.pwmChannel, dutyCycleHalf);
  ledcWrite(MTR_1.pwmChannel, dutyCycleHalf);
  delay(100);
  breakMotors(enable1Pin);
  delay(50);
  enableMotors(enable1Pin);
  ledcWrite(MTR_4.pwmChannel, dutyCycleHalf);
  ledcWrite(MTR_3.pwmChannel, dutyCycleHalf);
  ledcWrite(MTR_2.pwmChannel, dutyCycleHalf);
  ledcWrite(MTR_1.pwmChannel, dutyCycleHalf);
  breakMotors(enable1Pin);  
}

//----------------------------------------------------------------------------------------------------------------------------------------//Funciones de MQTT
void check_for_connection (){
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) { // Try to reconnect.
      lastReconnectAttempt = now;
      if (reconnect()) { // Attempt to reconnect.
        lastReconnectAttempt = 0;
      }
    }
  }else { // Connected.
    client.loop();
  }
}
//----------------------------------------------------------------------------------------------------------------------------------------//funcion para traducir mensaje a vibraciones.
void traducir_a_vibraciones(int pwm_motor_channel, int pwm_motor_time_ms, int pwm_motor_effect){
  Serial.println(F("DEBUG: empezandotraduccion a vibraciones"));
  if(pwm_motor_effect = 1){
    Serial.println(F("DEBUG: el mensaje contenia un rampup"));
    rampUp(pwm_motor_channel);
  }
  if(pwm_motor_effect = 2){
    Serial.println(F("DEBUG: el mensaje contenia un rampDown"));
    rampDown(pwm_motor_channel);
  }
  if(pwm_motor_effect = 3){
    Serial.println(F("DEBUG: el mensaje contenia un motorClick"));
    motorClick(pwm_motor_channel, pwm_motor_time_ms);
  }
  else if (pwm_motor_effect = 4){
    Serial.println(F("DEBUG: eso era toda la pregunta find de mensaje"));
    findemensaje();
  }
  
  Serial.println(F("DEBUG: Cambiando de estado de pregunta a respuesta"));
  fsm_state = STATE_TRANSMIT_RESPUESTA;
}

void capturar_respuesta_de_botones(){
  Serial.println(F("capturando la respuesta:"));
  delay(100);

}

void publicar_la_respuesta_a_servidor(int idoperacion, int idguante, int repuesta){
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
    check_for_connection();
  }
  if (client.publish(responseTopic, output.c_str(), n)) {
    Serial.println(F("device Publish ok"));
  }else {
    Serial.println(F("device Publish failed:"));
  }
}

//************************************************************************************************************************************* SETUP ***************************************************************************************
void loop() {
  switch(fsm_state){                                                                                         //Iniciamos el switch case
    
    case STATE_IDLE:                                                                                        //Que debe hacer la maquina cuando esta en estado de IDLE
         if (millis() - last_Case_Status_Millis > intervalo_Case_Status_Millis){
          last_Case_Status_Millis = millis();
          Serial.print(F("fsm_state: "));
          Serial.println(fsm_state);
          Serial.println(F(""));
          Serial.print(F("message_received_status: "));
          Serial.println(mesgrecv);
          Serial.println(F(""));
         }
         
        
        
        check_for_connection();
               
    break;
    
    case STATE_PREGUNTA:
         Serial.println(F("Switch case state Pregunta: PREGUNTA"));
         delay(500);
         traducir_a_vibraciones(motor_channel, motor_time, motor_effect);
                           
    break;
    
    case STATE_TRANSMIT_RESPUESTA:
        Serial.println(F("Switch case state: RESPUESTA"));
        //insertar a qui un While
        capturar_respuesta_de_botones(); 

        publicar_la_respuesta_a_servidor(idOperation, gloveCode, answer);
                
        fsm_state = STATE_IDLE;
    break;
    
    default:
        fsm_state = STATE_IDLE;
    break;
  }
}
