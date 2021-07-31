#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>

/*
 * set LoraWan_RGB to Active,the RGB active in loraWan
 * RGB red means sending;
 * RGB purple means joined done;
 * RGB blue means RxWindow1;
 * RGB yellow means RxWindow2;
 * RGB green means received done;
 */

/* OTAA para*/
uint8_t devEui[] = { 0x97, 0x1f, 0x8e, 0xed, 0x7c, 0x0b, 0xaa, 0xa1 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x16, 0x7e, 0x6c, 0x3b, 0xc2, 0x76, 0x04, 0x56, 0x22, 0x11, 0xa9, 0xaf, 0x74, 0x72, 0xef, 0x15};

/* ABP para*/
uint8_t nwkSKey[] = { 0x3b, 0x83, 0x30, 0x90, 0x8a, 0x81, 0xbc, 0x0e, 0x31, 0x7f, 0xbb, 0x1d, 0x1f, 0x56, 0x79, 0xe1};
uint8_t appSKey[] = { 0x16, 0x5a, 0xc8, 0x09, 0xa9, 0x31, 0xd1, 0x40, 0xe7, 0x69, 0x75, 0xa6, 0x30, 0xaa, 0x95, 0x43}; 
uint32_t devAddr =  ( uint32_t )0x01f20103;

/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = LORAMAC_REGION_EU868; //ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

float temperature, humidity, pressure, lux, co2, tvoc;
uint16_t baseline;
int count;
int maxtry = 50;

//BMP280 bmp;

/*!
   \brief   Prepares the payload of the frame
*/

static void prepareTxFrame( uint8_t port )
{
	/*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
	*appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
	*if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
	*if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
	*for example, if use REGION_CN470, 
	*the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
	*/
  /*
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(500);
  count = 0;
  bmp.begin();
  delay(500);
  bmp.setSampling(BMP280::MODE_NORMAL,    
                  BMP280::SAMPLING_X2,    
                  BMP280::SAMPLING_X16,  
                  BMP280::FILTER_X16,    
                  BMP280::STANDBY_MS_500); 
  float temp = bmp.readTemperature();
  pressure = (float)bmp.readPressure() / 100.0;
  Wire.end();
  while (pressure > 1190.0 && count < maxtry) {
    bmp.begin();
    delay(500);
    bmp.setSampling(BMP280::MODE_NORMAL,    
                    BMP280::SAMPLING_X2,     
                    BMP280::SAMPLING_X16,   
                    BMP280::FILTER_X16,   
                    BMP280::STANDBY_MS_500); 
    pressure = (float)bmp.readPressure() / 100.0;
    Wire.end();
    count++;
    delay(500);
  }
  if (pressure > 1190.0) {
    pressure = 0;
    Serial.println("BMP ERROR");
  }

  Wire.end();
  digitalWrite(Vext, HIGH);
  */
  uint16_t batteryVoltage = getBatteryVoltage();
  unsigned char *puc;

  temperature = 20.0;
  humidity = 33.0;
  puc = (unsigned char *)(&temperature);
  appDataSize = 10;
  appData[0] = puc[0];
  appData[1] = puc[1];
  appData[2] = puc[2];
  appData[3] = puc[3];

  puc = (unsigned char *)(&humidity);
  appData[4] = puc[0];
  appData[5] = puc[1];
  appData[6] = puc[2];
  appData[7] = puc[3];

  appData[8] = (uint8_t)(batteryVoltage >> 8);
  appData[9] = (uint8_t)batteryVoltage;

  Serial.print("T=");
  Serial.print(temperature);
  Serial.print("C, RH=");
  Serial.print(humidity);
  Serial.print(", BatteryVoltage:");
  Serial.println(batteryVoltage);
}


void setup() {
	Serial.begin(115200);
#if(AT_SUPPORT)
  Serial.println("AT support enabled");
	enableAt();
#endif
	deviceState = DEVICE_STATE_INIT;
  Serial.println("start join");
	LoRaWAN.ifskipjoin();
  Serial.println("setup done");
}

void loop()
{
  Serial.println("enter loop");
	switch( deviceState )
	{
		case DEVICE_STATE_INIT:
		{
#if(LORAWAN_DEVEUI_AUTO)
			LoRaWAN.generateDeveuiByChipID();
#endif
#if(AT_SUPPORT)
			getDevParam();
#endif
			printDevParam();
      Serial.println("init");
			LoRaWAN.init(loraWanClass,loraWanRegion);
			deviceState = DEVICE_STATE_JOIN;
			break;
		}
		case DEVICE_STATE_JOIN:
		{
      Serial.println("join");
			LoRaWAN.join();
			break;
		}
		case DEVICE_STATE_SEND:
		{
      Serial.println("send");
			prepareTxFrame( appPort );
			LoRaWAN.send();
			deviceState = DEVICE_STATE_CYCLE;
			break;
		}
		case DEVICE_STATE_CYCLE:
		{
      Serial.println("cycle");
			// Schedule next packet transmission
			txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
			LoRaWAN.cycle(txDutyCycleTime);
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
      Serial.println("sleep");
			LoRaWAN.sleep();
			break;
		}
		default:
		{
      Serial.println("set state to init");
			deviceState = DEVICE_STATE_INIT;
			break;
		}
	}
}
