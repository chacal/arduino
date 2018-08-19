#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_BNO055.h>

/* Constructor */
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);
Adafruit_BNO055 bno = Adafruit_BNO055();

/* u8g2.begin() is required and will sent the setup/init sequence to the display */
void setup(void) {
  Serial.begin(115200);
  u8g2.begin();

  if(!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);

  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);
   
  /* Display some basic information on this sensor */
  displaySensorDetails();
}


void loop(void) {
  sensors_event_t event;
  uint8_t sys, gyro, accel, mag = 0;
  
  bno.getEvent(&event);
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  
  char buf[20];
  u8g2.firstPage();
  do {  
    u8g2.setFont(u8g2_font_helvR12_tr);
  
    sprintf(buf, "Heading: %d", (int)((float)event.orientation.x));
    u8g2.drawStr(0, 12, buf);
  
    sprintf(buf, "Pitch: %d", (int)((float)event.orientation.y));
    u8g2.drawStr(0, 28, buf);
  
    sprintf(buf, "Roll: %d", (int)((float)event.orientation.z));
    u8g2.drawStr(0, 44, buf);
  
    sprintf(buf, "s: %d g: %d a: %d m: %d", sys, gyro, accel, mag);
    u8g2.drawStr(0, 60, buf);
  } while ( u8g2.nextPage() );

  
  delay(100);
}


void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
