
enum Ina219Mode { POWER_DOWN = 0, TRIGGER_SHUNT = 1 };

void initializeINA219();
void configureINA219(Ina219Mode mode);
void initializeConfig();
void initializeRadio();
int readExternalVoltage();
