#ifndef TFT_H
#define TFT_H
#include <TFT_eSPI.h>
#include "global.h"
#include "capteurs.h"
#include <esp_task_wdt.h>
TaskHandle_t tft_task = NULL;

TFT_eSPI tft = TFT_eSPI(135, 240);
void _tftTask(void *arg)
{
    // Task Setup
    TickType_t xLastPrintTime;
    xLastPrintTime = xTaskGetTickCount();
    const TickType_t delay = pdMS_TO_TICKS(100);
    static bool oldScreenState = !global.screenOn;
    // Task Loop
    for (;;)
    {
        // Si il y a un changement sur l'état de l'écran on l'applique sur le hardware
        if(global.repaintAll){
            tft.fillScreen(TFT_BLACK);
            global.repaintAll = false;
            requestSceenRepaint();
        }
        if (global.screenOn != oldScreenState)
        {
            //Serial.println("Turning screen " + String(global.screenOn ? "On" : "Off"));
            tft.fillScreen(TFT_BLACK);
            digitalWrite(TFT_BL, global.screenOn);
            oldScreenState = global.screenOn;
            if (oldScreenState)
            { // si on rallume l'écran on demande un repaint
                requestSceenRepaint();
            }
        }
        // Si demande de repaint et ecran allumé =>
        if (global.needScreenRepaint && global.screenOn)
        {
            const uint8_t bat = constrain(map(global.batterie, 0, 100, 0, 23), 0, 23);
            // batterie
            // bat color
            BateryState_t state = getBatteryState();
            tft.drawRoundRect(0, 0, 25, 10, 4, TFT_WHITE);
            tft.fillRoundRect(1, 1, 23, 8, 4, TFT_BLACK);
            if (state != CRITICAL_STATE)
            {
                uint32_t color = TFT_GREEN;
                if (state == LOW_STATE)
                    color = TFT_RED;
                tft.fillRoundRect(1, 1, bat, 8, 3, color);
            }
            else
            {
                tft.drawLine(22, 7, 2, 2, TFT_RED);
            }
            // Power PWM
            switch (global.current_mode)
            {
            case OFF:
                tft.drawString("      ", (tft.width() / 2) - (tft.textWidth("      ") / 2), 220);
                tft.drawString("ETEINT", (tft.width() / 2) - tft.textWidth("ETEINT") / 2, tft.height()/2);
                break;
            case FLASH:
                tft.drawString("           ", (tft.width() / 2) - (tft.textWidth("         ") / 2), 220);
                tft.drawString("FLASH MODE", (tft.width() / 2) - tft.textWidth("FLASH MODE") / 2, tft.height()/2);
                break;
            case CONTINU:
                const uint8_t x_pos = 20;
                tft.drawRect(42, x_pos, 50, 194, TFT_WHITE);
                uint8_t size = map(global.current_PWM_percent, 0, 100, 0, 189);
                tft.fillRect(43, x_pos + 1, 48, 189 - size, TFT_BLACK);
                if (size > 189)
                    size = 189;
                tft.fillRect(45, x_pos + 3 + 189 - size, 44, size, TFT_WHITE);
                tft.drawString("     ", (tft.width() / 2) - (tft.textWidth("     ") / 2), 220);
                String s = (String(global.current_PWM_percent) + "%");
                tft.drawString(s.c_str(), (tft.width() / 2) - tft.textWidth(s) / 2, 220);
                break;
            }
            // Puissance cons
            tft.drawString("       ", (tft.width() / 2) - (tft.textWidth("       ") / 2), 230);
            String power = (String(global.intesite) + "mA");
            tft.drawString(power.c_str(), (tft.width() / 2) - tft.textWidth(power) / 2, 230);
            // Buttons
            tft.drawRoundRect(-2, 231, 20, 11, 3, TFT_WHITE);
            tft.drawChar('+', 5, 234);

            tft.drawRoundRect(117, 231, 20, 11, 3, TFT_WHITE);
            tft.drawChar('-', 125, 234);
            // end
            global.needScreenRepaint = false;
        }
        if (millis() - global.lastUserActivity > 10000 && global.screenOn)
        {
            turnOffScreen();
        }
        vTaskDelayUntil(&xLastPrintTime, delay);
    }
    vTaskDelete(NULL);
}
void initTft()
{
    tft.init();
    tft.fillScreen(TFT_BLACK);
    requestSceenRepaint();
    esp_task_wdt_init(10, true);
    xTaskCreate(_tftTask, "TFT_TASK", 2048, NULL, 2, &tft_task);
    esp_task_wdt_delete(tft_task);
}
#endif