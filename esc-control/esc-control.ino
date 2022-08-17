#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
// Update these with values suitable for your network.
const char *ssid = "HUAWEI-1035";
const char *password = "93542670";
const char *mqtt_server = "143.198.182.161";
//const char* mqtt_server = "iot.eclipse.org";
String inStringESCMessage = "";

Servo ESCPIN;  // create servo object to control a servo
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
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

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message from: ");
  Serial.print(topic);
  for(int i=0;i<length;i++)
  {
    inStringESCMessage += (char)payload[i];
  }
    Serial.print(" | ");
    Serial.print("Value: ");
    Serial.println(inStringESCMessage.toInt());
    ESCPIN.write(inStringESCMessage.toInt());
    inStringESCMessage = "";
}//end callback

void reconnect() {
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-FELIA-Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("/FELIA/ESC");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ESCPIN.attach(D1, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
