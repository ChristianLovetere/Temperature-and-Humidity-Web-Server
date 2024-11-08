void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //open serial for checking

  String listOfCommandsToSend[7] = {"AT\r\n", "AT+PARAMETER=7,9,4,7\r\n", "AT+NETWORKID=6\r\n",
                                    "AT+ADDRESS=1\r\n", "AT+PARAMETER?\r\n", "AT+NETWORKID?\r\n",
                                    "AT+ADDRESS?\r\n"};

  delay(7000);
  for (int i = 0; i < 7; i++){
    Serial.print(listOfCommandsToSend[i]);
    delay(250);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  String buffer;
  while (Serial.available()){
    char c = Serial.read();
    buffer += c;
  }
  buffer.replace("+ERR=2\r\n", "");
  Serial.print(buffer);

  delay(4000);
}