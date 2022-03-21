#ifndef STATE_SAVER_H
#define STATE_SAVER_H
#include "global.h"
#include <Preferences.h>
#define NVS_PART_SAVE "nvs_partition"
Preferences _preferences;
void initSave(){
    _preferences.begin(NVS_PART_SAVE);
}
void loadSavedData(){
    if(!_preferences.isKey("data"))return;
    global_struct *globalPtr = &global;
    _preferences.getBytes("data",globalPtr,sizeof(global));
    global.lastUserActivity = millis();
    global.current_PWM = 1024;
    global.consigne = 0;
}
void saveLoop(){
    global_struct *globalPtr = &global;
    _preferences.putBytes("data",globalPtr,sizeof(global));
}
#endif