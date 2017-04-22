// Watch video here: https://www.youtube.com/watch?v=BKp4ib1UQj8

/* Connection pins:
  Arduino     Soil Moisture Sensor YL-69
  A0             Analog A0
  5V               VCC
  GND              GND
*/

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT); //set up analog pin 0 to be input
}

void loop()
{
  int s = analogRead(A0); //take a sample
  Serial.print(s); Serial.print(" - ");

  if (s >= 1000) {
    Serial.println("Sensor is not in the Soil or DISCONNECTED");
  } else if (s >= 600) {
    Serial.println("Soil is DRY");
  } else if (s >= 370) {
    Serial.println("Soil is HUMID");
  } else {
    Serial.println("Sensor in WATER");
  }

  delay(50);
}

