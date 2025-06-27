#include "NewPing.h"

// ---------- Ultrasonic Sensor Pins ----------
#define TRIGGER_PIN_LEFT 9
#define ECHO_PIN_LEFT 10
#define TRIGGER_PIN_RIGHT 6
#define ECHO_PIN_RIGHT 7
#define MAX_DISTANCE_INCH 24.0
#define MAX_DISTANCE_CM (int)(MAX_DISTANCE_INCH / 0.393701)

// ---------- Solenoid Valve Pins ----------
#define LEFT_VALVE_PIN 4
#define RIGHT_VALVE_PIN 5

// ---------- Measurement and Control ----------
const float RAIL_LENGTH_INCH = 24.0;
const float POSITION_TOLERANCE_INCH = 0.4;
const int PULSE_DURATION_MS = 1000;
const int PAUSE_BETWEEN_MOVES = 300;

NewPing sonar_left(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE_CM);
NewPing sonar_right(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE_CM);

// Function declarations
float averageDistanceInch(NewPing &sensor, int samples = 5);
float estimatePosition();
void moveLeftPulse();
void moveRightPulse();
void stopCarriage();
void checkSerialInput();

float targetPositionInch = -1;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_VALVE_PIN, OUTPUT);
  pinMode(RIGHT_VALVE_PIN, OUTPUT);
  stopCarriage();
  Serial.println("System ready.");
  Serial.println("Type a target position (inches from left sensor), or type 'left'/'right' to move manually.");
}

void loop() {
  checkSerialInput();

  if (targetPositionInch > 0) {
    float currentPosition = estimatePosition();

    if (currentPosition < 0) {
      Serial.println("⚠️ Invalid sensor readings.");
      return;
    }

    float error = targetPositionInch - currentPosition;

    Serial.print("Current: ");
    Serial.print(currentPosition, 2);
    Serial.print(" in, Target: ");
    Serial.print(targetPositionInch, 2);
    Serial.print(" in, Error: ");
    Serial.println(error, 2);

    if (abs(error) <= POSITION_TOLERANCE_INCH) {
      Serial.println("✅ Target reached.");
      stopCarriage();
      targetPositionInch = -1;
      Serial.println("Enter next target position (inches) or 'left'/'right':");
    } else if (error > 0) {
      Serial.println("➡️ Moving RIGHT");
      moveRightPulse();
    } else {
      Serial.println("⬅️ Moving LEFT");
      moveLeftPulse();
    }

    delay(PAUSE_BETWEEN_MOVES);
  }
}

// ---------- Control Functions ----------
void moveLeftPulse() {
  digitalWrite(LEFT_VALVE_PIN, HIGH);
  digitalWrite(RIGHT_VALVE_PIN, LOW);
  delay(PULSE_DURATION_MS);
  stopCarriage();
}

void moveRightPulse() {
  digitalWrite(LEFT_VALVE_PIN, LOW);
  digitalWrite(RIGHT_VALVE_PIN, HIGH);
  delay(PULSE_DURATION_MS);
  stopCarriage();
}

void stopCarriage() {
  digitalWrite(LEFT_VALVE_PIN, HIGH);
  digitalWrite(RIGHT_VALVE_PIN, HIGH);
}

// ----------1 Position Estimation ----------
float estimatePosition() {
  float dist_left = averageDistanceInch(sonar_left);
  float dist_right = averageDistanceInch(sonar_right);

  //以下对两个数据做了一致性校验（Distance Consistency Check），
  //新代码中将做双向平衡位置计算，得到一个数据（距离左侧的距离），
  //并且加入窗口平滑输出
  if (dist_left <= 0 || dist_right <= 0 || dist_left + dist_right > RAIL_LENGTH_INCH + 2) {
    return -1; // invalid
  }

  return dist_left;
}

// ----------1.1 Averaging ----------
//这里在后面的代码中会被取代，因为新的算法是取中值
float averageDistanceInch(NewPing &sensor, int samples) {//samples: number of samples
  long sum = 0;
  int valid = 0;
  for (int i = 0; i < samples; i++) {
    int d = sensor.ping_cm(); //我想修改这部分变成ping_in()，这样读取值就是inch了
    if (d > 0) {
      sum += d;
      valid++;
    }
    delay(20);  //为什么delay？
  }
  return valid > 0 ? (float)sum / valid * 0.393701 : -1;
}

// ---------- Serial Command Handling ----------
void checkSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Manual control
    if (input.equalsIgnoreCase("left")) {
      Serial.println("⬅️ Manual LEFT");
      moveLeftPulse();
      return;
    }
    if (input.equalsIgnoreCase("right")) {
      Serial.println("➡️ Manual RIGHT");
      moveRightPulse();
      return;
    }

    // Try to parse as number
    float val = input.toFloat();
    if (val > 0 && val < RAIL_LENGTH_INCH) {
      targetPositionInch = val;
      Serial.print("🎯 New target set: ");
      Serial.print(targetPositionInch, 2);
      Serial.println(" in");
    } else {
      Serial.println("❌ Invalid input. Type a number (inches), or 'left'/'right'");
    }
  }
}
