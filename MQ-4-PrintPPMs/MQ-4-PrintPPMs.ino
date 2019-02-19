/*
 * MQ-4 sensor reading with calibration and printing of PPM values
 * instead of just raw analog input.
 * 
 * Original version with details can be found at:
 * https://www.geekstips.com/mq4-sensor-natural-gas-methane-arduino/
 */

#define SENSOR_ANALOG_PIN A6

// R0 is the resistance of the sensor at a known concentration without the presence of other gases, or in fresh air.
// Initialize as "calibration needed". If already calibrated, just use your calibrated value instead of -1. (e.g. 11.820)
float calibration_R0 = -1;
//float calibration_R0 = 11.820;

void setup() {
  Serial.begin(9600);

  pinMode(SENSOR_ANALOG_PIN, INPUT);

  // Calibrate the sensor and determine R0, if not already provided.
  calibrate();

  // Display the calibration result
  Serial.print(F("Calibration: R0 = "));
  Serial.println(calibration_R0);
}

void loop() {
  double ppm = readPPM();
  Serial.print("PPM methane = ");
  Serial.println(ppm);

  delay(1000);
}

void calibrate() {
  if (calibration_R0 > 0) {
    // Skip calibration if R0 value is provided from a previous fresh air calibration.
    return;
  }
  
  Serial.println(F("Calibrating..."));

  // Get the average analog sensor value from 500 readings.
  float sensorValue;
  for (int x = 0 ; x < 500 ; x++){
    sensorValue = sensorValue + analogRead(SENSOR_ANALOG_PIN);
  }
  sensorValue = sensorValue / 500.0;

  // Convert average analog reading to voltage.
  float sensor_volt = sensorValue * (5.0 / 1023.0);
  
  // Calculate RS in fresh air.
  float RS_air = ((5.0 * 10.0) / sensor_volt) - 10.0;
  
  // Calculate R0.
  calibration_R0 = RS_air / 4.4; 
}

double readPPM() {
  // Read analog values of sensor.
  float sensorValue = analogRead(SENSOR_ANALOG_PIN);

  // Convert analog values to voltage.
  float sensor_volt = sensorValue * (5.0 / 1023.0);

  // Get value of RS in a gas (sensor resistance).
  float RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; 

  // Get ratio RS_gas/RS_air.
  float ratio = RS_gas / calibration_R0;

  float m = -0.318; // Slope
  float b = 1.133; // Y-Intercept

  // Get ppm value in linear scale according to the the ratio value.
  double ppm_log = (log10(ratio) - b) / m; 
  // Convert ppm value to log scale.
  double ppm = pow(10, ppm_log);
}
