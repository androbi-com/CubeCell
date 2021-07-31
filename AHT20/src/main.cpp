#include <Arduino.h>
#include <Adafruit_AHTX0.h>
//#include "CubeCell_NeoPixel.h" // seems to be needed when using LoRa_APP.h
//#include "LoRa_APP.h"

#define timetillwakeup 10000

Adafruit_AHTX0 aht;
static TimerEvent_t wakeUp;
uint8_t lowpower=0;

// activate low power mode
void onSleep()
{
  Serial.println("Going into lowpower mode");
  delay(10); // wait for everything to be printed
  lowpower=1;
  // prepare wake up from timer
  TimerSetValue( &wakeUp, timetillwakeup );
  TimerStart( &wakeUp );
}

// timer wakeup callback
void onWakeUp()
{
  Serial.println("Woke up!");
  lowpower=0;
  digitalWrite(Vext,LOW); // activate Vext, also activates LED
}

// startup code
void setup() {
  // switch on VEXT
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext,LOW);

  // prepare Serial
  Serial.begin(115200);
  Serial.println("CubeCell AHT20 with low power mode");

  // prepare sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  // setup timer
  // Radio.Sleep();      // don't need radio
  TimerInit( &wakeUp, onWakeUp );
}

// after startup and wakeup
void loop() {
  
  if (lowpower) { // go into deep sleep
    lowPowerHandler();
  }

  // read sensor data
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  
  // switch of VEXT and go to sleep
  digitalWrite(Vext,HIGH);
  onSleep();
}