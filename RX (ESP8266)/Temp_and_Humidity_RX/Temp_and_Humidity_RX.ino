#define MILLIS_BETWEEN_READINGS 1000
#define NUM_CONFIG_COMMANDS 7

String buffer; //buffer to hold RF received info

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //open serial for checking

  String listOfCommandsToSend[NUM_CONFIG_COMMANDS] = {"AT\r\n", "AT+PARAMETER=7,9,4,7\r\n", "AT+NETWORKID=6\r\n",
                                                      "AT+ADDRESS=1\r\n", "AT+PARAMETER?\r\n", "AT+NETWORKID?\r\n",
                                                      "AT+ADDRESS?\r\n"};

  delay(7000);
  for (int i = 0; i < NUM_CONFIG_COMMANDS; i++){
    Serial.print(listOfCommandsToSend[i]);
    delay(250);
  }
}

String humString;
String tempString;

float humidity;
float temperature;

int dollarSignIdx; //index of the dollar sign delimiter
int percentIdx; //index of the percent delimiter
int caratIdx; //index of the carat delimiter

void loop() {
  buffer = "";
  while (Serial.available()){ //while Serial buffer has info,
    char c = Serial.read(); //store byte as char
    buffer += c; //append recent char to buffer
  }
  buffer.replace("+ERR=2\r\n", ""); //remove extra error text
  //Serial.print(buffer);

  dollarSignIdx = buffer.indexOf('$'); //get location of first delimiter
  percentIdx = buffer.indexOf('%'); //get location of second delimiter
  caratIdx = buffer.indexOf('^'); //get location of third delimiter

  if (newReadingsReceived())
  {
    updateReadings();
  }

  //Serial.print(humidity);
  //Serial.print(" | ");
  //Serial.println(temperature);
  delay(MILLIS_BETWEEN_READINGS);
}

bool newReadingsReceived()
{
  return dollarSignIdx > -1 && percentIdx > -1 && caratIdx > -1;
}

void updateReadings()
{
  humString = buffer.substring(dollarSignIdx+1, percentIdx);
  tempString = buffer.substring(percentIdx+1, caratIdx);
  
  humidity = humString.toFloat();
  temperature = tempString.toFloat();
}