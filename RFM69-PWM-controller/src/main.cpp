#include <Arduino.h>
#include <RFM69.h>
#include <SPI.h>
#include <power.h>
#include <limits.h>

#define NETWORKID            50
#define NODEID               100
#define RFM69_NSS            10    // SPI Chip Select / NSS for RFM69
#define RFM69_IRQ_PIN        2     // IRQ pin for RFM69
#define RFM69_IRQ_NUM        0     // Pin 2 is EXT_INT0
#define FREQUENCY            RF69_433MHZ

#define MOSFET_GATE_PIN      6     // This pin is used for PWM control

#define SERIAL_BAUD          57600
#define COMMAND_TIMEOUT_MS   60000 // If no control commands have arrived within this time, the controlled level is set to 0
#define LEVEL_CONTROL_TAG    'l'

RFM69 radio(RFM69_NSS, RFM69_IRQ_PIN, true, RFM69_IRQ_NUM);

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(MOSFET_GATE_PIN, OUTPUT);
  digitalWrite(MOSFET_GATE_PIN, LOW);

  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower();
  radio.setPowerLevel(20);
  radio.promiscuous(true);

  Serial.print("Node: ");
  Serial.println(NODEID);
  Serial.println("Listening at 433 Mhz...");
}

unsigned long lastCommandTime = millis();

void loop() {
  if (radio.receiveDone()) {
    uint8_t size = radio.DATALEN;
    uint8_t buf[size];
    memcpy(buf, (const void*)radio.DATA, size);

    if (radio.ACKRequested()) {
      unsigned long start = micros();
      radio.sendACK();
      unsigned long duration = micros() - start;
      Serial.print("ACK sent in "); Serial.print(duration); Serial.print("us. ");
    }

    Serial.print("RX_RSSI: "); Serial.println(radio.RSSI);

    if(size == 2 && buf[0] == LEVEL_CONTROL_TAG) {
      Serial.print("Setting level to ");
      Serial.println(buf[1]);
      analogWrite(MOSFET_GATE_PIN, buf[1]);
      lastCommandTime = millis();
    }
  } else if(millis() - lastCommandTime > COMMAND_TIMEOUT_MS && lastCommandTime != ULONG_MAX) {
    Serial.print("No control command received in "); Serial.print(COMMAND_TIMEOUT_MS); Serial.println("ms. Setting control level to 0.");
    digitalWrite(MOSFET_GATE_PIN, LOW);
    lastCommandTime = ULONG_MAX;
  }
}
