#include "LoRaWanMinimal_APP.h"
#include "Arduino.h"
#define ACTIVE_REGION LORAMAC_REGION_EU868

// OTAA parameters
static uint8_t devEui[] = { 0x97, 0x1f, 0x8e, 0xed, 0x7c, 0x0b, 0xaa, 0xa1 };
static uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t appKey[] = { 0x16, 0x7e, 0x6c, 0x3b, 0xc2, 0x76, 0x04, 0x56, 0x22, 0x11, 0xa9, 0xaf, 0x74, 0x72, 0xef, 0x15};

uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

static uint8_t counter=0;

///////////////////////////////////////////////////
//Some utilities for going into low power mode
TimerEvent_t sleepTimer;
//Records whether our sleep/low power timer expired
bool sleepTimerExpired;

static void wakeUp()
{
  sleepTimerExpired=true;
}

static void lowPowerSleep(uint32_t sleeptime)
{
  delay(100);
  sleepTimerExpired=false;
  TimerInit( &sleepTimer, &wakeUp );
  TimerSetValue( &sleepTimer, sleeptime );
  TimerStart( &sleepTimer );
  //Low power handler also gets interrupted by other timers
  //So wait until our timer had expired
  while (!sleepTimerExpired) lowPowerHandler();
  TimerStop( &sleepTimer );
}

void setup() {
	Serial.begin(115200);

  // if (ACTIVE_REGION==LORAMAC_REGION_AU915) {
  //   LoRaWAN.setSubBand2(); //TTN uses sub-band 2 in AU915
  // }
 
  LoRaWAN.begin(LORAWAN_CLASS, ACTIVE_REGION);
  
  //Enable ADR
  LoRaWAN.setAdaptiveDR(true);

  while (1) {
    Serial.print("Joining... ");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined()) {
      //In this example we just loop until we're joined, but you could
      //also go and start doing other things and try again later
      Serial.println("JOIN FAILED! Sleeping for 30 seconds");
      lowPowerSleep(30000);
    } else {
      Serial.println("JOINED");
      break;
    }
  }
}

//
void loop()
{
  //Counter is just some dummy data we send for the example
  counter++; 
  
  //In this demo we use a timer to go into low power mode to kill some time.
  //You might be collecting data or doing something more interesting instead.
  lowPowerSleep(15000);  

  //Now send the data. The parameters are "data size, data pointer, port, request ack"
  Serial.printf("\nSending packet with counter=%d\n", counter);
  //Here we send confirmed packed (ACK requested) only for the first five (remember there is a fair use policy)
  //bool requestack=counter<5?true:false;
  bool requestack = true;
  if (LoRaWAN.send(1, &counter, 1, requestack)) {
    Serial.println("Send OK");
  } else {
    Serial.println("Send FAILED");
  }
}

///////////////////////////////////////////////////
//Example of handling downlink data
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  Serial.printf("Received downlink: %s, RXSIZE %d, PORT %d, DATA: ",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++) {
    Serial.printf("%02X",mcpsIndication->Buffer[i]);
  }
  Serial.println();
}