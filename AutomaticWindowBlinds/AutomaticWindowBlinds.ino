#define INSIDE_PHOTORESISTOR_PIN A7
#define OUTSIDE_PHOTORESISTOR_PIN A6

#define INSIDE_LIGHT_DIFFERENCE_THRESHOLD_PERCENT 20

#define DEBUG true

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

int currentStep = 0;

#define COMMAND_AUTO "auto"
#define COMMAND_MANUAL "manual"
#define COMMAND_OPEN "open"
#define COMMAND_CLOSE "close"
#define COMMAND_GOTO "goto"
boolean modeAuto = true;

void setup() {
  Serial.begin(9600);

  if (DEBUG) {
    Serial.print("Open position: ");
    Serial.println(0);

    Serial.print("Closed position: ");
    Serial.println(STEPS_TO_OPERATE_BLINDS);
  }
}

void loop() {
  String command = Serial.readStringUntil('\n');

  int inLigt = getLight(INSIDE_PHOTORESISTOR_PIN);
  int outLigt = getLight(OUTSIDE_PHOTORESISTOR_PIN);
  int lightDifference = inLigt - outLigt;

  if (DEBUG) {
    Serial.println();

    Serial.print("IN Light: ");
    Serial.println(inLigt);

    Serial.print("OUT Light: ");
    Serial.println(outLigt);

    Serial.print("Difference: ");
    Serial.println(lightDifference);

    Serial.print("State: ");
    Serial.println(blindsClosed ? "Closed" : "Open");

    Serial.print("Current Step: ");
    Serial.print(currentStep);
    Serial.print(" / ");
    Serial.println(STEPS_TO_OPERATE_BLINDS);

    Serial.print("Mode: ");
    Serial.println(modeAuto ? "auto" : "manual");

    Serial.print("Command: ");
    Serial.println(command);
  }

  if (command == COMMAND_AUTO) {
    modeAuto = true;
  } else if (command == COMMAND_MANUAL) {
    modeAuto = false;
  }

  if (modeAuto) {
    // Auto mode.
    if (lightDifference >= INSIDE_LIGHT_DIFFERENCE_THRESHOLD_PERCENT) {
      // Light difference is abolve the threshold.

      if (!blindsClosed) {
        operateBlinds(CLOSE);
      }
    } else {
      if (blindsClosed) {
        operateBlinds(OPEN);
      }
    }
  } else {
    // Manual mode.
    if (command == COMMAND_OPEN) {
      operateBlinds(OPEN);
    } else if (command == COMMAND_CLOSE) {
      if (!blindsClosed) {
        operateBlinds(CLOSE);
      }
    } else if (command.startsWith(COMMAND_OPEN)) {
      long steps = command.substring(sizeof(COMMAND_OPEN)).toInt();
      stepTo(currentStep - steps);
    } else if (command.startsWith(COMMAND_CLOSE)) {
      long steps = command.substring(sizeof(COMMAND_CLOSE)).toInt();
      stepTo(currentStep + steps);
    } else if (command.startsWith(COMMAND_GOTO)) {
      int percent = command.substring(sizeof(COMMAND_GOTO)).toInt();
      int steps = map(percent, 0, 100, 0, STEPS_TO_OPERATE_BLINDS);
      stepTo(steps);
    }
  }

  delay(1000);
}

int getLight(int photoresistorPin) {
  // Do a double reading on the analog pin to try to minimize leakage from adjacent pins.
  int value = analogRead(photoresistorPin);
  value = analogRead(photoresistorPin);
  return map(value, 0, 1023, 0, 100);
}

void operateBlinds(boolean close) {
  if (DEBUG) {
    Serial.println(close == CLOSE ? "Closing..." : "Opening...");
  }

  stepTo(close == CLOSE ? STEPS_TO_OPERATE_BLINDS : 0);

  if (DEBUG) {
    Serial.println("Done.");
  }
}

void stepTo(int targetStep) {
  // Over/Underflow protection.
  if (targetStep > STEPS_TO_OPERATE_BLINDS) {
    targetStep = STEPS_TO_OPERATE_BLINDS;
    Serial.print("Overflow fixed.");
  } else if (targetStep < 0) {
    targetStep = 0;
    Serial.print("Underflow fixed.");
  }

  int steps = targetStep - currentStep;

  // True for clockwise, false for counter-clockwise.
  // Starting position is opened (step 0). Positive steps closes, nevative steps open.
  // To close, we go counter-clockwise. To open, we go clockwise.
  boolean direction = steps > 0 ? false : true;

  if (DEBUG) {
    Serial.print(direction ? "Opening " : "Closing ");
    Serial.print(steps);
    Serial.print(" steps... ");
  }

  for (int s = 0; s < abs(steps); s++) {
    stepper.step(direction);
  }

  if (DEBUG) {
    Serial.println("Move done.");
  }

  // Make sure the stepper is left in rest mode after moving, since the  Cheap Stepper library
  // seems to leave it energized in the last executed stage and consuming useless power.
  for (int i = 0; i < 4; i++) {
    digitalWrite(stepper.getPin(i), 0);
  }

  // Remember the state.
  currentStep = targetStep;
  blindsClosed = currentStep == STEPS_TO_OPERATE_BLINDS;
}
