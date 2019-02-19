#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 7, 8);
#define LCD_LIGHT_PIN 3

#define SENSOR_MQ135_PIN A4

#define SENSOR_MQ4_PIN A6

#include "DHT.h"
#define DHT22_PIN 2
DHT dht(DHT22_PIN, DHT22);

void setup() {
  // LCD Light control.
  pinMode(LCD_LIGHT_PIN, OUTPUT);
  digitalWrite(LCD_LIGHT_PIN, 0);  
  delay(1000);

  // Init LCD.
  display.begin();
  display.setContrast(60);
  display.setTextSize(1);
  display.setTextColor(BLACK);

  // Init DHT22.
  dht.begin();
}

void loop() {
  // Read sensor values.
  int mq135Value = analogRead(SENSOR_MQ135_PIN);

  int mq4Value = analogRead(SENSOR_MQ4_PIN);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

  // Display the values.
  display.clearDisplay();

  display.setCursor(0, 0);

  display.print(F("T: "));
  display.print(temperature, 2);
  display.println(F("C  "));

  display.print(F("H: "));
  display.print(humidity, 2);
  display.println(F("%"));

  display.print(F("Heat: "));
  display.print(heatIndex, 2);
  display.println(F("C"));

  display.print(F("Air: "));
  display.print(mq135Value, DEC);
  display.println(F("/1023 "));

  display.print(F("Gas: "));
  display.print(mq4Value, DEC);
  display.println(F("/1023"));

  display.display();
  delay(2000);
}

