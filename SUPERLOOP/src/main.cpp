#include <Arduino.h>

struct Config {
  static constexpr uint8_t BLUE_LED_PIN = 4;
  static constexpr uint8_t RED_LED_PIN = 5;
  static constexpr uint8_t GREEN_LED_PIN = 6;

  static constexpr uint32_t BLUE_INTERVAL = 200;
  static constexpr uint32_t RED_INTERVAL = 500;
  static constexpr uint32_t GREEN_INTERVAL = 1000;
};

class LED {
  private:
    uint8_t pin;
    uint32_t interval;
    uint32_t lastToggleTime;
    bool state;
  public:
  LED(uint8_t pin, uint32_t interval) : pin(pin), interval(interval), lastToggleTime(0), state(false) {}
  
  void init() {
    pinMode(pin, OUTPUT);
  }
  void update() {
    uint32_t currentTime = millis();
    if (currentTime - lastToggleTime >= interval) {
      state = !state;
      digitalWrite(pin, state ? HIGH : LOW);
      lastToggleTime = currentTime;
    }
  }
};

LED blueLED(Config::BLUE_LED_PIN, Config::BLUE_INTERVAL);
LED redLED(Config::RED_LED_PIN, Config::RED_INTERVAL);
LED greenLED(Config::GREEN_LED_PIN, Config::GREEN_INTERVAL);

void setup() {
  Serial.begin(115200);

  blueLED.init();
  redLED.init();
  greenLED.init();
}

void loop() {
  blueLED.update();
  redLED.update();
  greenLED.update();
}
