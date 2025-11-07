// === RC CAR + FOLLOW MODE + AVOIDER MODE (MG996R SERVO) ===
// Mode RC: F, B, L, R, S
// Mode FOLLOW: tekan X
// Mode AVOIDER: tekan W
// Servo otomatis kembali ke 90° saat kembali ke RC/FOLLOW mode

#include <Servo.h>

// === PIN SETUP ===
int ENA = 5;   // Motor Kiri (PWM)
int IN1 = 8;   // Motor Kiri +
int IN2 = 9;   // Motor Kiri -
int ENB = 6;   // Motor Kanan (PWM)
int IN3 = 10;  // Motor Kanan +
int IN4 = 11;  // Motor Kanan -

int trigPin = 2;
int echoPin = 3;
int servoPin = 4;

Servo myServo;

// === VARIABEL ===
char command;
bool followMode = false;
bool avoidMode = false;

// === SETUP ===
void setup() {
  Serial.begin(9600);
  
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myServo.attach(servoPin);
  myServo.write(90); // posisi awal depan

  stopCar();
  Serial.println("Mode: RC Manual");
}

// === LOOP UTAMA ===
void loop() {
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.println(command);

    // Toggle Follow Mode
    if (command == 'X' || command == 'x') {
      followMode = !followMode;
      avoidMode = false;
      myServo.write(90);
      if (followMode) Serial.println("Mode: FOLLOW OBJECT");
      else {
        stopCar();
        Serial.println("Mode: RC MANUAL");
      }
    }

    // Toggle Avoider Mode
    else if (command == 'W' || command == 'w') {
      avoidMode = !avoidMode;
      followMode = false;
      if (avoidMode) Serial.println("Mode: AVOIDER (Anti Tabrak)");
      else {
        stopCar();
        myServo.write(90);
        Serial.println("Mode: RC MANUAL");
      }
    }

    // RC Manual Control
    else if (command == 'F' || command == 'B' || command == 'L' || command == 'R' || command == 'S') {
      followMode = false;
      avoidMode = false;
      myServo.write(90);
      manualControl(command);
    }
  }

  if (followMode) followObject();
  else if (avoidMode) avoidObstacle();
}

// === RC MODE ===
void manualControl(char cmd) {
  switch (cmd) {
    case 'F': forward(); break;
    case 'B': backward(); break;
    case 'L': left(); break;
    case 'R': right(); break;
    case 'S': stopCar(); break;
  }
}

// === FOLLOW OBJECT ===
void followObject() {
  int distance = getDistance();

  if (distance > 12 && distance < 50) forward();
  else if (distance < 8 && distance > 0) backward();
  else stopCar();

  delay(100);
}

// === AVOIDER MODE ===
void avoidObstacle() {
  int distanceCenter, distanceLeft, distanceRight;

  // --- Cek depan ---
  myServo.write(90);
  delay(200);
  distanceCenter = getDistance();

  if (distanceCenter < 15) {
    stopCar();
    delay(200);

    // --- Cek kiri ---
    myServo.write(150);
    delay(400);
    distanceLeft = getDistance();

    // --- Cek kanan ---
    myServo.write(30);
    delay(400);
    distanceRight = getDistance();

    // Kembali ke depan
    myServo.write(90);
    delay(200);

    // Pilih arah terbaik
    backward();
    delay(400);
    stopCar();
    delay(200);

    if (distanceLeft > distanceRight) {
      left();
      delay(500);
    } else {
      right();
      delay(500);
    }
  } else {
    forward();
  }

  delay(100);
}

// === SENSOR ULTRASONIK ===
int getDistance() {
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 20000);
  distance = duration * 0.034 / 2;
  if (distance == 0) distance = 100;
  return distance;
}

// === GERAKAN MOTOR ===
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
