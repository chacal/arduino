#pragma once

#include <WString.h>

void webServerInit(const std::function<void()> &configUpdated);

void webServerBroadcastWs(const String &msg);