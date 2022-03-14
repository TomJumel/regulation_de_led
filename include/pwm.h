#ifndef PWM_FUNCTIONS
#define PWM_FUNCTIONS
#include <Arduino.h>
#include "global.h"

void _pwmTask(void *arg)
{
    long time = millis();
    for (;;)
    {
        switch (global.current_mode)
        {
        case OFF:
            ledcWrite(PWM_CHANNEL, global.current_PWM = PWM_MAX);
            break;
        case FLASH:
            if (millis() - time > 950)
            {
                ledcWrite(PWM_CHANNEL, PWM_MAX / 2);
                delay(50);
                ledcWrite(PWM_CHANNEL, PWM_MAX);
                time = millis();
            }
            break;
        case CONTINU:
            
            break;
            uint32_t pwm = ledcRead(PWM_CHANNEL);
            global.current_PWM = pwm;
            if (global.button_left == SINGLE_CLICK)
            {
                if (pwm < PWM_MAX)
                {
                    ledcWrite(PWM_CHANNEL, pwm + PWM_PAS);
                }
                else
                {
                    ledcWrite(PWM_CHANNEL, PWM_MAX);
                }
                global.button_left = 0;
            }
            if (global.button_right == SINGLE_CLICK)
            {

                if (pwm > (PWM_PAS))
                {
                    ledcWrite(PWM_CHANNEL, pwm - PWM_PAS);
                }
                else
                {
                    ledcWrite(PWM_CHANNEL, 0);
                }
                global.button_right = 0;
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
 //xTaskCreate(_pwmTask, "PWM_TASK", 4080, NULL, 1, NULL);
}
#endif