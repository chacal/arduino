#include <Arduino.h>
#include <RFM69.h>
#include <SPI.h>
#include <limits.h>
#include <Thermometer.h>
#include <StreamPrint.h>
#include "main.h"

#define NETWORKID                      50
#define RFM_RECEIVER_ID                 1     // Gateway is ID 1
#define NODEID                        100
#define RFM69_NSS                      10     // SPI Chip Select / NSS for RFM69
#define RFM69_IRQ_PIN                   2     // IRQ pin for RFM69
#define RFM69_IRQ_NUM                   0     // Pin 2 is EXT_INT0
#define FREQUENCY             RF69_433MHZ

#define MOSFET_GATE_PIN                 6     // This pin is used for PWM control
#define ThermistorIN                   A6     // MOSFET temperature is measured from this pin
#define DIVIDER_RESISTOR            10000     // Resistor value for the thermistor voltage divider
#define TEMPERATURE_SEND_PERIOD_MS  15000     // How often the MOSFET temperature is reported
#define NRF_INSTANCE_NUMBER        NODEID     // Instance number for sending MOSFET temperatures

#define SERIAL_BAUD                 57600
#define COMMAND_TIMEOUT_MS          60000     // If no control commands have arrived within this time, the controlled level is set to 0
#define LEVEL_CONTROL_TAG             'l'


struct {
  char tag = 't';
  uint8_t instance = NRF_INSTANCE_NUMBER;
  float temp;
  int vcc = 3300;
  unsigned long previousSampleTimeMicros = 0;
} tempMeasurements;


RFM69 radio(RFM69_NSS, RFM69_IRQ_PIN, true, RFM69_IRQ_NUM);

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(MOSFET_GATE_PIN, OUTPUT);
  digitalWrite(MOSFET_GATE_PIN, LOW);

  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower();
  radio.setPowerLevel(20);
  radio.promiscuous(true);

  Serial << "Node: " << NODEID << endl;
  Serial << "Listening at 433 Mhz..." << endl;
}

unsigned long lastCommandTime = millis();
unsigned long lastTemperatureTime = millis();

void loop() {

  if(radio.receiveDone()) {
    handleReceivedMessage();
  } else if(commandTimedOut()) {
    turnOffMosfet();
  } else if(temperatureSendPeriodElapsed()) {
    sendMosfetTemperature();
  }

}

void handleReceivedMessage() {
  uint8_t size = radio.DATALEN;
  uint8_t buf[size];
  memcpy(buf, (const void *) radio.DATA, size);

  if(radio.ACKRequested()) {
    unsigned long start = micros();
    radio.sendACK();
    unsigned long duration = micros() - start;
    Serial << "ACK sent in " << duration << "us. ";
  }
  Serial << "RX_RSSI: " << radio.RSSI << endl;

  if(size == 2 && buf[0] == LEVEL_CONTROL_TAG) {
    Serial << "Setting level to " << buf[1] << endl;
    analogWrite(MOSFET_GATE_PIN, buf[1]);
    lastCommandTime = millis();
  }
}

bool commandTimedOut() { return millis() - lastCommandTime > COMMAND_TIMEOUT_MS && lastCommandTime != ULONG_MAX; }

void turnOffMosfet() {
  Serial << "No control command received in " << COMMAND_TIMEOUT_MS << "ms. Setting control level to 0." << endl;
  digitalWrite(MOSFET_GATE_PIN, LOW);
  lastCommandTime = ULONG_MAX;
}

bool temperatureSendPeriodElapsed() { return millis() - lastTemperatureTime > TEMPERATURE_SEND_PERIOD_MS; }

void sendMosfetTemperature() {// Read and wait to get the ADC settled
  analogRead(ThermistorIN);
  delayMicroseconds(100);
  tempMeasurements.temp = getTemperature(analogRead(ThermistorIN), DIVIDER_RESISTOR, 0);
  bool acked = radio.sendWithRetry(RFM_RECEIVER_ID, &tempMeasurements, sizeof(tempMeasurements), 3, 20);
  Serial << "MOSFET temp: " << tempMeasurements.temp << " Tx acked: " << acked << " TX_RSSI: " << radio.RSSI << endl;
  lastTemperatureTime = millis();
}

bool levelSendPeriodElapsed() { return millis() - lastLevelTime > LEVEL_SEND_PERIOD_MS; }

void sendMosfetLevel() {

}