#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials and MQTT server settings
const char* ssid = "Gopesh";           // Replace with your WiFi SSID
const char* password = "00000000";   // Replace with your WiFi password
const char* mqtt_server = "broker.emqx.io";  // EMQX MQTT broker
const int mqtt_port = 1883;
const char* mqtt_topic_wave = "SIT210/wave";  // MQTT topic

// LED pin
const int ledPin = 8;

WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();

  // Flash LED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

// Function to connect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoSubscriber")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_wave);  // Subscribe to the topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}