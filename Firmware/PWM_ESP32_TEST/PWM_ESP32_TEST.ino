// the number of the LED pin
const int MotorPin = 25;  // 25 corresponds to GPIO16

// setting PWM properties
const int freq = 5000;
const int MotorChannel = 0;
const int resolution = 8;
 
void setup(){
  Serial.begin(115200);
  // configure LED PWM functionalitites
  ledcSetup(MotorChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(MotorPin, MotorChannel);
  Serial.println("Finalizar SetUp");
}
 
void loop(){
  // increase the LED brightness
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    // changing the LED brightness with PWM
   ledcWrite(MotorChannel, dutyCycle);
    delay(15);
  }

  // decrease the LED brightness
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    ledcWrite(MotorChannel, dutyCycle);   
    delay(15);
  }
}
