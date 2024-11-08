#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11
 //SoftwareSerial serial1 = SoftwareSerial(rxPin,txPin);

void setup() {
  // put your setup code here, to run once:

  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  delay(2000);
  Serial.begin(115200);
  //Serial.end();
  
  //serial1.begin(115200);
  delay(1000);
  Serial.print("AT\r\n");
  Serial.flush();
  delay(1000);
  Serial.print("AT+PARAMETER=12,9,4,7\r\n");
  Serial.flush();
  delay(1000);
  Serial.print("AT+NETWORKID=6\r\n");
  Serial.flush();
  delay(1000);
  Serial.print("AT+ADDRESS=1\r\n");
  Serial.flush();
  delay(1000);
  Serial.print("AT+PARAMETER?\r\n");
  Serial.flush();
  delay(1000);
  Serial.print("AT+NETWORKID?\r\n");
  Serial.flush();
  delay(1000);
  Serial.print("AT+ADDRESS?\r\n");
  Serial.flush();
  delay(1000);
  //Serial.end();
}

void loop() {
  // Send AT command
  Serial.print("AT+SEND=0,5,HELLO\r\n");

  // Wait for a response from the module
  delay(2000);  // Longer delay to allow processing

  // Read and print the response
  while (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);
  }
  Serial.flush();

  //Reset the module (if needed)
  digitalWrite(8, LOW);
  delay(100);
  digitalWrite(8, HIGH);
}