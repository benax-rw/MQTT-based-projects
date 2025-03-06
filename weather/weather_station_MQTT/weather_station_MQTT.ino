#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "Benax-POP8A";
const char* password = "ben@kushi";

// MQTT Broker (VPS)
const char* mqtt_server = "157.173.101.159";  // Replace with your VPS IP

// DHT Sensor Configuration
#define DHTPIN 2
#define DHTTYPE DHT11  // Change to DHT22 if using DHT22
DHT dht(DHTPIN, DHTTYPE);

// MQTT Setup
WiFiClient espClient;
PubSubClient client(espClient);

// Function1 to Connect to WiFi
void setup_wifi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

// Function2 to Connect to MQTT Broker
void reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP8266")) { 
            Serial.println("Connected to MQTT!");
        } else {
            Serial.print("Failed, retrying in 5 seconds...");
            delay(5000);
        }
    }
}

// Function3 to Read Sensor Data
void read_sensor(float &temperature, float &humidity) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        temperature = -1; // Invalid value
        humidity = -1;     // Invalid value
    }
}

// Function4 to Publish Data to MQTT
void publish_sensor_data(float temperature, float humidity) {
    if (temperature == -1 || humidity == -1) {
        return;  // Do not publish invalid values
    }

    String temp_payload = String(temperature);
    String hum_payload = String(humidity);

    client.publish("/work_group_01/room_temp/temperature", temp_payload.c_str());
    client.publish("/work_group_01/room_temp/humidity", hum_payload.c_str());

    Serial.println("Published: Temperature = " + temp_payload + "°C");
    Serial.println("Published: Humidity = " + hum_payload + "%");
}

void setup() {
    Serial.begin(115200);
    setup_wifi(); //call Function1
    client.setServer(mqtt_server, 1883);
    dht.begin();
}

void loop() {
    if (!client.connected()) {
        reconnect_mqtt(); //call Function2
    }
    client.loop();

    float temperature, humidity;
    read_sensor(temperature, humidity); //call Function3
    publish_sensor_data(temperature, humidity); //call Function4

    delay(5000);  // Send data every 5 seconds
}

