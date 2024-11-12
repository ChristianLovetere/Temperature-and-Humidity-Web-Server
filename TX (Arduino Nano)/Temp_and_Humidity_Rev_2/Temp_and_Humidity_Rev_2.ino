#include <Wire.h>
#define SENSOR_ADDR 0x38

int lastReq = 0; //hold time since last request
int status = 0; //hold the status
int bits[7] = {0}; //the 7 bits of information returned on successful measurement
float humidity; //float for humidity (%)
float temperature; //float for temp (C)

void setup() {
  Wire.begin(); //start I2C connection
  Serial.begin(115200); //open serial for checking

  String listOfCommandsToSend[7] = {"AT\r\n", "AT+PARAMETER=7,9,4,7\r\n", "AT+NETWORKID=6\r\n",
                                    "AT+ADDRESS=0\r\n", "AT+PARAMETER?\r\n", "AT+NETWORKID?\r\n",
                                    "AT+ADDRESS?\r\n"};

  for (int i = 0; i < 7; i++){
    Serial.print(listOfCommandsToSend[i]);
    delay(125);
    while (Serial.available()){
      char c = Serial.read();
      Serial.print(c);
    }
    delay(125);
  Serial.flush();
  }
}

void loop() {
  delay(1000); //can only take reading once per second
  read(); //get reading
  //delay(100); //wait a little
  //Serial.println(getHumidity());
  //Serial.println(getTemperature());
  float hum = getHumidity();
  float temp = getTemperature();

  char humString[6];
  char tempString[6];

  dtostrf(hum, 4, 1, humString);
  dtostrf(temp, 4, 1, tempString);

  char dataPacket[15];
  strcpy(dataPacket, "$");
  strcat(dataPacket, humString);
  strcat(dataPacket, "%");
  strcat(dataPacket, tempString);
  strcat(dataPacket, "^");

  char buffer[35];

  char packet1[11] = "AT+SEND=1,";
  int packetLength = strlen(dataPacket);
  char packet2[4];
  
  itoa(packetLength, packet2, 10);
  strcat(packet2, ",");
  strcpy(buffer, packet1);
  strcat(buffer, packet2);
  strcat(buffer, dataPacket);
  strcat(buffer, "\r\n");

  Serial.println(buffer);
  //Serial.print(dataPacket);
  /*Serial.print(humString);
  Serial.print("\t");
  Serial.println(tempString);
  Serial.print("AT+SEND=0,5,HELLO\r\n");*/
  delay(1000);

  while (Serial.available()){
    char c = Serial.read();
    Serial.print(c);
  }
  Serial.flush();

  //Reset the module (if needed)
  digitalWrite(8, LOW);
  delay(100);
  digitalWrite(8, HIGH);
}

float getHumidity(){
  return humidity;
}

float getTemperature(){
  return temperature;
}

int readData(){ //after triggering a measurement, read the sensor's returned data
  const byte length = 7;
  int bytes = Wire.requestFrom(SENSOR_ADDR, (int)length);

  bool allZero = true;
  for (int i = 0; i < bytes; i++)
  {
    bits[i] = Wire.read();
    allZero = allZero && (bits[i] == 0);
  }
  lastReq = millis();
  return bytes;
}

int read(){ //top level function for updating temp and hum global variables.
  int status = requestData();
  if (status < 0)
    return status;
  long int start = millis();
  while (isMeasuring())
  {
    yield();
  }
  status = readData();
  if (status < 0)
    return status;

  return convert();
}

int convert(){ //turn the 6 bytes of hum and temp info into readable information
  status = bits[0];
  long int output = bits[1];
  output <<= 8;
  output += bits[2];
  output <<= 4;
  output +=(bits[3] >> 4);
  humidity = output / 1048576.0 * 100;

  output = (bits[3] & 0x0F);
  output <<= 8;
  output += bits[4];
  output <<= 8;
  output += bits[5];
  temperature = output / 1048576.0 * 200 - 50;
}
int requestData(){ //ask the sensor to take a measurement
  resetSensor();

  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(0xAC);
  Wire.write(0x33);
  Wire.write(0x00);
  int transmissionEnded = Wire.endTransmission();

  lastReq = millis();
  return transmissionEnded;
}

byte resetSensor(){ //reset sensor registers before taking a new measurement
  byte count = 0;
  if ((readStatus() & 0x18) != 0x18)
  {
    if (resetReg(0x1B))
      count++;
    if (resetReg(0x1C))
      count++;
    if (resetReg(0x1E))
      count++;
    delay(10);
  }
  return count;
}

byte readStatus(){ //take sensor status
  Wire.requestFrom(SENSOR_ADDR, 1);
  delay(1);
  return (byte) Wire.read();
}

bool resetReg(byte reg){ //reset requested register
  byte value[3];
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(reg);
  Wire.write(0x00);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0)
    return false;
  delay(5);

  int numBytes = Wire.requestFrom(SENSOR_ADDR, 3);
  for (int i = 0; i < numBytes; i++){
    value[i] = Wire.read();
  }
  delay(10);

  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(0xB0 | reg);
  Wire.write(value[1]);
  Wire.write(value[2]);
  if (Wire.endTransmission() != 0)
    return false;
  delay(5);
  return true;
}

bool isMeasuring(){ //return weather or not the sensor is in the process of taking a measurement
  return (readStatus() & 0x80) == 0x80;
}
