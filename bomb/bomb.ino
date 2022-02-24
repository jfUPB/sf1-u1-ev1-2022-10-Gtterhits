#include "SSD1306Wire.h"
#define LED_1 14
#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 32
#define DOWN_BTN 33
#define ARM_BTN 13
const uint32_t interval0 = 1000;

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);


void setup() {
  task();
  Serial.begin(115200);
}


void task() {
  enum class BombStates {INIT, CONFIG, ARMED};
  static BombStates bombState =  BombStates::INIT;
  static uint8_t counter = 20;

  uint32_t currentMillis0 = millis();

  bool bool_DOWN = false;
  bool bool_UP = false;
  bool bool_ARM = false;

  switch (bombState) {
    case BombStates::INIT: {
        pinMode(LED_1, OUTPUT);
        pinMode(BOMB_OUT, OUTPUT);
        pinMode(LED_COUNT, OUTPUT);
        pinMode(ARM_BTN, INPUT_PULLUP);
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(UP_BTN, INPUT_PULLUP);

        // LED_COUNT encendido
        digitalWrite(LED_COUNT, HIGH);
        digitalWrite(BOMB_OUT, LOW);
        digitalWrite(LED_1, LOW); // Error, led encendido siempre

        // Init display
        display.init();
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);

        // Pasa a configurar
        bombState = BombStates::CONFIG;
        break;
      }
    case BombStates::CONFIG: {
        // Evento 1: mostrar el 20 en la pantalla
        // Actualizar la pantalla
        /*
          display.clear();
          display.drawString(10, 20, String(counter));
          display.display();
        */
        display.clear();
        display.drawString(10, 20, String(counter));
        display.display();




        // Esperar a que se presione algun boton (ARMED, UP, DOWN)
        if (digitalRead(DOWN_BTN) == LOW) { // Evento 2: presionar UP para subir el tiempo
          bool_DOWN = true;
          if (bool_DOWN) {
            if (counter > 10) {
              counter--;
              bool_DOWN = false;
            }
          }
        }
        else if (digitalRead(UP_BTN) == LOW) { // Evento 2: presionar UP para subir el tiempo
          bool_UP = true;
          if (bool_UP) {
            if (counter < 60) {
              counter++;
              bool_UP = false;
            }
          }
        }
        else if (digitalRead(ARM_BTN) == LOW) { // Evento 4: presionar armado para pasar al estado ARMED
          bool_ARM = true;
          if (bool_ARM){
            bombState = BombStates::ARMED;
            bool_ARM = false;
          }
          

        }
        break;
      }
    case BombStates::ARMED: {
        static uint32_t previousMillis0 = 0;
        static uint8_t ledState_BOMB_OUT = LOW;
        uint32_t currentMillis0 = millis();

        if (currentMillis0 - previousMillis0 >= interval0) {
          previousMillis0 = currentMillis0;
          if (ledState_BOMB_OUT == LOW) {
            ledState_BOMB_OUT = HIGH;
          }
          else {
            ledState_BOMB_OUT = LOW;
          }
          digitalWrite(LED_COUNT, ledState_BOMB_OUT);
          if (ledState_BOMB_OUT == HIGH) {
            counter--;
          }
        }
        display.clear();
        display.drawString(10, 20, String(counter));
        display.display();
        if (counter == 0) {
          digitalWrite(LED_COUNT, LOW);
          digitalWrite(BOMB_OUT, HIGH);
        }
        break;
      }
    default:
      break;
  }
}

void loop() {
  task();
}
