#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


/*
 * Wiring for RF receiver module (XY-MK-5V):
 * - RF GND -> Arduino GND
 * - RF VCC -> Arduino 5V
 * - RF Data -> Arduino Digital Pin 11
 * 
 * Wiring for LCD display:
 * - 5V -> VCC
 * - GND -> GND
 * - A4 -> SDA
 * - A5 -> SCL
 */

int FULL_TANK_VALUE = 1250;
RH_ASK driver(1200);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte mark100[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte mark84[8] = {
  B11111,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte mark68[8] = {
  B11111,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte mark52[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111
};

byte mark36[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111
};

byte mark20[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111
};

byte mark4[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

void setup()
{
    pinMode(13, OUTPUT);
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    lcd.begin();
    lcd.createChar(0, mark100);
    lcd.createChar(1, mark84);
    lcd.createChar(2, mark68);
    lcd.createChar(3, mark52);
    lcd.createChar(4, mark36);
    lcd.createChar(5, mark20);
    lcd.createChar(6, mark4);
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        turnLedOn();
        char message[buflen + 1];
        strFromBuf(message, buf, buflen);
        
        printWithMillis(message);
        showOnLCD(message);
        
        turnLedOff();
    }
}

void strFromBuf(char* dst, uint8_t* src, uint8_t bufLen)
{
  strncpy(dst, (char*)src, bufLen);
  dst[bufLen] = 0;
}

void turnLedOn()
{
    digitalWrite(13, HIGH);
}

void turnLedOff()
{
    digitalWrite(13, LOW);
}

void printWithMillis(char* message)
{
    Serial.print(millis());
    Serial.print(": ");
    Serial.println(message);  
}

void showOnLCD(char* message)
{
    char* divider = strchr(message, ':');
    int dividerIndex = divider - message;
    
    char val[16];
    strncpy(val, message, dividerIndex);
    val[dividerIndex] = '\0';
    int valNumber = atoi(val);

    char raw[16];
    strncpy(raw, message+dividerIndex+1, strlen(message)-dividerIndex);
    raw[strlen(message)-dividerIndex] = '\0';

    int percentage = min((float)valNumber / FULL_TANK_VALUE * 100, 100);
    char percentageStr[5];
    sprintf(percentageStr, "%3d%%", percentage);
    
    lcd.clear();
    lcd.print(val);
    lcd.setCursor(0,1);
    lcd.print(raw);
    lcd.setCursor(10,0);
    lcd.print(percentageStr);
    lcd.setCursor(15,0);
    
    if(percentage > 84)
      lcd.write(byte(0));
    else if(percentage > 68)
      lcd.write(byte(1));
    else if(percentage > 52)
      lcd.write(byte(2));
    else if(percentage > 36)
      lcd.write(byte(3));
    else if(percentage > 20)
      lcd.write(byte(4));
    else if(percentage > 4)
      lcd.write(byte(5));
    else
      lcd.write(byte(6));
}

