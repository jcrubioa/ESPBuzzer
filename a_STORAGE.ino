#include <EEPROM.h>

struct dataLayout
{
  char ssid[64];
  char password[64];
  char gotifyHost[64];
  char gotifyToken[64];
  char sleepTime[6];
  char wakeupTime[6];
  uint16_t debugMode;
  uint16_t sleeping;
  uint16_t wakeUntilSleepSeconds;
  uint16_t wifiTimeoutSeconds;
} persistentData;

struct additionalData
{
  uint16_t sleeping;
  uint16_t wifiTries;
} additional;

struct timeData
{
  int currentHour;
  int currentMinute;
  int sleepHour;
  int sleepMinute;
  int wakeupHour;
  int wakeupMinute;
};

void setDefaults(struct dataLayout* record){
  if (String(record->sleepTime) == "")
    strcpy(record->sleepTime, "21:30");
  if (String(record->wakeupTime) == "")
    strcpy(record->wakeupTime, "6:00");
  if (record->wakeUntilSleepSeconds == 0 || record->wakeUntilSleepSeconds == 65535)
    record->wakeUntilSleepSeconds = 300;
  if (record->wifiTimeoutSeconds == 0 || record->wifiTimeoutSeconds == 65535)
    record->wifiTimeoutSeconds = 10;
  if (record->debugMode == 0 || record->debugMode == 65535)
    record->debugMode = 1;
}

void setSleeping(){ 
  EEPROM.put(sizeof(persistentData) + 1, additional);
  EEPROM.commit();
}

uint16_t getSleeping(){
  EEPROM.get(sizeof(persistentData) + 1, additional);
  if (additional.wifiTries == 65535)
    additional.wifiTries == 0;
}
