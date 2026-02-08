void writeSD(String dataString){
      File dataFile = SD.open("datalog4.csv", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
}

String mkdata(){
  // temp hum
  float pressureKPA = 0, temperatureC = 0;
  pressureKPA = mpl115a2.getPressure();
  temperatureC = mpl115a2.getTemperature();

  // solar cell
  float sensorValues[2] = {analogRead(A0), analogRead(A1)};
  float voltage[2] = {sensorValues[0]*5.0/1023, sensorValues[1]*5.0/1023};

  // oxygen
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);
  
  // air quality
  PM25_AQI_Data data;
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    // return;
  }

  String str = String(pressureKPA) + ", ";
  str += String(temperatureC) + ", " ; 
  str += String(voltage[0]) + ", " ; 
  str += String(voltage[1]) + ", "; 
  str += String(oxygenData) + ", " ; 
  str += String(data.pm10_standard) + ", "; 
  str += String(data.pm25_standard) + ", "; 
  str += String(data.pm100_standard) + ", " ; 
  str += String(data.pm10_env) + ", " ; 
  str += String(data.pm25_env) + ", " ; 
  str += String(data.pm100_env) + ", " ; 
  str += String(data.particles_03um) + ", "; 
  str += String(data.particles_05um) + ", " ; 
  str += String(data.particles_10um) + ", " ; 
  str += String(data.particles_25um) + ", " ; 
  str += String(data.particles_50um) + ", " ; 
  str += String(data.particles_100um) + ", " ; 
  str += String(data.aqi_pm25_us) + ", " ; 
  str += String(data.aqi_pm100_us) + "\n"; 
  
  return str;
}
