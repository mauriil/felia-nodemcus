#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
// Update these with values suitable for your network.
const char *ssid = "HUAWEI-1035";
const char *password = "93542670";
const char *mqtt_server = "143.198.182.161";

#define LED D0 //BUILT IN
#define Buzzer D5

String inStringESCMessage = "";

TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); 
Servo RUDDER;
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
  digitalWrite (Buzzer, HIGH);
  delay(1000);
  Serial.println("WiFi connected");
  digitalWrite (Buzzer, LOW);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message from: ");
  Serial.print(topic);
  if(strcmp(topic, "/FELIA/RUDDER") == 0)
  {
    for (int i = 0; i < length; i++)
    {
      inStringESCMessage += (char)payload[i];
    }
    Serial.print(" | ");
    Serial.print("Value: ");
    Serial.println(inStringESCMessage.toInt());
    RUDDER.write(inStringESCMessage.toInt());
    inStringESCMessage = "";
  }
  if(strcmp(topic, "/FELIA/YAW") == 0)
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
    digitalWrite (Buzzer, HIGH);
    // Create a random client ID
    String clientId = "ESP8266-FELIA-FLAP/ROLL/ELEVATOR-Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    // if you MQTT broker has clientID,username and password
    // please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      digitalWrite (Buzzer, LOW);
      digitalWrite(LED, HIGH);
      delay(300);
      digitalWrite(LED, LOW);
      delay(300);
      digitalWrite(LED, HIGH);
      delay(300);
      digitalWrite(LED, LOW);
      delay(300);
      digitalWrite(LED, HIGH);
      // once connected to MQTT broker, subscribe command if any
      delay(300);
      client.subscribe("/FELIA/RUDDER");
      delay(300);
      client.subscribe("/FELIA/YAW");
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
  pinMode(LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  Serial.begin(9600);
  SerialGPS.begin(9600);
  digitalWrite(LED, HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ELEVATOR.attach(D3);
  RUDDER.attach(D4);
  delay(1000);
  digitalWrite(LED, LOW);
  ELEVATOR.write(90);
  RUDDER.write(90);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        String data = String(gps.location.lat(), 6)+","+String(gps.location.lng(), 6)+","+gps.satellites.value()+","+gps.altitude.meters()+","+gps.course.deg()+","+gps.speed.kmph();
        // data = latitude,longitude,satelites,altitude,course,speed(kmp)
        client.publish("/FELIA/GPS",(char*) data.c_str(), true);
        //snprintf(mqtt_payload, 50, "%d", String(gps.satellites.value()));
      }
    }
}
