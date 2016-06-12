#include <power.h>

#define MIN_AWAKE_TIME_MS 15  // This needs to be more than Bounce2 library's default debounce time (10ms)
#define DEBUG 0

void initializeConfig();
void initializeRadio();
bool hasMinAwakeTimeElapsed();
void goToSleep();
