#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

//wiring is vin to 3.3v, gnd to gnd, sda to sda, scl to scl

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Wire.begin();           // IMPORTANT on Uno R4
  delay(1000);            // let I2C settle

  if (!bno.begin()) {
    Serial.println("No BNO055 detected!");
    while (1);
  }

  delay(1000);            // let sensor boot

  bno.setMode((adafruit_bno055_opmode_t)0x0C);
  delay(100);

  bno.setExtCrystalUse(true);

  Serial.println("BNO055 ready!");
}



void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

  Serial.print("Heading: ");
  Serial.print(euler.x());
  Serial.print("  Roll: ");
  Serial.print(euler.y());
  Serial.print("  Pitch: ");
  Serial.println(euler.z());

  Serial.print("Accel X: ");
  Serial.print(accel.x());
  Serial.print(" Y: ");
  Serial.print(accel.y());
  Serial.print(" Z: ");
  Serial.println(accel.z());

  Serial.println("-----");
  delay(500);
}
