#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define SD_CS 10   // Chip Select pin for SD module

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
File dataFile;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Wire.begin();           
  delay(1000);            

  if (!bno.begin()) {
    Serial.println("No BNO055 detected!");
    while (1);
  }

  delay(1000);            
  bno.setMode((adafruit_bno055_opmode_t)0x0C);
  delay(100);
  bno.setExtCrystalUse(true);

  Serial.println("BNO055 ready!");

  // ---------- SD CARD INIT ----------
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed!");
    while (1);
  }
  Serial.println("SD init done.");

  // Create/Open file and write header
  dataFile = SD.open("imu.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("time_ms,heading,roll,pitch,accel_x,accel_y,accel_z");
    dataFile.close();
    Serial.println("File ready.");
  } else {
    Serial.println("Error opening imu.csv");
  }
}

void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

  unsigned long t = millis();

  // -------- SERIAL OUTPUT --------
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

  // -------- SD OUTPUT --------
  dataFile = SD.open("imu.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(t);
    dataFile.print(",");
    dataFile.print(euler.x());
    dataFile.print(",");
    dataFile.print(euler.y());
    dataFile.print(",");
    dataFile.print(euler.z());
    dataFile.print(",");
    dataFile.print(accel.x());
    dataFile.print(",");
    dataFile.print(accel.y());
    dataFile.print(",");
    dataFile.println(accel.z());
    dataFile.close();
  } else {
    Serial.println("Error writing to imu.csv");
  }

  delay(500);
}
