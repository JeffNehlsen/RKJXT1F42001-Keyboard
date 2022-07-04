#include <Arduino.h>
#include <Encoder.h>
#include <Keyboard.h>
#include <AceButton.h>
#include <arduino-timer.h>
using namespace ace_button;

#define ENCODER_OPTIMIZE_INTERRUPTS
#define ENCODER_A_PIN 10
#define ENCODER_B_PIN 16

#define BTN_PUSH_PIN 9
#define BTN_A_PIN 8
#define BTN_B_PIN 7
#define BTN_C_PIN 6
#define BTN_D_PIN 5

#define RESET_TIMER_DELAY 10

Encoder encoder(ENCODER_A_PIN, ENCODER_B_PIN);

Timer<1> timer;

AceButton pushButton(BTN_PUSH_PIN);
AceButton aButton(BTN_A_PIN);
AceButton bButton(BTN_B_PIN);
AceButton cButton(BTN_C_PIN);
AceButton dButton(BTN_D_PIN);

int oldPosition = -999;

volatile bool directionPressed = false;

bool resetDirection(void *argument) {
  directionPressed = false;
  Serial.print("Finishing timer. directionPressed is now ");
  Serial.println(directionPressed);
  return false;
}

void resetTimer() {
  Serial.println("Resetting timer!");
  if (!timer.empty()) {
    timer.cancel();
  }

  timer.in(RESET_TIMER_DELAY, resetDirection);
}

void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState) {
  Serial.print(F("handleEvent(): pin: "));
  Serial.print(button->getPin());
  Serial.print(F("; eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  if (eventType == AceButton::kEventPressed) {
    switch (button->getPin()) {
    case BTN_PUSH_PIN:
      break;
    case BTN_A_PIN:
    case BTN_B_PIN:
    case BTN_C_PIN:
    case BTN_D_PIN:
      directionPressed = true;
      Serial.print("DIRECTION PRESSED. directionPressed: ");
      Serial.println(directionPressed);
      break;
    }
  }

  if (eventType == AceButton::kEventReleased) {
    switch (button->getPin()) {
    case BTN_PUSH_PIN:
      Serial.print("PUSH UP. directionPressed: ");
      Serial.println(directionPressed);
      if (!directionPressed) {
        Serial.println("directionPressed is false. Doing a push action!");
        Keyboard.write(KEY_F13);
      }
      break;
    case BTN_A_PIN:
      Keyboard.write(KEY_RIGHT_ARROW);
      Serial.println("A PRESSED");
      resetTimer();
      break;
    case BTN_B_PIN:
      Keyboard.write(KEY_UP_ARROW);
      Serial.println("B PRESSED");
      resetTimer();
      break;
    case BTN_C_PIN:
      Keyboard.write(KEY_LEFT_ARROW);
      Serial.println("C PRESSED");
      resetTimer();
      break;
    case BTN_D_PIN:
      Keyboard.write(KEY_DOWN_ARROW);
      Serial.println("D PRESSED");
      resetTimer();
      break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);

  pinMode(BTN_PUSH_PIN, INPUT_PULLUP);
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(BTN_C_PIN, INPUT_PULLUP);
  pinMode(BTN_D_PIN, INPUT_PULLUP);

  pinMode(17, OUTPUT);
  Serial.println("Starting!");

  Keyboard.begin();

  ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
}

void loop() {
  timer.tick();
  pushButton.check();
  aButton.check();
  bButton.check();
  cButton.check();
  dButton.check();

  long newPosition = encoder.read();
  if (newPosition != oldPosition) {
    Serial.print("oldPosition: ");
    Serial.print(oldPosition);

    if (oldPosition > newPosition) {
      Keyboard.write(KEY_F14);
      Serial.println(" SCROLL UP PRESSED");
    }
    else {
      Keyboard.write(KEY_F15);
      Serial.println(" SCROLL DOWN PRESSED");
    }

    oldPosition = newPosition;
    Serial.print(" -- newPosition: ");
    Serial.println(newPosition);
  }
}
