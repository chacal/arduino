#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <LoopbackStream.h>
#include <PubSubClient.h>
#include <StreamPrint.h>

const char *ssid = "<SSID>";
const char *password = "<PASSWORD>";
const char *mqtt_server = "mqtt-home.chacal.online";


void connectWiFi();
void connectMQTT();


LoopbackStream mqttInputBuffer(1024);
WiFiClient wifiClient;
PubSubClient mqttClient(mqtt_server, 1883, wifiClient, mqttInputBuffer);
IRsend irsend(3);


void setup(void) {
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    irsend.begin();
    connectWiFi();
    connectMQTT();
    randomSeed(micros());
}

void loop(void) {
    if (!mqttClient.connected()) {
        connectMQTT();
    }
    mqttClient.loop();
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
    Serial << "Message arrived [" << topic << "] " << mqttInputBuffer.readString() << endl;
}

void connectWiFi() {
    WiFi.begin(ssid, password);
    Serial << endl;

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial << ".";
    }
    Serial << endl
           << "Connected to " << ssid << endl
           << "IP address: " << WiFi.localIP() << endl;
}

void connectMQTT() {
    mqttClient.setCallback(mqttCallback);

    // Loop until we're connected
    while (!mqttClient.connected()) {
        String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

        Serial << "Connecting to " << mqtt_server << " as " << clientId << endl;

        if (mqttClient.connect(clientId.c_str())) {
            Serial << "connected" << endl;
            mqttClient.subscribe("/kuikkeloinen/test");
        } else {
            Serial << "failed, rc=" << mqttClient.state() << " trying again in 5 seconds" << endl;
            delay(5000);
        }
    }
}
