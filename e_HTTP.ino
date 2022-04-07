void handle_WifiSubmit() {
  String input_ssid;
  String input_password;

  input_ssid = server.arg("ssid");
  input_password = server.arg("password");

  Serial.print("SSID: ");
  Serial.println(input_ssid);
  Serial.print("Password: ");
  Serial.println(input_password);

  strcpy(persistentData.ssid, input_ssid.c_str());
  strcpy(persistentData.password, input_password.c_str());

  EEPROM.put(0, persistentData);
  EEPROM.commit();

  server.send(200, "text/html", "Wifi configurado SSID(\"" + input_ssid + "\") Contrasena(\"" + input_password + "\")<br><a href=\"/\">Regresar al inicio</a>");
  delay(500);
  esp_restart();
}

void handle_NtfySubmit() {
  String input_ntfyHost;
  String input_ntfyAlertTopic;
  String input_ntfyConfigTopic;

  input_ntfyHost = server.arg("ntfyHost");
  input_ntfyAlertTopic = server.arg("ntfyAlertTopic");
  input_ntfyConfigTopic = server.arg("ntfyConfigTopic");


  Serial.print("Ntfy Host: ");
  Serial.println(input_ntfyHost);
  Serial.print("Ntfy Alert Topic: ");
  Serial.println(input_ntfyAlertTopic);
  Serial.print("Ntfy Config Topic: ");
  Serial.println(input_ntfyConfigTopic);

  strcpy(persistentData.ntfyHost, input_ntfyHost.c_str());
  strcpy(persistentData.ntfyAlertTopic, input_ntfyAlertTopic.c_str());
  strcpy(persistentData.ntfyConfigTopic, input_ntfyConfigTopic.c_str());

  EEPROM.put(0, persistentData);
  EEPROM.commit();
  
  server.send(200, "text/html", "Ntfy configurado Host(\"" + input_ntfyHost + "\") Alert Topic (\"" + input_ntfyAlertTopic + "\") Config Topic (\"" + input_ntfyConfigTopic + "\")<br><a href=\"/\">Regresar al inicio</a>");
  delay(500);
  esp_restart();
}

void handle_TimerSubmit() {
  String input_sleepTime;
  String input_wakeupTime;

  String input_sleepTimeH;
  String input_wakeupTimeH;
  String input_sleepTimeM;
  String input_wakeupTimeM;

  input_sleepTimeH = server.arg("sleepTimeH");
  input_wakeupTimeH = server.arg("wakeupTimeH");
  input_sleepTimeM = server.arg("sleepTimeM");
  input_wakeupTimeM = server.arg("wakeupTimeM");

  input_sleepTime = input_sleepTimeH + ":" + input_sleepTimeM;
  input_wakeupTime = input_wakeupTimeH + ":" + input_wakeupTimeM;

  Serial.print("Sleep time: ");
  Serial.println(input_sleepTime);
  Serial.print("Wakeup time: ");
  Serial.println(input_wakeupTime);

  strcpy(persistentData.sleepTime, input_sleepTime.c_str());
  strcpy(persistentData.wakeupTime, input_wakeupTime.c_str());

  EEPROM.put(0, persistentData);
  EEPROM.commit();

  server.send(200, "text/html", "Horarios configurados: Hora de dormir(\"" + input_sleepTime + "\"), hora de despertar(\"" + input_wakeupTime + "\")<br><a href=\"/\">Regresar al inicio</a>");
  delay(500);
  esp_restart();
}

void handle_DevicePropertiesSubmit() {
  String input_wakeUntilSleepSeconds;
  String input_wifiTimeoutSeconds;
  String input_debugMode;

  input_wakeUntilSleepSeconds = server.arg("wakeUntilSleepSeconds");
  input_wifiTimeoutSeconds = server.arg("wifiTimeoutSeconds");
  input_debugMode = server.arg("debugMode");

  Serial.print("wakeUntilSleepSeconds: ");
  Serial.println(input_wakeUntilSleepSeconds);
  Serial.print("wifiTimeoutSeconds: ");
  Serial.println(input_wifiTimeoutSeconds);
  Serial.print("debugMode: ");
  Serial.println(input_debugMode);

  persistentData.wakeUntilSleepSeconds = atoi(input_wakeUntilSleepSeconds.c_str());
  persistentData.wifiTimeoutSeconds = atoi(input_wifiTimeoutSeconds.c_str());
  persistentData.debugMode = atoi(input_debugMode.c_str());
  

  EEPROM.put(0, persistentData);
  EEPROM.commit();

  server.send(200, "text/html", "Configuraciones de dispositivo guardadas exitosamente<br><a href=\"/\">Regresar al inicio</a>");
  delay(500);
  esp_restart();
}

void handle_OnConnect() {
  Serial.println("Client connected");
  server.send(200, "text/html", SendHTML());
}

void handle_OnWifi() {
  Serial.println("Client connected to wifi conf");
  server.send(200, "text/html", WifiHTML());
}

void handle_OnNtfy() {
  Serial.println("Client connected to Telegram conf");
  server.send(200, "text/html", NtfyHTML());
}

void handle_OnTimer() {
  Serial.println("Client connected to Timer conf");
  server.send(200, "text/html", TimerHTML());
}

void handle_OnDeviceProperties() {
  Serial.println("Client connected to Device conf");
  server.send(200, "text/html", devicePropertiesHTML());
}

void handle_ClearEEPROMHome() {
  Serial.println("Client connected to ClearEEPROM conf");
  server.send(200, "text/html", ClearEEPROMHTML());
}

void handle_ClearEEPROMConfirm() {
  Serial.println("Client connected to ClearEEPROMConfirm conf");
  server.send(200, "text/html", ClearEEPROMConfirmHTML());
}

void handle_ClearEEPROM() {
  server.send(200, "text/html", "La memoria se ha borrado correctamente<br><a href=\"/\">Regresar al inicio</a>");
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  delay(500);
  esp_restart();
}

void handle_OnOTA(){
  server.send(200, "text/html", OTAHTML());
}

void handle_OnUpdate(){
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    /* flashing firmware to ESP*/
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
