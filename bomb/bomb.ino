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
void pricipalTask();

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void setup() {
  btnsTask();
  pricipalTask();
  Serial.begin(115200);
}

void loop() {
  btnsTask();
  pricipalTask();
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


void pricipalTask() {
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

        display.clear();
        display.drawString(10, 20, String(counter));
        display.display();

        if (evBtns == true) {
          evBtns = false;

          if (evBtnsData == UP_BTN) {
            if (counter < 60) {
              counter++;
            }

          }
          else if (evBtnsData == DOWN_BTN) {
            if (counter > 10) {
              counter--;
            }
          }
          else if (evBtnsData == ARM_BTN) {
            bombState = BombStates::ARMED;
            Serial.println("BombStates::ARMED");
          }
        }
        break;
      }

    case BombStates::ARMED: {

        const uint8_t vecLength = 7;
        static uint8_t vecTryPassword[vecLength];
        static uint8_t dataCounter = 0;
        static uint8_t vecTruePassword[vecLength] = {UP_BTN, UP_BTN, DOWN_BTN, DOWN_BTN, UP_BTN, DOWN_BTN, ARM_BTN};
        bool statePassword = false;

        const uint32_t interval = 500;
        static uint32_t previousMillis = 0;
        static uint8_t ledState_BOMB_OUT = LOW;
        uint32_t currentMillis = millis();

        enum class ArmedStates {COUNTER_PASSWORD, ABORT};
        static ArmedStates armedState =  ArmedStates::COUNTER_PASSWORD;

        switch (armedState) {
          case ArmedStates::COUNTER_PASSWORD: {

              //Cuenta regresiva

              if (currentMillis - previousMillis >= interval) {
                previousMillis = currentMillis;
                if (ledState_BOMB_OUT == LOW) {
                  ledState_BOMB_OUT = HIGH;
                }
                else {
                  ledState_BOMB_OUT = LOW;
                  counter--;
                  display.clear();
                  display.drawString(10, 20, String(counter));
                  display.display();
                }
                digitalWrite(LED_COUNT, ledState_BOMB_OUT);
                if (counter == 0) {
                  digitalWrite(LED_COUNT, LOW);
                  digitalWrite(BOMB_OUT, HIGH);
                  display.clear();
                  display.drawString(10, 20, "BOOM");
                  display.display();
                  delay(3000);
                  digitalWrite(LED_COUNT, HIGH);
                  digitalWrite(BOMB_OUT, LOW);
                  counter = 20;
                  display.clear();
                  display.drawString(10, 20, String(counter));
                  display.display();
                  bombState = BombStates::CONFIG;
                }
              }

              //ingrese la pasword


              if (evBtns == true) {
                evBtns = false;
                if (dataCounter < vecLength) {
                  if (evBtnsData == UP_BTN) {
                    vecTryPassword[dataCounter] = evBtnsData;
                  }
                  else if (evBtnsData == DOWN_BTN) {
                    vecTryPassword[dataCounter] = evBtnsData;
                  }
                  else if (evBtnsData == ARM_BTN) {
                    vecTryPassword[dataCounter] = evBtnsData;
                  }
                  dataCounter++;
                }
              }
              else if (dataCounter == vecLength) { //verifico que se el array este lleno y verificar si la contraseña es correcta y limpiar vector
                disarmTask(vecTryPassword, vecTruePassword, vecLength, &statePassword); //statePassword va ser igual a la funcion que retorna un bool true si la clave es correcta
                if (statePassword == true) { //si la clave es correcta entonces pase al estado CONFIG
                  digitalWrite(LED_COUNT, HIGH);
                  digitalWrite(BOMB_OUT, LOW);
                  counter = 20;
                  display.clear();
                  display.drawString(10, 20, String(counter));
                  display.display();
                  for (uint8_t j = 0; j < vecLength; j++) {
                    vecTryPassword[j] = 0;
                  }
                  bombState = BombStates::CONFIG;
                }
                else { //sino, i = 0, limpiar el vector y sigue la cuenta regresiva
                  dataCounter = 0;
                  for (uint8_t k = 0; k < vecLength; k++) {
                    vecTryPassword[k] = 0;
                  }
                }
              }


            }
            break;
          case ArmedStates::ABORT: {

            }
            break;
          default: {
            }
            break;
        }
      }
      break;
    default: {
      }
      break;
  }
}

void disarmTask(uint8_t *vecTryData, uint8_t *vecTrueData, uint8_t vecLengthData, bool *res) {
  for (uint8_t i = 0; i < vecLengthData; i++) {
    if (vecTryData[i] == vecTrueData[i]) {
      *res = true;
    }
    else {
      *res = false;
    }
  }
}
