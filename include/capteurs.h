#ifndef CAPTEUR_H
#define CAPTEUR_H
#include <Arduino.h>
#include "global.h"

#define ANALOG_INTENSITY 2
#define ANALOG_VOLTAGE 34
#define ANALOG_MAX 4096
#define MAX_BATTERIE_VOLTAGE 4200
#define MIN_BATTERIE_VOLTAGE 500

#define batterieValuePercent() map(analogReadMilliVolts(ANALOG_VOLTAGE), MIN_BATTERIE_VOLTAGE, MAX_BATTERIE_VOLTAGE, 0, 100)

typedef enum
{
    CRITICAL_STATE,
    LOW_STATE,
    GOOD_STATE
} BateryState_t;

BateryState_t getBatteryState()
{
    uint32_t value = analogReadMilliVolts(ANALOG_VOLTAGE);
    if (value <= MIN_BATTERIE_VOLTAGE)
        return CRITICAL_STATE;
    if (batterieValuePercent() < 25)
        return LOW_STATE;
    return GOOD_STATE;
}
TaskHandle_t capteurHandle = NULL;
// I_Ve=1.818*Vs
double filter(double in);
double filterBat(double in);
void _taskRegulation(void *args)
{
    uint8_t compteur = 0;
    unsigned long lastTime = micros();
    for (;;)
    {
        int newVSAOP = analogRead(ANALOG_INTENSITY);
        newVSAOP = (newVSAOP / 1.24);
        newVSAOP = constrain(newVSAOP, 0, 5000);
        // filtre numérique intensité
        double intent = filter(newVSAOP);
        global.intesite = intent;

        global.voltage = filterBat((analogRead(ANALOG_VOLTAGE) / 1.24) * 2);
        global.batterie = batterieValuePercent();
        float v = global.voltage / 1000.0;
        float e = global.intesite / 1000.0;
        global.power = v * e;

        // TODO : Régulation
        if (!global.consigne)
        {
            global.current_PWM = 0;
            ledcWrite(PWM_CHANNEL, PWM_MAX);
        }
        else
        {
            int erreur = global.consigne - global.intesite;
            int pwmE = PWM_MAX - global.current_PWM;
            int pwm = pwmE + erreur / 100;
            pwm = PWM_MAX - constrain(pwm, 0, PWM_MAX);
            global.current_PWM = pwm; //*0.008+global.current_PWM*0.992;
            ledcWrite(PWM_CHANNEL, global.current_PWM);
        }
        Serial.printf("%u,%u,%u,%u\n\r", global.intesite, global.consigne, global.current_PWM, global.voltage, 0);
        // Screen repaint every 100ms
        if (compteur > 1000)
        {
            requestSceenRepaint();
            compteur = 0;
        }
        compteur++; // 1ms
    }
    vTaskDelete(capteurHandle);
}

void initCapteur()
{
    analogReadResolution(12);
    xTaskCreate(_taskRegulation, "CAPTEUR_TASK", 4096, NULL, 1, &capteurHandle);
}
// y(n)=(x(n)+x(n−1)+19y(n−1))∗0.0476
double x1_var = 0;
double y1_var = 0;
double filter(double in)
{
    double y = (in + x1_var + 19.0 * y1_var) * 0.0476;
    x1_var = in;
    y1_var = y;
    return y;
}

double x1_var_batterie = 0;
double y1_var_batterie = 0;
double filterBat(double in)
{
    double y = (in + x1_var_batterie + 19.0 * y1_var_batterie) * 0.0476;
    x1_var_batterie = in;
    y1_var_batterie = y;
    return y;
}
#endif