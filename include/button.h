#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "global.h"
Button2 btnDroite(BUTTON_1);
Button2 btnGauche(BUTTON_2);

void _taskBUTTON(void *args)
{
    for (;;)
    {
        btnDroite.loop();
        btnGauche.loop();
        if (global.button_right == LONG_CLICK)
        {
            if (global.current_mode != CONTINU)
            {
                global.current_mode++;
            }
            else
            {
                global.current_mode = OFF;
            }
            global.button_right = 0;
            global.repaintAll = true;
                        
        }
        if (global.button_left == LONG_CLICK)
        {
            if (global.current_mode != OFF)
            {
                global.current_mode--;
            }
            else
            {
                global.current_mode = CONTINU;
            }
            global.button_left = 0;
            global.repaintAll = true;
        }
         if (global.button_left || global.button_right)
        {
            turnOnScreen();
            updateUserInterraction();
            requestSceenRepaint();
        }
        delay(1);
    }
    vTaskDelete(NULL);
}

void initButtons()
{
    btnDroite.setClickHandler([&](Button2 &btn)
                              { global.button_right = SINGLE_CLICK; });
    btnGauche.setClickHandler([&](Button2 &btn)
                              { global.button_left = SINGLE_CLICK; });
    btnDroite.setLongClickHandler([&](Button2 &btn)
                                  { global.button_right = LONG_CLICK; });
    btnGauche.setLongClickHandler([&](Button2 &btn)
                                  { global.button_left = LONG_CLICK; });
    xTaskCreate(_taskBUTTON, "BUTTON_TASK", 4096, NULL, 1, NULL);
}
#endif