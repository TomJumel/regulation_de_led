#ifndef PWM_FUNCTIONS
#define PWM_FUNCTIONS
#include <Arduino.h>
#include "global.h"

void _pwmTask(void *arg)
{
    uint8_t continiusMode = 0;
    for (;;)
    {
        switch (global.current_mode)
        {
        case OFF:
            ledcWrite(PWM_CHANNEL, global.current_PWM = PWM_MAX);
            global.consigne = 0;
            break;
        case FLASH:
                global.consigne = 600;
                delay(50);
                global.consigne = 0;
                delay(950);
            break;
        case CONTINU:
            if (global.button_left == SINGLE_CLICK)
            {
                //+
                if (continiusMode < 2)
                    continiusMode++;

                global.button_left = 0;
            }
            if (global.button_right == SINGLE_CLICK)
            {
                //-
                if (continiusMode > 0)
                    continiusMode--;
                global.button_right = 0;
            }
            //Serial.println(continiusMode);
            switch (continiusMode)
            {
            case 0:
                global.consigne = 200;
                break;
            case 1:
                global.consigne = 400;
                break;
            case 2:
                global.consigne = 700;
                break;
            default:
                global.consigne = 0;
                break;
            }
            break;
        }
        global.current_PWM_percent = 100 - pwmInPercent; // PNP -> inversé
        global.current_PWM_percent = constrain(global.current_PWM_percent, 0, 100);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}
void initPWM()
{
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, PWM_MAX);
    xTaskCreate(_pwmTask, "PWM_TASK", 4080, NULL, 1, NULL);
}
#endif