#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>
#include "Button/Button2.h"
typedef enum{OFF = 0,FLASH,CONTINU} Mode_LED;

static struct global_struct
{
    uint8_t batterie = 0;
    uint16_t intesite = 0;
    uint16_t consigne = 0;
    uint16_t voltage = 0;
    uint8_t current_PWM_percent = 0;
    uint16_t current_PWM = 0;
    float power = 0;
    bool needScreenRepaint = true,repaintAll = false;
    bool screenOn = true;
    uint32_t lastUserActivity = 0;
    uint8_t current_mode;
    uint8_t button_right = 0,button_left = 0;
} global;

#define requestSceenRepaint() global.needScreenRepaint = true

const char* bool2String(bool a)
{
    return a ? "True" : "False";
}
void printGlobalState(Print *stream)
{
    if (stream->availableForWrite())
    {
        stream->printf("{\n\r\tBaterie: %u%%,\n\r\tIntensite:%umA,\n\r\tVoltage:%umV,\n\r\tneedScreenRepaint:%s,\n\r\tscreenOn:%s\n\r}\n\r",
         global.batterie,
         global.intesite,
         global.voltage,
         bool2String(global.needScreenRepaint),
         bool2String(global.screenOn)
        );
    }
}

//------------PWM
// all declarations for pwm 
//----------------

#define BUTTON_1 35
#define BUTTON_2 0

#define PWM_PIN 32

#define PWM_CHANNEL 1
#define PWM_FREQ 64000
#define PWM_RESOLUTION 10
#define PWM_MAX 1023

#define PWM_PAS 20
//Calculs de PWM
#define current_pwm ledcRead(PWM_CHANNEL)
#define pwmInPercent map(current_pwm,0,PWM_MAX,0,100)

//------------------TFT 
//
//------------------------
#define turnOnScreen() global.screenOn = true
#define turnOffScreen() global.screenOn = false

//user interactions
#define updateUserInterraction() global.lastUserActivity = millis();
#endif