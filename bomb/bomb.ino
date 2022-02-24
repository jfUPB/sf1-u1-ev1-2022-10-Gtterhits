#include "SSD1306Wire.h"
#define LED_1 14
#define BOMB_OUT 25
#define LED_COUNT 26
#define UP_BTN 32
#define DOWN_BTN 33
#define ARM_BTN 13

bool evBtns = false;
uint8_t evBtnsData = 0;

void btnsTask();
void task();

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void setup() {
  btnsTask();
  task();
  Serial.begin(115200);
}

void loop() {
  btnsTask();
  task();
}

void btnsTask() {
  enum class BtnsStates {INIT, WAITING_PRESS , WAITING_STABLE, WAITING_RELEASE};
  static BtnsStates btnsState =  BtnsStates::INIT;
  static uint32_t referenceTime;
  const uint32_t STABLETIMEOUT = 30;
  static uint8_t lastBtn = 0;

  switch (btnsState) {
    case BtnsStates::INIT: {
        pinMode(ARM_BTN, INPUT_PULLUP);
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(UP_BTN, INPUT_PULLUP);
        btnsState = BtnsStates::WAITING_PRESS;
        break;
      }
    case BtnsStates::WAITING_PRESS: {

        if (digitalRead(UP_BTN) == LOW) {
          referenceTime = millis();
          lastBtn = UP_BTN;
          btnsState = BtnsStates::WAITING_STABLE;

        }
        else if (digitalRead(DOWN_BTN) == LOW) {
          lastBtn = DOWN_BTN;
          referenceTime = millis();
          btnsState = BtnsStates::WAITING_STABLE;

        }
        else if (digitalRead(ARM_BTN) == LOW) {
          lastBtn = ARM_BTN;
          referenceTime = millis();
          btnsState = BtnsStates::WAITING_STABLE;
        }

        break;
      }
    case BtnsStates::WAITING_STABLE: {
        if (lastBtn == UP_BTN) {
          if (digitalRead(UP_BTN) == HIGH) {
            btnsState = BtnsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
            btnsState = BtnsStates::WAITING_RELEASE;
          }
        }
        else if (lastBtn == DOWN_BTN) {
          if (digitalRead(DOWN_BTN) == HIGH) {
            btnsState = BtnsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
            btnsState = BtnsStates::WAITING_RELEASE;
          }
        }
        else if (lastBtn == ARM_BTN) {
          if (digitalRead(ARM_BTN) == HIGH) {
            btnsState = BtnsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
            btnsState = BtnsStates::WAITING_RELEASE;
          }
        }

        break;
      }
    case BtnsStates::WAITING_RELEASE: {

        if (lastBtn == UP_BTN) {
          if (digitalRead(UP_BTN) == HIGH) {
            evBtns = true;
            evBtnsData = UP_BTN;
            btnsState = BtnsStates::WAITING_PRESS;
            Serial.println("UP_BTN");
          }
        }
        else if (lastBtn == DOWN_BTN) {
          if (digitalRead(DOWN_BTN) == HIGH) {
            evBtns = true;
            evBtnsData = DOWN_BTN;
            btnsState = BtnsStates::WAITING_PRESS;
            Serial.println("DOWN_BTN");
          }
        }
        else if (lastBtn == ARM_BTN) {
          if (digitalRead(ARM_BTN) == HIGH) {
            evBtns = true;
            evBtnsData = ARM_BTN;
            btnsState = BtnsStates::WAITING_PRESS;
            Serial.println("ARM_BTN");
          }
        }

        break;
      }
    default:
      Serial.println("Ocurrio un error.");
      break;
  }

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


        display.clear();
        display.drawString(10, 20, String(counter));
        display.display();

        // Pasa a configurar
        bombState = BombStates::CONFIG;

        break;
      }

    case BombStates::CONFIG: {

        if (evBtns == true) {
          evBtns = false;

          if (evBtnsData == UP_BTN) {
            if (counter < 60) {
              counter++;
            }
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
          }
          else if (evBtnsData == DOWN_BTN) {
            if (counter > 10) {
              counter--;
            }
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
          }
          else if (evBtnsData == ARM_BTN) {
            bombState = BombStates::ARMED;
            Serial.println("BombStates::ARMED");

          }
        }

        break;
      }

    case BombStates::ARMED: {
        const uint32_t interval = 1000;
        const uint32_t stableTimeOut = 5000;
        
        static uint32_t previousMillis0 = 0;
        static uint8_t ledState_BOMB_OUT = LOW;
        uint32_t currentMillis0 = millis();


        if (currentMillis0 - previousMillis0 >= interval) {
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
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
          }
        }

        if (counter == 0) {
          digitalWrite(LED_COUNT, LOW);
          digitalWrite(BOMB_OUT, HIGH);
          
          display.clear();
          display.drawString(10, 20, "BOOM");
          display.display();
          
          currentMillis0 = millis();
          
          if (currentMillis0 >= stableTimeOut) {
            digitalWrite(LED_COUNT, HIGH);
            digitalWrite(BOMB_OUT, LOW);
            counter = 20;
            display.clear();
            display.drawString(10, 20, String(counter));
            display.display();
            
            bombState = BombStates::CONFIG;
          }
        }



        break;
      }
    default:
      break;
  }

}
