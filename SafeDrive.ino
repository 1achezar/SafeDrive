#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonPin = 2;    
int lastButtonState = HIGH;  
bool isSpeedMode = true;     

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }

  showText(); 
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    isSpeedMode = !isSpeedMode; 
    showText();                 
    delay(50);                  
  }

  lastButtonState = currentButtonState;
}

void showText() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 5); 

  if (isSpeedMode) {
    display.println(F(" SPEED"));
  } else {
    display.println(F("PARKING"));
  }

  display.display();
}