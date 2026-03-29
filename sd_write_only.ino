#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define SD_CS 10
#define SDA_PIN A4  // Uno SDA
#define SCL_PIN A5  // Uno SCL

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
bool bnoOK = false;

// Manually clock SCL to unstick a locked I2C device
void unlockI2CBus() {
  Serial.println("Attempting I2C bus unlock...");
  
  pinMode(SCL_PIN, OUTPUT);
  pinMode(SDA_PIN, INPUT_PULLUP);

  // Clock out up to 9 pulses — enough to finish any stuck transaction
  for (int i = 0; i < 9; i++) {
    digitalWrite(SCL_PIN, HIGH); delay(5);
    digitalWrite(SCL_PIN, LOW);  delay(5);
    // If SDA goes high, slave released the bus
    if (digitalRead(SDA_PIN) == HIGH) {
      Serial.print("Bus released after ");
      Serial.print(i + 1);
      Serial.println(" pulses.");
      break;
    }
  }

  // Send a STOP condition to fully reset bus state
  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, LOW);  delay(5);
  digitalWrite(SCL_PIN, HIGH); delay(5);
  digitalWrite(SDA_PIN, HIGH); delay(5);

  // Hand pins back to Wire
  pinMode(SDA_PIN, INPUT);
  pinMode(SCL_PIN, INPUT);
  delay(100);
}

bool reinitBNO() {
  unlockI2CBus();

  Wire.end();
  delay(100);
  Wire.begin();
  delay(500);

  if (!bno.begin()) {
    Serial.println("BNO055 reinit failed!");
    return false;
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  Serial.println("BNO055 reinitialized OK.");
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // SD first — before I2C
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  Serial.print("Initializing SD...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed!");
  } else {
    Serial.println("SD init done.");
    File f = SD.open("imu.csv", FILE_WRITE);
    if (f) {
      f.println("time_ms,heading,roll,pitch,accel_x,accel_y,accel_z");
      f.close();
    }
  }

  // IMU after SD
  Wire.begin();
  delay(500);

  if (!bno.begin()) {
    Serial.println("No BNO055 detected — check wiring!");
    while(1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
  bnoOK = true;
  Serial.println("BNO055 ready!");

  // Wait for minimum calibration
  uint8_t sys = 0, gyro = 0, accel_cal = 0, mag = 0;
  Serial.println("Calibrating — rotate sensor in a figure-8...");
  while (sys == 0 || mag == 0) {
    bno.getCalibration(&sys, &gyro, &accel_cal, &mag);
    Serial.print("SYS:"); Serial.print(sys);
    Serial.print(" GYRO:"); Serial.print(gyro);
    Serial.print(" ACCEL:"); Serial.print(accel_cal);
    Serial.print(" MAG:"); Serial.println(mag);
    delay(500);
  }
  Serial.println("Calibrated! Logging started.");
}

void loop() {
  if (!bnoOK) {
    Serial.println("Attempting BNO055 recovery...");
    bnoOK = reinitBNO();
    if (!bnoOK) {
      delay(1000);
      return;
    }
  }

  // Read IMU
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  unsigned long t = millis();

  // Detect lockup
  bool allZero = (euler.x() == 0 && euler.y() == 0 && euler.z() == 0
               && accel.x() == 0 && accel.y() == 0 && accel.z() == 0);

  if (allZero) {
    Serial.println("All zero — flagging for recovery.");
    bnoOK = false;
    delay(500);
    return;
  }

  // Serial output
  uint8_t sys, gyro, accel_cal, mag;
  bno.getCalibration(&sys, &gyro, &accel_cal, &mag);
  Serial.print("CAL SYS:"); Serial.print(sys);
  Serial.print(" MAG:"); Serial.print(mag);
  Serial.print(" | Hdg:"); Serial.print(euler.x());
  Serial.print(" Roll:"); Serial.print(euler.y());
  Serial.print(" Pitch:"); Serial.print(euler.z());
  Serial.print(" | Ax:"); Serial.print(accel.x());
  Serial.print(" Ay:"); Serial.print(accel.y());
  Serial.print(" Az:"); Serial.println(accel.z());

  // SD write
  digitalWrite(SD_CS, LOW);
  File dataFile = SD.open("imu.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(t);         dataFile.print(",");
    dataFile.print(euler.x()); dataFile.print(",");
    dataFile.print(euler.y()); dataFile.print(",");
    dataFile.print(euler.z()); dataFile.print(",");
    dataFile.print(accel.x()); dataFile.print(",");
    dataFile.print(accel.y()); dataFile.print(",");
    dataFile.println(accel.z());
    dataFile.close();
  } else {
    Serial.println("SD write error");
  }
  digitalWrite(SD_CS, HIGH);

  delay(500);
}
