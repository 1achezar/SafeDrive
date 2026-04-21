#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//LCD
//I2C LCD (address: 0x27, 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Ultrasonic Sensor Pins
// Front ultrasonic sensor
#define TRIG1 4
#define ECHO1 3

//Back ultrasonic sensor
#define TRIG2 11
#define ECHO2 10

//Passive Buzzer
#define BUZZER_PIN 9

//Parameters
float gateDistance = 1.30;   
float speedLimit   = 40.0;   
float triggerDist  = 15.0;   

//State Variables
bool firstGateTriggered = false;
bool speedCalculated    = false;

unsigned long t1 = 0;
unsigned long t2 = 0;

float getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  return duration * 0.034 / 2;
}

void setup() {
  // Set pin modes for ultrasonic sensors
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Set buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Show startup message
  lcd.setCursor(0, 0);
  lcd.print("Speed Detector");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
}

void loop() {
  // Read distances from both ultrasonic sensors
  float d1 = getDistanceCM(TRIG1, ECHO1);
  delay(40);  // Prevent ultrasonic interference
  float d2 = getDistanceCM(TRIG2, ECHO2);

  // First sensor detects object
  if (!firstGateTriggered && d1 > 0 && d1 < triggerDist) {
    firstGateTriggered = true;
    t1 = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vehicle detected");
    lcd.setCursor(0, 1);
    lcd.print("Measuring...");
  }

  // Second sensor detects object
  if (firstGateTriggered && !speedCalculated && d2 > 0 && d2 < triggerDist) {
    t2 = millis();
    speedCalculated = true;

    float deltaT = (t2 - t1) / 1000.0;

    lcd.clear();

    if (deltaT > 0.05) { 
      float speed = (gateDistance / deltaT) * 3.6;

      lcd.setCursor(0, 0);
      lcd.print("Speed:");
      lcd.print(speed, 1);
      lcd.print("km/h");

      lcd.setCursor(0, 1);
      if (speed > speedLimit) {
        lcd.print("Over Speed!");

        // Beep 3 times
        for (int i = 0; i < 3; i++) {
          tone(BUZZER_PIN, 2000);
          delay(200);
          noTone(BUZZER_PIN);
          delay(200);
        }
      } else {
        lcd.print("Normal");
      }
    } else {
      lcd.print("Invalid data");
    }

    delay(3000);

    // Reset system
    firstGateTriggered = false;
    speedCalculated = false;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Speed Detector");
    lcd.setCursor(0, 1);
    lcd.print("Waiting...");
  }
}