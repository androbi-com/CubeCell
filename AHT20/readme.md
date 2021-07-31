# CubeCell with AHT20 sensor over I2C

This example reads and prints sensor data and then enters low power mode. 

## CubeCell – Dev-Board 

* homepage https://heltec.org/project/htcc-ab01/ 
* pinout: https://heltec.org/wp-content/uploads/2019/09/ab01pinout-1024x531.png

## Adafruit AHT20

* homepage: https://learn.adafruit.com/adafruit-aht20
* pinout: https://learn.adafruit.com/adafruit-aht20/pinouts

## Connect AHT20 board to MCU

Connect AHT20 board <-> CubeCell board as follows:

    SDA <-> SDA
    SCL <-> SCL
    GND <-> GND
    VIN <-> VEXT

The Adafruit board has 10k pullups onboard, so no external pullups needed. Program output:

    CubeCell AHT20 with low power mode
    AHT10 or AHT20 found
    Temperature: 23.31 degrees C
    Humidity: 33.55% rH
    Going into lowpower mode
    Woke up!
    Temperature: 23.31 degrees C
    Humidity: 33.55% rH
    Going into lowpower mode
    ..

## Power profiling

When the sensor and MCU are active I obtain a 20ms long peak with an average consumption 
of 11.3 mA. During deep sleep I measure 9.8 uA. These values would yield a battery life
of 7 years for a 1000mAh battery, when measuring every minute (see https://www.geekstips.com/battery-life-calculator-sleep-mode/).