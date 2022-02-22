#include "SSD1306Wire.h"
#define LED_1 14
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
  enum class BombStates {INIT, CONFIG, ARMED};
  static BombStates bombState =  BombStates::INIT;
  static uint8_t counter = 20;

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
        display.clear();
        display.drawString(10, 20, String(counter));
        display.display();
        // Esperar a que se presione algun boton (ARMED, UP, DOWN)
        
      /*  if () {

        }
        else if () {

        }*/
        
        // Evento 2: presionar UP para subir el tiempo
        // Evento 3: presionar DOWN para bajar el tiempo.

        /*

                display.clear();
                display.drawString(10, 20, String(counter));
                display.display();

        */


        // Evento 4: presionar armado para pasar al estado ARMED

        break;
      }
    case BombStates::ARMED: {



        break;
      }
    default:
      break;
  }
}

void loop() {
  task();
}
