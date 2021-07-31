#include <Arduino.h>
#include "CubeCell_NeoPixel.h"
#include "LoRa_APP.h"

CubeCell_NeoPixel led_pixels(1, RGB, NEO_GRB + NEO_KHZ800);
#define timetillwakeup 10000
static TimerEvent_t sleep;
static TimerEvent_t wakeUp;
uint8_t lowpower=0;

// set led color n - color 1: red, 2: green, 3:blue i - intensity
void setColor(uint8_t n, uint8_t i) {
  if (n==1) {
    led_pixels.setPixelColor(0, led_pixels.Color(i, 0, 0));
  } else if (n==2) {
    led_pixels.setPixelColor(0, led_pixels.Color(0, i, 0));
  } else if (n==3) {
    led_pixels.setPixelColor(0, led_pixels.Color(0, 0, i));
  }
  led_pixels.show();   // Send the updated pixel colors to the hardware.
  delay(20);       // Pause before next pass through loop
}

// activate low power mode
void onSleep()
{
  Serial.println("Going into lowpower mode");
  delay(10); // wait for everything to be printed
  lowpower=1;
  // wake up from timer
  TimerSetValue( &wakeUp, timetillwakeup );
  TimerStart( &wakeUp );
}
void onWakeUp()
{
  Serial.println("Woke up!");
  lowpower=0;
  digitalWrite(Vext,LOW); // activate Vext, also activates LED
}

void setup() {
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext,LOW); // activate Vext, also activates LED
  Serial.begin(115200);
  led_pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  led_pixels.clear(); // Set all pixel colors to 'off'
  
  Radio.Sleep();      // don't need radio
  TimerInit( &wakeUp, onWakeUp );
}

void loop() {
  int16_t i=0;
  if(lowpower){
    lowPowerHandler();
  }
  for (int n=1; n <=3; n++) {
    i = 1;
    int8_t dir = 1;
    while (i > 0) {
      setColor(n, i);
      i+=dir*10;
      if (i > 255) {
        i = 255;
        dir = -1;
      }
    }
  }
  led_pixels.clear(); // Set all pixel colors to 'off'
  led_pixels.show();
  digitalWrite(Vext,HIGH); // switch of Vext circuit (also LED power)
  onSleep();
}