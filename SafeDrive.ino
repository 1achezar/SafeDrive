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
#define BUZZER_PIN 8 

#define PIN_RED   7  
#define PIN_GREEN 6  
#define PIN_BLUE  5  

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

void setLED(int r, int g, int b) {
  digitalWrite(PIN_RED, r);
  digitalWrite(PIN_GREEN, g);
  digitalWrite(PIN_BLUE, b);
}

float getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration <= 0) return -1;
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
    setLED(LOW, HIGH, LOW); 
  } else {
    display.println(F("PARKING"));
    lcd.clear();
    lcd.print("Parking Mode");
    setLED(LOW, LOW, LOW);
  }
  display.display();
}

void setup() {
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.init(); lcd.backlight();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { }
  updateOledText();
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    isSpeedMode = !isSpeedMode;
    updateOledText();
    firstGateTriggered = false;
    speedCalculated = false;
    delay(200); 
  }
  lastButtonState = currentButtonState;

  //MODE 1: PARKING
  if (!isSpeedMode) {
    float d1 = getDistanceCM(TRIG1, ECHO1);
    lcd.setCursor(0, 1);
    
    if (d1 > 0 && d1 < 50) {
      lcd.print("Dist: "); lcd.print(d1, 1); lcd.print(" cm     ");
      
      int alertDelay = map(constrain(d1, 5, 50), 5, 50, 50, 500);
      
      setLED(HIGH, LOW, LOW); 
      tone(BUZZER_PIN, 2000); 
      delay(50);              
      
      setLED(LOW, LOW, LOW);  
      noTone(BUZZER_PIN);     
      
      if (d1 < 5.0) {
        setLED(HIGH, LOW, LOW);
        tone(BUZZER_PIN, 2000);
        delay(alertDelay);
      } else {
        delay(alertDelay);      
      }
    } else {
      lcd.print("Waiting for car ");
      setLED(LOW, LOW, LOW);
      noTone(BUZZER_PIN);
    }
  } 
  
  //MODE 2: SPEED
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
      
      if (deltaT > 0.01) {
        float speed = (gateDistance / deltaT) * 3.6;
        lcd.print("Speed: "); lcd.print(speed, 1); lcd.print("km/h");
        lcd.setCursor(0, 1);

        if (speed > speedLimit) {
          lcd.print("OVER SPEED!");
          setLED(HIGH, LOW, LOW);        
          for(int i = 0; i < 3; i++) {
            tone(BUZZER_PIN, 2500); delay(100);
            noTone(BUZZER_PIN); delay(50);
          }
        } else {
          lcd.print("Status: Normal");
          setLED(LOW, HIGH, LOW);
          tone(BUZZER_PIN, 1500, 100);
        }
      }
      
      delay(2500); 
      firstGateTriggered = false;
      speedCalculated = false;
      updateOledText(); 
    }
  }
}