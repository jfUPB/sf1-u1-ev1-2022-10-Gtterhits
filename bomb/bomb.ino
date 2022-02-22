#include "SSD1306Wire.h"

#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);



void setup() {
  task();
}


void task() {
  enum class BombStates {INIT, };
  static BombStates bombState =  BombStates::INIT;

  switch (bombState) {
    case BombStates::INIT: {
        pinMode(BOMB_OUT, OUTPUT);
        pinMode(LED_COUNT, OUTPUT);
        pinMode(ARM_BTN, INPUT_PULLUP);
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(UP_BTN, INPUT_PULLUP);
        display.init();
        display.setContrast(255);
        display.clear();
        break;
      }
    default:
      break;


  }


}

void loop() {
  task();
}

/*
  void Config_Bomb() { //
  static uint32_t
  static uint8_t

  }

  void Arm_Bomb() { //

  }

  void*/
