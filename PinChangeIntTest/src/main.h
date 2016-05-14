#ifndef PINCHANGEINTTEST_MAIN_H
#define PINCHANGEINTTEST_MAIN_H

#define MIN_AWAKE_TIME_MS 15  // This needs to be more than Bounce2 library's default debounce time (10ms)

bool hasMinAwakeTimeElapsed();
void goToSleep();
void powerDown();

#endif //PINCHANGEINTTEST_MAIN_H
