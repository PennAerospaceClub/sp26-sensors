#include <Wire.h>               //OLED/LUX
#include <SPI.h>                //SD/ATM
#include <SD.h>                 //SD
#include <Adafruit_SSD1306.h>   //OLED

#include "DFRobot_OxygenSensor.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include "Adafruit_SHT31.h"
#include <Wire.h>
#include <Adafruit_MPL115A2.h>


#define Oxygen_IICAddress ADDRESS_1
#define COLLECT_NUMBER  10             // collect number, the collection range is 1-100.
DFRobot_OxygenSensor oxygen;

SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

Adafruit_MPL115A2 mpl115a2;

//SD Card ==================================
#define chipSelect 53


void setup() {
  //Serial
  Serial.begin(115200); //Computer
  pinMode(53, OUTPUT);
  if (!SD.begin(53)) {
    Serial.println("initialization failed");
    return;
  }
  // dataFile = SD.open("datalog3.csv", FILE_WRITE);

  // oxygen
  while(!oxygen.begin(Oxygen_IICAddress)){
    Serial.println("I2c device number error !");
    delay(1000);
  }
  Serial.println("I2c connect success !");

  // air quality
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait three seconds for sensor to boot up!
  delay(3000);

  // If using serial, initialize it and set baudrate before starting!
  pmSerial.begin(9600);

  // There are 3 options for connectivity!
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("PM25 found!");

  // temp humidity
   if (! mpl115a2.begin()) {
    Serial.println("Sensor not found! Check wiring");
    while (1);
  }
  Serial.print("pressure (kPa), Temp (C), Voltage 1, Voltage 2, Oxygen Concentration (%vol), ");
  Serial.print("Concentration Units standard(PM 1.0), Concentration Units standard(PM 2.5), Concentration Units standard(PM 10.0), Concentration Units environmental(PM 1.0), Concentration Units environmental(PM 2.5), Concentration Units environmental(PM 10.0), Particles > 0.3um / 0.1L air, Particles > 0.5um / 0.1L air, Particles > 1.0um / 0.1L air, Particles > 2.5um / 0.1L air, Particles > 5.0um / 0.1L air, Particles > 10.0um / 0.1L air, PM2.5 AQI US, PM10.0 AQI US");
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:
  String str = mkdata();
  Serial.println(str);
  writeSD(str);
  delay(10000);
}
