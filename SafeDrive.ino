#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//LCD Config (16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

//OLED Config (128x32)
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

//Parameters & Variables
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
  } else {
    display.println(F("PARKING"));
  }
  display.display();
}

void setup() {
  // Pin Modes
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Speed Detector");
  lcd.setCursor(0, 1);
  lcd.print("Ready...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  }
  updateOledText();
}

void loop() {
  //OLED Button Logic
  int currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    isSpeedMode = !isSpeedMode;
    updateOledText();
    delay(50);
  }
  lastButtonState = currentButtonState;

  //Speed Detection Logic
  float d1 = getDistanceCM(TRIG1, ECHO1);
  delay(30);
  float d2 = getDistanceCM(TRIG2, ECHO2);

  //First sensor detects object
  if (!firstGateTriggered && d1 > 0 && d1 < triggerDist) {
    firstGateTriggered = true;
    t1 = millis();
    lcd.clear();
    lcd.print("Vehicle detected");
  }

  //Second sensor detects object
  if (firstGateTriggered && !speedCalculated && d2 > 0 && d2 < triggerDist) {
    t2 = millis();
    speedCalculated = true;
    float deltaT = (t2 - t1) / 1000.0;

    lcd.clear();
    if (deltaT > 0.05) { 
      float speed = (gateDistance / deltaT) * 3.6;
      lcd.setCursor(0, 0);
      lcd.print("Speed: ");
      lcd.print(speed, 1);
      lcd.print("km/h");

      lcd.setCursor(0, 1);
      if (speed > speedLimit) {
        lcd.print("OVER SPEED!");
        for (int i = 0; i < 3; i++) {
          tone(BUZZER_PIN, 2000);
          delay(150);
          noTone(BUZZER_PIN);
          delay(100);
        }
      } else {
        lcd.print("Status: Normal");
      }
    } else {
      lcd.print("Invalid Reading");
    }

    delay(2500); 
    
  
    firstGateTriggered = false;
    speedCalculated = false;
    lcd.clear();
    lcd.print("Speed Detector");
    lcd.setCursor(0, 1);
    lcd.print("Waiting...");
  }
}