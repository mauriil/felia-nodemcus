#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// Update these with values suitable for your network.
const char *ssid = "HUAWEI-1035";
const char *password = "93542670";
const char *mqtt_server = "broker.mqttdashboard.com";
// const char* mqtt_server = "iot.eclipse.org";

int GREEN_LEFT_D1 = 5;
int GREEN_LEFT_D2 = 4;
int RED_RIGHT_D3 = 0;
int RED_RIGHT_D4 = 2;
boolean navLights = false;
int STROBE_TOPLEFT_D5 = 14;
int STROBE_TOPRIGHT_D6 = 12;
int STROBE_TAIL_D7 = 13;
boolean strobeLights = false;
int LANDING_D8 = 15;
boolean landingLights = false;

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
  Serial.print("Command from MQTT broker is : [");
  Serial.print(topic);
  if (topic == "/FELIA/NAVLIGHTS")
  {
    navLights = true;
  }
  if (topic == "/FELIA/STROBELIGHTS")
  {
    strobeLights = true;
  }
  if (topic == "/FELIA/LANDINGLIGHTS")
  {
    landingLights = true;
  }

} // end callback

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-FELIA-Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    // if you MQTT broker has clientID,username and password
    // please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // once connected to MQTT broker, subscribe command if any
      client.subscribe("/FELIA/NAVLIGHTS");
      delay(150);
      client.subscribe("/FELIA/STROBELIGHTS");
      delay(150);
      client.subscribe("/FELIA/LANDINGLIGHTS");
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
  pinMode(GREEN_LEFT_D1, OUTPUT);
  pinMode(GREEN_LEFT_D2, OUTPUT);
  pinMode(RED_RIGHT_D3, OUTPUT);
  pinMode(RED_RIGHT_D4, OUTPUT);
  pinMode(STROBE_TOPLEFT_D5, OUTPUT);
  pinMode(STROBE_TOPRIGHT_D6, OUTPUT);
  pinMode(STROBE_TAIL_D7, OUTPUT);
  pinMode(LANDING_D8, OUTPUT);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (navLights == true){
    digitalWrite(GREEN_LEFT_D1, HIGH);
    digitalWrite(GREEN_LEFT_D2, HIGH);
    digitalWrite(RED_RIGHT_D3, HIGH);
    digitalWrite(RED_RIGHT_D4, HIGH);
  } else {
    digitalWrite(GREEN_LEFT_D1, LOW);
    digitalWrite(GREEN_LEFT_D2, LOW);
    digitalWrite(RED_RIGHT_D3, LOW);
    digitalWrite(RED_RIGHT_D4, LOW);
  }
  if (strobeLights == true){    
    digitalWrite(STROBE_TAIL_D7, HIGH);
    for (int i = 0; i <= 4; i++) {
      if (i == 2) {
        digitalWrite(STROBE_TAIL_D7, LOW);
      }      
      digitalWrite(STROBE_TOPLEFT_D5, HIGH);
      digitalWrite(STROBE_TOPRIGHT_D6, HIGH);
      delay(200);
      digitalWrite(STROBE_TOPLEFT_D5, LOW);
      digitalWrite(STROBE_TOPRIGHT_D6, LOW);
      delay(200);
    }

  } else {
    digitalWrite(STROBE_TOPLEFT_D5, LOW);
    digitalWrite(STROBE_TOPRIGHT_D6, LOW);
    digitalWrite(STROBE_TAIL_D7, LOW);
  }
  if (landingLights == true){
    digitalWrite(LANDING_D8, HIGH);
  } else {
    digitalWrite(LANDING_D8, LOW);
  }
  
  
  
  client.loop();
}
