// Misc
#include <Wire.h>

// // UV
// #include "DEV_Config.h"
// #include "LTR390.h"
// UDOUBLE UV,ALS;

//oxygen
#include "DFRobot_OxygenSensor.h"

#define Oxygen_IICAddress ADDRESS_1
#define COLLECT_NUMBER  10             // collect number, the collection range is 1-100.
DFRobot_OxygenSensor oxygen;

// IMU
#include <MKRIMU.h>

// Temp & Humid
#include "Adafruit_SHT31.h"
bool enableHeater = false;
uint8_t loopCnt = 0;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
double sht31_temp;
double sht31_humidity;

void setup() {

  // // UV
  // delay(3000);
  // if(LTR390_Init() != 0){
  //     Serial.print("init err!!!");
  //     while(1);
  // }
  // LTR390_SetIntVal(5, 20);

  // oxygen
  Serial.begin(115200);
  while(!oxygen.begin(Oxygen_IICAddress)){
    Serial.println("I2c device number error !");
    delay(1000);
  }
  Serial.println("I2c connect success !");

  delay(500);

  // IMU
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in G's");
  Serial.println("X\tY\tZ");

  // temp/humid
  sht31.begin(0x44);
  Serial.println("temp:" + sht31.begin(0x44));
}

void loop() {
  // // UV
  // UV = LTR390_UVS();
  // Serial.print("UV = ");
  // Serial.println(UV);

  // oxygen
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);
  Serial.print(" oxygen concentration is ");
  Serial.print(oxygenData);
  Serial.println(" %vol");
  delay(500);

  // IMU
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);

    delay(500);
  }

  // temp/humid
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (!isnan(t)) {  // check if 'is not a number'
    sht31_temp = t;
  }

  if (!isnan(h)) {  // check if 'is not a number'
    sht31_humidity = h;
  }
  Serial.print("temp ");
  Serial.print(t);
  Serial.print("     ");
  Serial.print("humidity ");
  Serial.println(h);
  Serial.print("     ");
  delay(500);
}
