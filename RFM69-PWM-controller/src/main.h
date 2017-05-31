#pragma once

void handleReceivedMessage();

bool commandTimedOut();
void turnOffMosfet();

bool temperatureSendPeriodElapsed();
void sendMosfetTemperature();

bool levelSendPeriodElapsed();
void sendMosfetLevel();
