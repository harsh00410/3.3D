#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials and MQTT server settings
const char* ssid = "Gopesh";           // Replace with your WiFi SSID
const char* password = "00000000";     // Replace with your WiFi password
const char* mqtt_server = "broker.emqx.io";  // EMQX MQTT broker
const int mqtt_port = 1883;
const char* mqtt_topic_wave = "SIT210/wave";  // MQTT topic

// Ultrasonic Sensor pins
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

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

// Function to connect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoPublisher")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

// Ultrasonic wave detection function
void detect_wave() {
  // Send trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);                 // Ensure trigger is off for at least 2 µs
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);                // Trigger pulse duration
  digitalWrite(trigPin, LOW);

  // Read echo pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  // Debug print to check actual readings
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Publish message if wave detected (object within 20 cm)
  if (distance > 0 && distance < 20) {  // Adjust the threshold to 20 cm
    client.publish(mqtt_topic_wave, "Wave detected by Publisher");
    Serial.println("Wave detected, message published!");
    delay(3000);  // Delay to avoid publishing too often
  } else {
    Serial.println("No wave detected.");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  detect_wave();
}
