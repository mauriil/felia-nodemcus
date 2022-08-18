#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
// Update these with values suitable for your network.
const char *ssid = "HUAWEI-1035";
const char *password = "93542670";
const char *mqtt_server = "143.198.182.161";

String inStringESCMessage = "";

Servo FLAPS;
Servo ROLL;
Servo ELEVATOR;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(100);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message from: ");
  Serial.print(topic);
  if(strcmp(topic, "/FELIA/FLAPS") == 0)
  {
    for (int i = 0; i < length; i++)
    {
      inStringESCMessage += (char)payload[i];
    }
    Serial.print(" | ");
    Serial.print("Value: ");
    Serial.println(inStringESCMessage.toInt());
    FLAPS.write(inStringESCMessage.toInt());
    inStringESCMessage = "";
  }
  if(strcmp(topic, "/FELIA/ROLL") == 0)
  {
    for (int i = 0; i < length; i++)
    {
      inStringESCMessage += (char)payload[i];
    }
    Serial.print(" | ");
    Serial.print("Value: ");
    Serial.println(inStringESCMessage.toInt());
    ROLL.write(inStringESCMessage.toInt());
    inStringESCMessage = "";
  }
  if(strcmp(topic, "/FELIA/ELEVATOR") == 0)
  {
    for (int i = 0; i < length; i++)
    {
      inStringESCMessage += (char)payload[i];
    }
    Serial.print(" | ");
    Serial.print("Value: ");
    Serial.println(inStringESCMessage.toInt());
    ELEVATOR.write(inStringESCMessage.toInt());
    inStringESCMessage = "";
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-FELIA-FLAP/ROLL/ELEVATOR-Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    // if you MQTT broker has clientID,username and password
    // please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // once connected to MQTT broker, subscribe command if any
      client.subscribe("/FELIA/FLAPS");
      delay(150);
      client.subscribe("/FELIA/ROLL");
      delay(150);
      client.subscribe("/FELIA/ELEVATOR");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
} // end reconnect()

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  FLAPS.attach(D1);
  ROLL.attach(D2);
  ELEVATOR.attach(D3);
  delay(1000);
  FLAPS.write(90);
  ROLL.write(90);
  ELEVATOR.write(90);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
