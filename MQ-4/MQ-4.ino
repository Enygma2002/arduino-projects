/*
 * Basic analog input reading of the MQ-4 sensor.
 */

#define SENSOR_ANALOG_PIN A6

int sensorValue;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  sensorValue = analogRead(SENSOR_ANALOG_PIN);

  Serial.print(F("Sensor value: "));
  Serial.println(sensorValue, DEC);

  // Delay between readings...
  delay(100);
}
