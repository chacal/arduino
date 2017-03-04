#pragma once

#include "ConfigSaver.h"
#include <PubSubClient.h>
#include "StreamPrint.h"

void connectMQTT(PubSubClient &mqttClient, MqttConfiguration &mqttConfig, Client &client, Stream &stream, MQTT_CALLBACK_SIGNATURE);