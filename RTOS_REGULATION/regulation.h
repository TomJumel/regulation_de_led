#ifndef CAPTEUR_H
#define CAPTEUR_H
#include <Arduino.h>

#define BROCHE_INTENSITY 2
#define PWM_MAX 1023
#define PWM_CHANNEL 0



TaskHandle_t capteurHandle = NULL;

uint16_t regulation(uint16_t consigne, uint16_t intensite);
double filtre(double in);

uint16_t intensite,consigne;

void _taskRegulation(void *args)
{
    uint8_t compteur = 0;
    unsigned long lastTime = micros();
    for (;;)
    {
        int newVSAOP = analogRead(BROCHE_INTENSITY);
        newVSAOP = (newVSAOP / 1.24);
        newVSAOP = constrain(newVSAOP, 0, 5000);
        // filtre numérique intensité
        double intent = filtre(newVSAOP);
        intensite = intent;

        // TODO : Régulation
        
        uint16_t pwm_global = regulation(400,intensite);

        ledcWrite(PWM_CHANNEL, pwm_global);
    }
    vTaskDelete(capteurHandle);
}

void initRegulation()
{
    analogReadResolution(12);
    xTaskCreate(_taskRegulation, "REGU_TASK", 4096, NULL, 1, &capteurHandle);
}



// y(n)=(x(n)+x(n−1)+19y(n−1))∗0.0476
double x1_var = 0;
double y1_var = 0;
double filtre(double in)
{
    double y = (in + x1_var + 19.0 * y1_var) * 0.0476;
    x1_var = in;
    y1_var = y;
    return y;
}


uint16_t regulation(uint16_t consigne, uint16_t intensite)//en mA
{
    int erreur = consigne - intensite;
    int pwmE = PWM_MAX - ledcRead(PWM_CHANNEL);
    int pwm = pwmE + (erreur / 100);
   return PWM_MAX - constrain(pwm, 0, PWM_MAX);
}
#endif