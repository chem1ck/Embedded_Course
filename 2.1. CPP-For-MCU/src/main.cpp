#include <Arduino.h>

enum class LED_STATE {
  ON,
  OFF
};

enum class SYSTEM_MODE {
  BLINK,
  ALWAYS_ON,
  ALWAYS_OFF
};

struct Config {
  static constexpr size_t LED_PIN = 4;
  static constexpr size_t BUTTON_PIN = 18;
  static constexpr size_t interval = 1000;
  static constexpr uint32_t debounceDelay = 50;
};

class LED {
public:
  void init(){
    pinMode(Config::LED_PIN, OUTPUT);
  }
  void setState(LED_STATE state){
    digitalWrite(Config::LED_PIN, state == LED_STATE::ON ? HIGH : LOW);
  }
};


LED myLED;
volatile bool buttonPressed = false;

void IRAM_ATTR buttonISR() {
  buttonPressed = true;
}

void setup() {
  Serial.begin(115200);
  myLED.init();

  pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Config::BUTTON_PIN), buttonISR, FALLING);
}

void loop() {
  uint32_t startTimers = micros();

  static SYSTEM_MODE currentMode = SYSTEM_MODE::BLINK;

  if(buttonPressed == true) {
    buttonPressed = false;
    static uint32_t lastInterruptTime = 0;
    uint32_t interruptTime = millis();

    if(interruptTime - lastInterruptTime > Config::debounceDelay) {
      if(currentMode == SYSTEM_MODE::BLINK) {
        currentMode = SYSTEM_MODE::ALWAYS_ON;
        myLED.setState(LED_STATE::ON);
      } else if(currentMode == SYSTEM_MODE::ALWAYS_ON) {
        currentMode = SYSTEM_MODE::ALWAYS_OFF;
        myLED.setState(LED_STATE::OFF);
      } else {
        currentMode = SYSTEM_MODE::BLINK;
      }
      lastInterruptTime = interruptTime;
    }
  }

  uint32_t currentMillis = millis();
  static LED_STATE ledState = LED_STATE::OFF;
  static uint32_t previousMillis = 0;
  static uint32_t countIterations = 0;
  static uint32_t accumulatedTime = 0;

  if(currentMode == SYSTEM_MODE::BLINK) {
    if(currentMillis - previousMillis >= Config::interval) {
    previousMillis = currentMillis;

    if(ledState == LED_STATE::OFF) {
      ledState = LED_STATE::ON;
    } else {
      ledState = LED_STATE::OFF;
    }

    myLED.setState(ledState);
  }
  }
  
  uint32_t elapsedTime = micros() - startTimers;
  accumulatedTime += elapsedTime;
  countIterations++;

  if(countIterations % 1000 == 0) {
    float averageTime = static_cast<float>(accumulatedTime) / countIterations;
    Serial.printf("Average time per loop iteration: %.2f microseconds\n", averageTime);
    accumulatedTime = 0;
    countIterations = 0;
    
  }
}

