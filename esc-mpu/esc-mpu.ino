// SERVO (ESC) VALUE : >=29  = OFF
// SERVO (ESC) VALUE : 30  = RALENTI
// SERVO (ESC) VALUE : 40  = RALENTI +++
// SERVO (ESC) VALUE : 180 = MAX VALUE
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Update these with values suitable for your network.
const char *ssid = "HUAWEI-1035";
const char *password = "93542670";
const char *mqtt_server = "143.198.182.161";
#define LED D0 //BUILT IN
#define Buzzer D5

Adafruit_MPU6050 mpu;

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
  digitalWrite (Buzzer, HIGH);
  delay(1000);
  Serial.println("WiFi connected");
  digitalWrite (Buzzer, LOW);
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
    digitalWrite (Buzzer, HIGH);
    // Create a random client ID
    String clientId = "ESP8266-FELIA-ESC-MPU-Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
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
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ESCPIN.attach(D3, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  // client.publish("/FELIA/accelerationx", String(a.acceleration.x).c_str()); // X  m/s^2
  // client.publish("/FELIA/accelerationy", String(a.acceleration.y).c_str()); // Y  m/s^2
  // client.publish("/FELIA/accelerationz", String(a.acceleration.z).c_str()); // Z  m/s^2

  client.publish("/FELIA/gyrox", String(map(a.gyro.x, -10, 10, 0, 180)).c_str()); // X rad/s
  client.publish("/FELIA/gyroy", String(map(a.gyro.y, -10, 10, 0, 180)).c_str()); // Y rad/s
  // client.publish("/FELIA/gyroz", String(a.gyro.z).c_str()); // Z rad/s

  delay(100);
}
