// boolean initIMU(){
//   Serial.print(IMU.gyroscopeSampleRate());
//   return IMU.begin();
// }

// void readIMU(){
//   if (IMU.gyroscopeAvailable()) {
//     IMU.readGyroscope(xG, yG, zG);

//     Serial.print(xG);
//     Serial.print('\t');
//     Serial.print(yG);
//     Serial.print('\t');
//     Serial.println(zG);
//   }
//   if (IMU.accelerationAvailable()) {
//     IMU.readAcceleration(xA, yA, zA);

//     Serial.print(xA);
//     Serial.print('\t');
//     Serial.print(yA);
//     Serial.print('\t');
//     Serial.println(zA);
//   }
// }