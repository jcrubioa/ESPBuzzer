#include "EEPROM.h"
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Update.h>
#include <ESPmDNS.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;

String gotifyIP;

WiFiUDP udp;
WiFiClientSecure client;

UniversalTelegramBot *bot;

unsigned long lastClientGotInTs;
unsigned long lastClientGotOutTs;

int operationMode = 0;
int sleeping = 0;

int in = 0;

unsigned long start;
WebServer server(80);

void setup() {
  start = millis();
  lastClientGotInTs = 0;
  lastClientGotOutTs = 0;
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200);

  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to init EEPROM");
    delay(10000);
  }

  getSleeping();
  sleeping = additional.sleeping;
  Serial.print("Sleeping fetch: ");
  Serial.println(sleeping);

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    esp_restart();
  } else if (wakeup_reason == NULL){
    additional.sleeping = 0;
    setSleeping();
  }

  Serial.print("Sleeping saved: ");
  Serial.println(additional.sleeping);
  
  EEPROM.get(0, persistentData);
  setDefaults(&persistentData);
  Serial.println("SSID read: " + String(persistentData.ssid));
  Serial.println("Password read: " + String(persistentData.password));
  Serial.println("Gotify URL read: " + String(persistentData.gotifyHost));
  Serial.println("Gotify Token read: " + String(persistentData.gotifyToken));
  Serial.println("Telegram Bot Token read: " + String(persistentData.telegramBot));
  Serial.println("Telegram Bot chat ID read: " + String(persistentData.telegramChatID));
  Serial.println("Time to sleep read: " + String(persistentData.sleepTime));
  Serial.println("Time to wakeup read: " + String(persistentData.wakeupTime));
  Serial.print("clientInCoolDownSeconds: ");
  Serial.println(persistentData.clientInCoolDownSeconds);
  Serial.print("clientOutCoolDownMs: ");
  Serial.println(persistentData.clientOutCoolDownMs);
  Serial.print("sensorRefreshRateMs: ");
  Serial.println(persistentData.sensorRefreshRateMs);
  Serial.print("wakeUntilSleepSecondsonds: ");
  Serial.println(persistentData.wakeUntilSleepSeconds);
  Serial.print("wifiTimeoutSeconds: ");
  Serial.println(persistentData.wifiTimeoutSeconds);
  Serial.print("debugMode: ");
  Serial.println(persistentData.debugMode);


  bot = new UniversalTelegramBot (persistentData.telegramBot, client);
  if (String(persistentData.ssid) != "" && String(persistentData.gotifyHost) != "" && String(persistentData.gotifyToken)) {
    int secondsPassed = 0;
    Serial.println("Wifi connecting.");
    WiFi.begin(persistentData.ssid, persistentData.password);              // Try to connect with the given SSID and PSS
    Serial.println("Connecting to WiFi");
    
    bool wifiConnected = false;
    while (WiFi.status() != WL_CONNECTED && secondsPassed < persistentData.wifiTimeoutSeconds) {  // Wait until WiFi is connected or timeout
      Serial.print(".");
      delay(1000);
      secondsPassed++;
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Wifi tries: ");
      Serial.println(additional.wifiTries);
      if (additional.wifiTries < 3){
        additional.wifiTries++;
        setSleeping();
        esp_restart();
      }
      operationMode = 0;
      Serial.println();
      Serial.println("Couldn't connect to wifi. Restart device or configure another network");
    } else {
      additional.wifiTries = 0;
      setSleeping();
      operationMode = 1;
      Serial.println();
      Serial.println("Connected to the WiFi network"); // Print wifi connect message
      Serial.println(WiFi.localIP());
      configTime(gmtOffset_sec, 0, ntpServer);
      if(!MDNS.begin("esp32")) {
        Serial.println("Error starting mDNS");
      }

      gotifyIP = resolveServer(persistentData.gotifyHost);
      if (gotifyIP == "")
        esp_restart();
      notify("Usa esta direccion para configurar el dispositivo: " + ip2Str(WiFi.localIP()));
    }
  }
  if (operationMode == 0) {
    Serial.println("Wifi not connected. Setup mode...");
    operationMode = 0;
    const char* ssid = "ESP32";  // Enter SSID here
    const char* password = "12345678";  //Enter Password here
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 0, 0);

    WiFi.disconnect();
    delay(2000);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password);
    delay(100);
  }
  server.on("/", handle_OnConnect);
  server.on("/wifi", handle_OnWifi);
  server.on("/telegram", handle_OnTelegram);
  server.on("/timer", handle_OnTimer);
  server.on("/gotify", handle_OnGotify);
  server.on("/wifi/get", handle_WifiSubmit);
  server.on("/telegram/get", handle_TelegramSubmit);
  server.on("/timer/get", handle_TimerSubmit);
  server.on("/gotify/get", handle_GotifySubmit);
  server.on("/devicePropertiesHTML", handle_OnDeviceProperties);
  server.on("/deviceProperties/get", handle_DevicePropertiesSubmit);
  server.on("/clearEEPROM", handle_ClearEEPROMHome);
  server.on("/clearEEPROMConfirmed", handle_ClearEEPROMConfirm);
  server.on("/clearEEPROM/get", handle_ClearEEPROM);
  server.on("/ota", handle_OnOTA);
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, handle_OnUpdate);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("Config HTTP server started");
}

void loop() {
  unsigned long finish = millis();
  
  if (operationMode == 1 && !(finish - start >= persistentData.wakeUntilSleepSeconds * 1000 || sleeping)) {
    if (WiFi.status() == WL_CONNECTED) {
      //int leftDistance = readDistance(trigPin2, echoPin2);
      int rightDistance = readDistance(trigPin1, echoPin1);
      int leftDistance = 55;
      if (persistentData.debugMode == 1){
        Serial.print("Left ");
        Serial.println(leftDistance);
        Serial.print("Right ");
        Serial.println(rightDistance);
      }

      if(rightDistance < 110)
        notify("Ha llegado un cliente " + String(rightDistance));
      
      unsigned long currentTime = millis();
      if (leftDistance < persistentData.lengthRangeCm && rightDistance > leftDistance && currentTime - lastClientGotInTs >= persistentData.clientInCoolDownSeconds * 1000 && currentTime - lastClientGotOutTs >= persistentData.clientOutCoolDownMs) {
        in++;
        if (in >= 2){
          Serial.println("Client got in");
            if (persistentData.debugMode == 0)
              notify("Ha llegado un cliente");
            lastClientGotInTs = millis();
            in = 0;
        }
      } else if (rightDistance < persistentData.lengthRangeCm && leftDistance >= persistentData.lengthRangeCm && currentTime - lastClientGotInTs >= persistentData.clientInCoolDownSeconds * 1000) {
        Serial.println("Client got out");
        lastClientGotOutTs = millis();
      }
      delay(persistentData.sensorRefreshRateMs);
    } else {
      reconnect();
    }
  } else if (operationMode == 1 && (finish - start >= persistentData.wakeUntilSleepSeconds * 1000 || sleeping)) {
    Serial.println("Fetching current time and checking time to sleep");
    start = millis();
    timeData times;
    fetchTimes(&times);
    bool sleep = calculateTimeToSleep(times);
    bool timersNotSet = times.sleepHour + times.sleepMinute + times.wakeupHour + times.wakeupMinute == 0;
    if (!timersNotSet && sleep) {
      int minutesToSleep = calculateMinutesToSleep(times);
      if (minutesToSleep >= 60){
        Serial.print("Desired minutes to sleep: ");
        Serial.print(minutesToSleep);
        Serial.println(". Truncated to 30 minutes");
        minutesToSleep = 30;
        additional.sleeping = 1;
        setSleeping();
      }
      Serial.print("Going to sleep for: ");
      Serial.println(minutesToSleep);
      esp_sleep_enable_timer_wakeup((minutesToSleep * 60) * uS_TO_S_FACTOR);
      esp_deep_sleep_start();
    }
  } else {
    delay(1000);
  }
  server.handleClient();
}
