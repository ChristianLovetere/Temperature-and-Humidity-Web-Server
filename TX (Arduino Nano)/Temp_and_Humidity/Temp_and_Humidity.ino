#include <Wire.h>
#define SENSOR_ADDR 0x38

byte sensorReg = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {

    byte hum_upper8;
    byte hum_middle8;
    byte hum_lower4_temp_upper4;
    byte temp_middle8;
    byte temp_lower8;
    byte crc;

  delay(150); //wait 100ms for startup

  //startup / status phase
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(0x71);
  delay(10);
  Wire.requestFrom(SENSOR_ADDR, 1); //request a byte from the sensor
  Wire.endTransmission(SENSOR_ADDR);
  sensorReg = readSensor(); //store req'd byte in sensorReg
  if ((sensorReg & 0x18) != 0x18){
    uint8_t value[3];
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0x1b);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission(SENSOR_ADDR);
    int bytes = Wire.requestFrom(SENSOR_ADDR, (uint8_t)3);
    for (int i = 0; i < bytes; i++)
      value[i] = Wire.read();
    delay(10);
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0xB0 | 0x1b);
    Wire.write(value[1]);
    Wire.write(value[2]);

    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0x1c);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission(SENSOR_ADDR);
    bytes = Wire.requestFrom(SENSOR_ADDR, (uint8_t)3);
    for (int i = 0; i < bytes; i++)
      value[i] = Wire.read();
    delay(10);
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0xB0 | 0x1c);
    Wire.write(value[1]);
    Wire.write(value[2]);

    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0x1e);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission(SENSOR_ADDR);
    bytes = Wire.requestFrom(SENSOR_ADDR, (uint8_t)3);
    for (int i = 0; i < bytes; i++)
      value[i] = Wire.read();
    delay(10);
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0xB0 | 0x1e);
    Wire.write(value[1]);
    Wire.write(value[2]);
  }
  Wire.endTransmission(SENSOR_ADDR);
  delay(15); //wait 10ms before requesting measurement

  Serial.println("left status phase");
  Serial.print(sensorReg);
  Serial.println(" should be 24");

  //take measurement phase
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(0xAC); //trigger measurement
  Wire.write(0x33);
  Wire.write(0x00);
  //wait 80 for measurement to finish
  delay(120);
  Wire.endTransmission(SENSOR_ADDR);

  Serial.print(sensorReg);
  Serial.println(" = sensorReg after waiting for measurement");
  while ((readSensor() & 0x80) != 0){
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.requestFrom(SENSOR_ADDR, 1);
    delay(1000);
    sensorReg = readSensor(); //get measurement
    Wire.endTransmission(SENSOR_ADDR);
    delay(10);
    Serial.println(readSensor, HEX);
  }
  Serial.println("measurement successful");
  Serial.print(sensorReg);
  Serial.println(" should be < 128");

  //read measurement phase
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.requestFrom(SENSOR_ADDR, 6);
  if(Wire.available()){
    //Serial.println(Wire.available());
    hum_upper8 = Wire.read();
    hum_middle8 = Wire.read();
    hum_lower4_temp_upper4 = Wire.read();
    temp_middle8 = Wire.read();
    temp_lower8 = Wire.read();
    crc = Wire.read();
  }
  Wire.endTransmission(SENSOR_ADDR);
  Serial.println("entering calculation section");

  unsigned long int hum_data = hum_lower4_temp_upper4 & 0xf0; //hum_data = llll0000
  hum_data = hum_data >> 4; //hum_data = 0000llll
  hum_data |= (hum_middle8 << 4); //hum_data = mmmmmmmmllll
  hum_data |= (hum_upper8 << 12); //hum_data = uuuuuuuummmmmmmmllll

  unsigned long int temp_data = hum_lower4_temp_upper4 & 0x0f; //temp_data = 0000uuuu
  temp_data = temp_data << 16;  //temp_data = uuuu0000000000000000
  temp_data |= (temp_middle8 << 8); //temp_data = uuuummmmmmmm00000000
  temp_data |= temp_lower8; //temp_data = uuuummmmmmmmllllllll

  long int humidity = (hum_data / 1048576) * 100;
  long int temp_celsius = ((temp_data / 1048576) * 200) - 50;
  //long int humidity = hum_data;
  //long int temp_celsius = temp_data;
  Serial.println(hum_data);
  Serial.println(humidity);
  Serial.println(temp_data);
  Serial.println(temp_celsius);

  delay(2000);
}

byte readSensor(){
  if (Wire.available()) { 
    return Wire.read();
  }
}