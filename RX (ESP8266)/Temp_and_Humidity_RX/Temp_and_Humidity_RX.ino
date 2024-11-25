#define MILLIS_BETWEEN_READINGS 1000
#define NUM_CONFIG_COMMANDS 7

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Venue_slow";
const char* password = "onmyhonor";

ESP8266WebServer server(80);

String buffer;
String humString;
String tempString;

float humidity;
float temperature;

int dollarSignIdx;
int percentIdx;
int caratIdx;

void setup() {
  Serial.begin(115200);

  // Configure WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  // Configure web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");

  String listOfCommandsToSend[NUM_CONFIG_COMMANDS] = {"AT\r\n", "AT+PARAMETER=7,9,4,7\r\n", "AT+NETWORKID=6\r\n",
                                                      "AT+ADDRESS=1\r\n", "AT+PARAMETER?\r\n", "AT+NETWORKID?\r\n",
                                                      "AT+ADDRESS?\r\n"};

  delay(7000);
  for (int i = 0; i < NUM_CONFIG_COMMANDS; i++){
    Serial.print(listOfCommandsToSend[i]);
    delay(250);
  }
}

void loop() {
  buffer = "";
  while (Serial.available()) {
    char c = Serial.read();
    buffer += c;
  }
  buffer.replace("+ERR=2\r\n", "");

  dollarSignIdx = buffer.indexOf('$');
  percentIdx = buffer.indexOf('%');
  caratIdx = buffer.indexOf('^');

  if (newReadingsReceived()) {
    updateReadings();
  }

  server.handleClient(); // Handle client requests

  delay(MILLIS_BETWEEN_READINGS);
}

bool newReadingsReceived() {
  return dollarSignIdx > -1 && percentIdx > -1 && caratIdx > -1;
}

void updateReadings() {
  humString = buffer.substring(dollarSignIdx + 1, percentIdx);
  tempString = buffer.substring(percentIdx + 1, caratIdx);

  humidity = humString.toFloat();
  temperature = tempString.toFloat();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Humidity and Temperature Readings</h1>";
  html += "<p>Humidity: " + String(humidity) + "%</p>";
  html += "<p>Temperature: " + String(temperature) + "C</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}
