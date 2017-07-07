#define INSIDE_PHOTORESISTOR_PIN A7
#define OUTSIDE_PHOTORESISTOR_PIN A6

#define INSIDE_LIGHT_DIFFERENCE_THRESHOLD_PERCENT 20

#include <CheapStepper.h>

CheapStepper stepper;
// here we declare our stepper using default pins:
// arduino pin <--> pins on ULN2003 board:
// 8 <--> IN1
// 9 <--> IN2
// 10 <--> IN3
// 11 <--> IN4

boolean blindsClosed = false;
#define OPEN false
#define CLOSE true

// 28-BYJ48 has a Gear ratio of 64, and Stride Angle 5.625°  so this motor has a 4096 Steps.
// (360°/5.625°)*64"Gear ratio" = 64 * 64 = 4096
#define STEPS_PER_TURN 4096

#define TURNS_TO_OPERATE_BLINDS 2
#define STEPS_TO_OPERATE_BLINDS STEPS_PER_TURN * TURNS_TO_OPERATE_BLINDS

void setup() {
  Serial.begin(9600);
}

void loop() {

  int inLigt = getLight(INSIDE_PHOTORESISTOR_PIN);
  int outLigt = getLight(OUTSIDE_PHOTORESISTOR_PIN);
  int lightDifference = inLigt - outLigt;

  Serial.print("IN Light: ");
  Serial.println(inLigt);
  Serial.print("OUT Light: ");
  Serial.println(outLigt);
  Serial.print("Difference: ");
  Serial.println(lightDifference);

  if (inLigt - outLigt >= INSIDE_LIGHT_DIFFERENCE_THRESHOLD_PERCENT) {
    // Light difference is abolve the threshold.

    if (!blindsClosed) {
      operateBlinds(CLOSE);
      blindsClosed = true;
    }
  } else {
    if (blindsClosed) {
      operateBlinds(OPEN);
      blindsClosed = false;
    }
  }

  delay(1000);
}

int getLight(int photoresistorPin) {
  int value = analogRead(photoresistorPin);
  return map(value, 0, 1023, 0, 100);
}

void operateBlinds(boolean close) {
  Serial.println(close == CLOSE ? "Closing..." : "Opening...");

  for (int s = 0; s < STEPS_TO_OPERATE_BLINDS; s++) {
    // true for clockwise, false for counter-clockwise
    stepper.step(!close);
  }

  Serial.println("Done.");

  // Make sure the stepper is left in rest mode after moving.
  for (int i = 0; i < 4; i++) {
    digitalWrite(stepper.getPin(i), 0);
  }
}

