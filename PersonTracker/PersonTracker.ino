#define DISTANCE_MAX 100.0
#define HEADING_INCREMENT 10

// Sensors: Trigger, Echo
byte leftSensor[] = {A4, A5};
byte centerSensor[] = {10, 9};
byte rightSensor[] = {A2, A3};
byte* sensors[] = {leftSensor, centerSensor, rightSensor};

int duration;
float distance;

float leftDistance;
float centerDistance;
float rightDistance;

char newDirection;
int heading;

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

  delay(6000);

  Serial.println("Distance:");
}

void loop()
{
  leftDistance = readSensor(leftSensor);
  centerDistance = readSensor(centerSensor);
  rightDistance = readSensor(rightSensor);

  Serial.print("Left:\t");
  Serial.println(leftDistance);
  Serial.print("Center:\t");
  Serial.println(centerDistance);
  Serial.print("Right:\t");
  Serial.println(rightDistance);

  bool isReadingLeft = isReading(leftDistance);
  bool isReadingCenter = isReading(centerDistance);
  bool isReadingRight = isReading(rightDistance);
  newDirection = getNewDirection(isReadingLeft, isReadingCenter, isReadingRight);
  heading = newDirection * HEADING_INCREMENT;

  Serial.print("Rotate: ");
  Serial.print(heading);
  Serial.println(" degrees");

  Serial.println();

  delay(500);
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

