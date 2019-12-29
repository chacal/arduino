#pragma once

#include <PubSubClient.h>

void connectMQTT(PubSubClient &mqttClient, Client &client);

void publishToMqtt(PubSubClient &mqttClient, char *str);