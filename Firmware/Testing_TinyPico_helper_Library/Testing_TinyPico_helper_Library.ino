#include <TinyPICO.h>

// Interval between internal temperature reads
unsigned long next_temp_read = 0;   // Next time step in milliseconds
uint8_t temp_read_interval = 1000;  // This is in milliseconds

unsigned long lastMillis_BAT;
unsigned long lastMillis_LED;

// Initialise the TinyPICO library
TinyPICO tp = TinyPICO();

void setup()
{
  Serial.begin(115200);

            // Not used
}

void loop()
{
  if (millis() - lastMillis_BAT > 60000) {
    lastMillis_BAT = millis();
    float battery =  tp.GetBatteryVoltage();
    bool ischarging = tp.IsChargingBattery();
    Serial.print(F("voltaje de la bateria: "));
    Serial.println( battery);
    Serial.print(F("La bateria esta cargando"));
    Serial.println(ischarging);
   }
   
   if (millis() - lastMillis_LED > 30000) {
    lastMillis_LED = millis();
    Serial.println("cambiando de Color de LED");
    tp.DotStar_CycleColor(25);
   }
    
 
    // Cycle the DotStar colour every 25 miliseconds
   

    // You can set the DotStar colour directly using r,g,b values
    // tp.DotStar_SetPixelColor( 255, 128, 0 );

    // You can set the DotStar colour directly using a uint32_t value
    // tp.DotStar_SetPixelColor( 0xFFC900 );

    // You can aclear the DotStar too
    // tp.DotStar_Clear();

    // To power down the DotStar for deep sleep you call this
    // tp.DotStar_SetPower( false );

}
