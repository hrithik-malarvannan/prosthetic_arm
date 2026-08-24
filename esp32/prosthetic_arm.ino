#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "secrets.h"

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

Servo servoArray[5] = {
  servo1,
  servo2,
  servo3,
  servo4,
  servo5
};

const int servoPins[5] = {
  12,
  14,
  27,
  26,
  25
};

const int emgPin = 36;

const int numReadings = 20;
int readings[numReadings];
int readIndex = 0;
int total = 0;

int emgValue = 0;
float emgFiltered = 0;
float emgBaseline = 0;
float emgActivity = 0;

int minActivity = 0;
int maxActivity = 300;
int threshold = 250;

int servoPositions[5] = {
  0, 0, 0, 0, 0
};

int targetPosition = 0;

bool autoMode = true;

WebServer server(80);

void setAllServos(int angle) {
  angle = constrain(angle, 0, 180);

  for (int i = 0; i < 5; i++) {
    servoArray[i].write(angle);
    servoPositions[i] = angle;
  }
}

void handleServo() {
  if (!server.hasArg("id") || !server.hasArg("angle")) {
    server.send(400, "text/plain", "Missing id or angle");
    return;
  }

  int id = server.arg("id").toInt();
  int angle = server.arg("angle").toInt();

  if (id < 0 || id > 4) {
    server.send(400, "text/plain", "Invalid servo ID");
    return;
  }

  angle = constrain(angle, 0, 180);

  autoMode = false;

  servoArray[id].write(angle);
  servoPositions[id] = angle;

  server.send(200, "text/plain", "Servo command sent");
}

void handleEMG() {
  String state;

  if (emgActivity > threshold) {
    state = "ACTIVE";
  } else {
    state = "RELAXED";
  }

  String json = "{";
  json += "\"emgValue\":" + String(emgValue) + ",";
  json += "\"filtered\":" + String(emgFiltered, 1) + ",";
  json += "\"activity\":" + String(emgActivity, 1) + ",";
  json += "\"threshold\":" + String(threshold) + ",";
  json += "\"handState\":\"" + state + "\",";
  json += "\"mode\":\"" + String(autoMode ? "AUTO" : "MANUAL") + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void handleMode() {
  if (!server.hasArg("mode")) {
    server.send(400, "text/plain", "Missing mode");
    return;
  }

  String mode = server.arg("mode");

  if (mode == "auto") {
    autoMode = true;
    server.send(200, "text/plain", "AUTO mode enabled");
  }
  else if (mode == "manual") {
    autoMode = false;
    server.send(200, "text/plain", "MANUAL mode enabled");
  }
  else {
    server.send(400, "text/plain", "Invalid mode");
  }
}

void handleStatus() {
  String json = "{";
  json += "\"wifi\":true,";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"mode\":\"" + String(autoMode ? "AUTO" : "MANUAL") + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for (int i = 0; i < 5; i++) {
    servoArray[i].setPeriodHertz(50);
    servoArray[i].attach(servoPins[i], 500, 2400);
    servoArray[i].write(0);
    delay(100);
  }

  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  Serial.println();
  Serial.println("====================================");
  Serial.println("EMG-Controlled Prosthetic Hand");
  Serial.println("====================================");

  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed");
  }

  Serial.println();
  Serial.println("Calibrating EMG baseline...");
  Serial.println("Keep your forearm relaxed and still!");

  delay(2000);

  long sum = 0;

  for (int i = 0; i < 200; i++) {
    sum += analogRead(emgPin);
    delay(5);
  }

  emgBaseline = sum / 200.0;

  Serial.print("Baseline: ");
  Serial.println(emgBaseline);

  server.on("/servo", HTTP_GET, handleServo);
  server.on("/emg", HTTP_GET, handleEMG);
  server.on("/mode", HTTP_GET, handleMode);
  server.on("/status", HTTP_GET, handleStatus);

  server.begin();

  Serial.println("HTTP server started");
  Serial.println("System ready");
  Serial.println("====================================");
}

void loop() {
  server.handleClient();

  emgValue = analogRead(emgPin);

  static float smoothedEMG = 0;
  float alpha = 0.1;

  smoothedEMG =
    alpha * emgValue +
    (1 - alpha) * smoothedEMG;

  emgFiltered = smoothedEMG;

  total -= readings[readIndex];

  readings[readIndex] = (int)emgFiltered;

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

  if (autoMode) {

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
      targetPosition = 0;
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
      servoPositions[i] = currentPos;
    }
  }

  static unsigned long lastPrintTime = 0;

  if (millis() - lastPrintTime >= 500) {

    lastPrintTime = millis();

    Serial.print("EMG: ");
    Serial.print(emgValue);

    Serial.print(" | Activity: ");
    Serial.print(emgActivity);

    Serial.print(" | Mode: ");
    Serial.println(autoMode ? "AUTO" : "MANUAL");
  }

  delay(20);
}
