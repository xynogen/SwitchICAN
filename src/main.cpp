#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

// Global Var
#ifndef STASSID
#define STASSID "PAIO"
#define STAPSK "complexity"
#endif


#define RPWMML D5
#define LPWMML D6
#define RPWMMR D7
#define LPWMMR D8
#define EN D2
#define SERVO D4
#define POMPA1 D0
#define POMPA2 D1

const char *ssid = STASSID;
const char *password = STAPSK;

//Switch State
int SW0;
int SW1;
int SW2;
int SW3;

// Set Static IP Configuration
IPAddress local_IP(192, 168, 0, 103);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// Set server Port
WiFiServer server(80);
Servo myservo;

void setup()
{
  SW0 = 1;
  SW1 = 1;
  SW2 = 1; 
  SW3 = 1;
  Serial.begin(115200);
  pinMode(SERVO,OUTPUT);
  pinMode(RPWMML,OUTPUT);
  pinMode(LPWMML,OUTPUT);
  pinMode(RPWMMR,OUTPUT);
  pinMode(LPWMML,OUTPUT);
  pinMode(EN,OUTPUT);
  pinMode(POMPA1, OUTPUT);
  pinMode(POMPA2, OUTPUT);
  digitalWrite(POMPA1, HIGH);
  digitalWrite(POMPA2, HIGH);
  
  myservo.attach(SERVO);
  myservo.write(180);
  delay(1000);

  
  
  // Clear the serial output
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.print(F("Connecting to : "));
  Serial.println(ssid);

  // Setting Static IP Address to NodeMCU
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println(F("STA Failed to configure"));
  }

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F(""));

  Serial.println(F(""));
  Serial.println(F("WiFi Connected"));
  Serial.print(F("IP Address : "));
  Serial.println(WiFi.localIP());
  Serial.println(F(""));

  server.begin();
  Serial.println(F("Server Started"));
}

void loop()
{
  WiFiClient client = server.available();

  if (!client)
    return;

  Serial.println(F("New Client"));

  // Default value 1000
  client.setTimeout(5000);

  // Read the request
  String req = client.readStringUntil('\r');
  Serial.println(F("Request: "));
  Serial.println(req);

  if (req.indexOf(F("/SW/0/0")) != -1)
  {
    SW0 = 0;
    myservo.write(0);
    delay(1000);
  }
   
  else if (req.indexOf(F("/SW/0/1")) != -1)
  {
    SW0 = 1;
    myservo.write(180);
    delay(1000);
  }
  
  else if (req.indexOf(F("/SW/1/0")) != -1)
  {
    SW1 = 0;
    digitalWrite(EN, HIGH);
    analogWrite(RPWMML, 255);
    analogWrite(LPWMML, 0);
    analogWrite(RPWMMR, 255);
    analogWrite(LPWMMR, 0);
  }
  
  else if (req.indexOf(F("/SW/1/1")) != -1)
  {
    SW1 = 1;
    digitalWrite(EN, LOW);
    analogWrite(RPWMML, 0);
    analogWrite(LPWMML, 0);
    analogWrite(RPWMMR, 0);
    analogWrite(LPWMMR, 0);
  }
  
  else if (req.indexOf(F("/SW/2/0")) != -1)
   {
    SW2 = 0;
    digitalWrite(POMPA1, SW2);
   }
   
  else if (req.indexOf(F("/SW/2/1")) != -1)
  {
    SW2 = 1;
    digitalWrite(POMPA1, SW2);
  }
  
  else if (req.indexOf(F("/SW/3/0")) != -1)
  {
    SW3 = 0;
    digitalWrite(POMPA2, SW3);
  }

  else if (req.indexOf(F("/SW/3/1")) != -1)
  {
    SW3 = 1;
    digitalWrite(POMPA2, SW3);
  }
 
  while (client.available())
    client.read();


  /*
  Format for JSON data
  {"SW0": X,
  "SW1": X,
  "SW2": X}
  */

  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
  client.print(F("{\"SW0\":"));
  client.print(SW0);
  client.print(F(",\"SW1\":"));
  client.print(SW1);
  client.print(F(",\"SW2\":"));
  client.print(SW2);
  client.print(F(",\"SW3\":"));
  client.print(SW3);
  client.print(F("}\r\n\r\n"));
  Serial.println(F("Disconnecting from client"));
}