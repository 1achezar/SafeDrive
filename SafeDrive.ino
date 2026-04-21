#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//LCD & OLED Config
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Pins
#define TRIG1 4
#define ECHO1 3
#define TRIG2 11
#define ECHO2 10
#define BUZZER_PIN 9
const int buttonPin = 2;

//Parameters
float gateDistance = 1.30; 
float speedLimit   = 40.0; 
float triggerDist  = 15.0; 

bool firstGateTriggered = false;
bool speedCalculated    = false;
unsigned long t1 = 0;
unsigned long t2 = 0;
int lastButtonState = HIGH;  
bool isSpeedMode = true; 

//Functions

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

void updateOledText() {
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 8); 
  if (isSpeedMode) {
    display.println(F(" SPEED"));
    lcd.clear();
    lcd.print("Speed Detector");
    lcd.setCursor(0, 1);
    lcd.print("Ready...");
  } else {
    display.println(F("PARKING"));
    lcd.clear();
    lcd.print("Parking Mode");
  }
  display.display();
}

void setup() {
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.init(); lcd.backlight();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { }
  updateOledText();
}

void loop() {
  //MODE SWITCH
  int currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    isSpeedMode = !isSpeedMode;
    updateOledText();
    firstGateTriggered = false;
    speedCalculated = false;
    delay(200); 
  }
  lastButtonState = currentButtonState;

  //MODE 1: PARKING (Distance Sensor)
  if (!isSpeedMode) {
    float d1 = getDistanceCM(TRIG1, ECHO1);
    
    lcd.setCursor(0, 1);
    if (d1 > 0 && d1 < 100) { // Only show distance if object is within 1 meter
      lcd.print("Dist: ");
      lcd.print(d1, 1);
      lcd.print(" cm     "); 
      
      if (d1 < 5.0) {
        tone(BUZZER_PIN, 1000, 20);
      }
    } else {
      lcd.print("No car detected ");
    }
  } 

  //MODE 2: SPEED (Race Timing)
  else {
    float d1 = getDistanceCM(TRIG1, ECHO1);
    delay(30);
    float d2 = getDistanceCM(TRIG2, ECHO2);

    if (!firstGateTriggered && d1 > 0 && d1 < triggerDist) {
      firstGateTriggered = true;
      t1 = millis();
      lcd.clear();
      lcd.print("Vehicle detected");
    }

    if (firstGateTriggered && !speedCalculated && d2 > 0 && d2 < triggerDist) {
      t2 = millis();
      speedCalculated = true;
      float deltaT = (t2 - t1) / 1000.0;
      lcd.clear();
      if (deltaT > 0.05) { 
        float speed = (gateDistance / deltaT) * 3.6;
        lcd.print("Speed: "); lcd.print(speed, 1); lcd.print("km/h");
        lcd.setCursor(0, 1);
        if (speed > speedLimit) {
          lcd.print("OVER SPEED!");
          tone(BUZZER_PIN, 2000, 500);
        } else {
          lcd.print("Status: Normal");
        }
      }
      delay(2500); 
      firstGateTriggered = false;
      speedCalculated = false;
      updateOledText();
    }
  }
}