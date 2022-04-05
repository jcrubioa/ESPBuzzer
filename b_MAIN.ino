#include "EEPROM.h"
#include <WebServer.h>
#include <ArduinoWebsockets.h>
#include <tiny_websockets/client.hpp>
#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <Update.h>
#include <ESPmDNS.h>

using namespace websockets;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

using namespace websockets;
void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

WebsocketsClient wsclient;


#define BUZZZER_PIN  18

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;

String gotifyIP;
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
  pinMode(BUZZZER_PIN, OUTPUT); // Sets the trigPin as an Output

  digitalWrite(BUZZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZZER_PIN, LOW);
  
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
  Serial.println("Time to sleep read: " + String(persistentData.sleepTime));
  Serial.println("Time to wakeup read: " + String(persistentData.wakeupTime));
  Serial.println(persistentData.wakeUntilSleepSeconds);
  Serial.print("wifiTimeoutSeconds: ");
  Serial.println(persistentData.wifiTimeoutSeconds);
  Serial.print("debugMode: ");
  Serial.println(persistentData.debugMode);

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
      if(!MDNS.begin("espBuzzer")) {
        Serial.println("Error starting mDNS");
      }

      gotifyIP = resolveServer(persistentData.gotifyHost);
      if (gotifyIP == "")
        esp_restart();

      // run callback when messages are received
      wsclient.onMessage(onMessageCallback);
      
      // run callback when events are occuring
      wsclient.onEvent(onEventsCallback);
    
      // Connect to server
      wsclient.setInsecure();
      String wsURL = "ws://" + gotifyIP + ":80/stream?token=CZ2KYR.mSekO4HB";
      bool connected = wsclient.connect(wsURL);
      Serial.println("WS url: " + wsURL);
      if (connected) 
      {
        Serial.println("Connected!");
        String WS_msg = String("Hello to Server");
        wsclient.send(WS_msg);
      }
      else 
      {
        Serial.println("Not Connected!");
      }
      notify("Usa esta direccion para configurar el dispositivo Buzzer: " + ip2Str(WiFi.localIP()));
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
  server.on("/timer", handle_OnTimer);
  server.on("/gotify", handle_OnGotify);
  server.on("/wifi/get", handle_WifiSubmit);
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
        wsclient.poll();
      delay(500);
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
