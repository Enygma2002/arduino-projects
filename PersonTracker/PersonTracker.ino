#define DISTANCE_MAX 50.0
#define HEADING_INCREMENT_DEGREES 10

// Sensors: Trigger, Echo
byte leftSensor[] = {10, 9};
byte centerSensor[] = {A4, A5};
byte rightSensor[] = {A2, A3};
byte* sensors[] = {leftSensor, centerSensor, rightSensor};

long duration;
float distance;

float leftDistance;
float centerDistance;
float rightDistance;

char newDirection;
int heading;

#include <Servo.h>

#define SERVO_PIN 3
#define SERVO_MIN 0
#define SERVO_MAX 180
// Determine the servo delay needed to move the increment, according to its specs.
#define SERVO_SPEED_SPEC_MS 170.0
#define SERVO_SPEED_SPEC_DEGREES 60.0
#define SERVO_MOVE_DELAY_MS (SERVO_SPEED_SPEC_MS / SERVO_SPEED_SPEC_DEGREES * HEADING_INCREMENT_DEGREES)

Servo servo;
int servoPosition = 90;

#define PIR_PIN 2
bool personVisible = false;

void setup()
{
  Serial.begin(9600);

  // Initialize sensors
  for (int i = 0; i < sizeof(sensors); i++) {
    byte* sensor = sensors[i];

    byte trigger = sensor[0];
    pinMode(trigger, OUTPUT);
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);

    byte echo = sensor[1];
    pinMode(echo, INPUT);
  }

  Serial.println("[setup] Sensors initialized.");

  // Initialize the servo.
  servo.attach(SERVO_PIN);
  // Set it to mid-point.
  servo.write(servoPosition);

  Serial.println("[setup] Servo initialized.");

  delay(2000);
}

void loop()
{
  // Scan for any persons in sight.
  personVisible = digitalRead(PIR_PIN);
  if (!personVisible) {
    // No one around. Stop here and sleep a bit longer.
    Serial.println("No one around.");
    delay(500);
    return;
  }

  Serial.println("Person detected!");
  
  // Read the sensors
  leftDistance = readSensor(leftSensor);
  centerDistance = readSensor(centerSensor);
  rightDistance = readSensor(rightSensor);

  Serial.print("Left:\t");
  Serial.println(leftDistance);
  Serial.print("Center:\t");
  Serial.println(centerDistance);
  Serial.print("Right:\t");
  Serial.println(rightDistance);

  // Determine the heading to move to.
  bool isReadingLeft = isReading(leftDistance);
  bool isReadingCenter = isReading(centerDistance);
  bool isReadingRight = isReading(rightDistance);
  newDirection = getNewDirection(isReadingLeft, isReadingCenter, isReadingRight);
  heading = newDirection * HEADING_INCREMENT_DEGREES * -1; // My servo has 180* to the left and 0* to the right, so I reverse the direction.

  Serial.print("Rotate: ");
  Serial.print(heading);
  Serial.println(" degrees");

  // Determine the new servo's position.
  servoPosition += heading;

  // Stay within the limits of the servo's movement range.
  if (servoPosition > SERVO_MAX) {
    servoPosition = 180;
  } else if (servoPosition < SERVO_MIN) {
    servoPosition = 0;
  }

  Serial.print("Position: ");
  Serial.print(servoPosition);
  Serial.print("...");

  // Actually move to the new position.
  servo.write(servoPosition);
  Serial.print("move wait: "); Serial.println(SERVO_MOVE_DELAY_MS);
  delay(SERVO_MOVE_DELAY_MS);

  Serial.println("OK");

  Serial.println();

  delay(50);
}

float readSensor(byte sensor[]) {
  byte trigger = sensor[0];
  byte echo = sensor[1];

  // Prepare for a reading.
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  // Do the actual readin.
  duration = pulseIn(echo, HIGH);

  // Convert the reading to distance in cm.
  distance = (duration / 2) / 29.1;

  return distance;
}

/**
  L && (!C || !R) = -1
  C && ((L && R) || !(L && R)) = 0 (default)
  L && R = 0 (edge case coverage)
  R && (!C || !L) = 1
*/
char getNewDirection(bool L, bool C, bool R) {
  char result = 0;

  if (L) {
    if (C) {
      if (R) {
        // Straight.
      } else {
        // To L
        result = -1;
      }
    } else {
      if (R) {
        // Straight?
      } else {
        // To L
        result = -1;
      }
    }
  } else {
    if (C) {
      if (R) {
        // To R
        result = 1;
      } else {
        // Straight.
      }
    } else {
      if (R) {
        // To R
        result = 1;
      } else {
        // Nothing to do.
      }
    }
  }

  return result;
}

bool isReading(float distance) {
  return distance <= DISTANCE_MAX;
}

