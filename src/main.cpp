#include <Arduino.h>
#include "global.h"
#include "tft.h"
#include "pwm.h"
#include "capteurs.h"
#include "button.h"
#include "state_saver.h"
void setup()
{
  Serial.begin(115200);
  /*if (getBatteryState() == CRITICAL_STATE)
  {
    Serial.print("Critical Battery state entering in sleep mode");
    esp_deep_sleep_start();
  }*/
  initSave();
  loadSavedData();
  initPWM();
  initTft();
  initCapteur();
  initButtons();
  Serial.println(global.current_PWM);
}
void loop()
{
  delay(2000);
  global.consigne = 400;
  delay(2000);
  global.consigne = 100;
}