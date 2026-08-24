#include <ESP32Servo.h>

Servo servo1, servo2, servo3, servo4, servo5;

const int servo1Pin = 12;   
const int servo2Pin = 14;   
const int servo3Pin = 27;   
const int servo4Pin = 26;   
const int servo5Pin = 25;   

const int emgPin = 36;      
int emgValue = 0;
float emgFiltered = 0;
float emgBaseline = 0;
float emgActivity = 0;

const int numReadings = 20;
int readings[numReadings];
int readIndex = 0;
int total = 0;

int minActivity = 0;
int maxActivity = 300;
int threshold = 250;

int allFingersOpen = 0;
int allFingersClosed = 180;

Servo servoArray[5] = {
  servo1, servo2, servo3, servo4, servo5
};

int servoPins[5] = {
  servo1Pin,
  servo2Pin,
  servo3Pin,
  servo4Pin,
  servo5Pin
};

int servoPositions[5] = {
  0, 0, 0, 0, 0
};

int targetPosition = 0;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("\n====================================");
  Serial.println("5-Finger Prosthetic Hand Controller");
  Serial.println("EMG Controlled System");
  Serial.println("====================================\n");

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for (int i = 0; i < 5; i++) {

    servoArray[i].setPeriodHertz(50);

    servoArray[i].attach(
      servoPins[i],
      500,
      2400
    );

    servoArray[i].write(allFingersOpen);

    delay(100);
  }

  Serial.println("Servos initialized\n");

  delay(1000);

  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  Serial.println("Calibrating EMG baseline...");
  Serial.println("Keep your forearm relaxed and still!");

  delay(2000);

  long sum = 0;

  for (int i = 0; i < 200; i++) {

    sum += analogRead(emgPin);

    delay(5);
  }

  emgBaseline = sum / 200.0;

  Serial.print("Baseline calibrated: ");
  Serial.println(emgBaseline);

  Serial.println(
    "\nReady! Flex your muscle to control the prosthetic hand."
  );

  Serial.println("====================================\n");

  delay(1000);
}

void loop() {


  emgValue = analogRead(emgPin);

  static float smoothedEMG = 0;

  float alpha = 0.1;

  smoothedEMG =
    alpha * emgValue +
    (1 - alpha) * smoothedEMG;

  emgFiltered = smoothedEMG;

  total -= readings[readIndex];

  readings[readIndex] = emgFiltered;

  total += readings[readIndex];

  readIndex =
    (readIndex + 1) % numReadings;

  float avgFiltered =
    total / (float)numReadings;


  emgActivity =
    abs(avgFiltered - emgBaseline);

  emgActivity =
    constrain(
      emgActivity,
      minActivity,
      maxActivity
    );


  if (emgActivity < threshold / 2) {

    emgBaseline =
      (0.99 * emgBaseline) +
      (0.01 * avgFiltered);
  }


  if (emgActivity > threshold) {

    targetPosition =
      map(
        emgActivity,
        threshold,
        maxActivity,
        0,
        180
      );

    targetPosition =
      constrain(
        targetPosition,
        0,
        180
      );

  } else {

    targetPosition =
      allFingersOpen;
  }


  for (int i = 0; i < 5; i++) {

    int currentPos =
      servoPositions[i];

    if (targetPosition > currentPos) {
      currentPos += 2;
    }
    else if (targetPosition < currentPos) {
      currentPos -= 2;
    }

    currentPos =
      constrain(
        currentPos,
        0,
        180
      );

    servoArray[i].write(currentPos);

    servoPositions[i] =
      currentPos;
  }

  static unsigned long lastPrintTime = 0;

  if (millis() - lastPrintTime >= 250) {

    lastPrintTime = millis();

    Serial.print("EMG Raw: ");
    Serial.print(emgValue);

    Serial.print(" | Filtered: ");
    Serial.print((int)avgFiltered);

    Serial.print(" | Activity: ");
    Serial.print((int)emgActivity);

    Serial.print(" | Target Pos: ");
    Serial.println(targetPosition);
  }

  delay(20);
}
