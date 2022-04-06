
String ip2Str(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++) {
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

bool reconnect(){
  int WIFI_RECONNECT_TIMEOUT_LOOPS = 10;
  int loopsPassed = 0;
  while (WiFi.status() != WL_CONNECTED && loopsPassed < WIFI_RECONNECT_TIMEOUT_LOOPS) {  // Wait until WiFi is connected or timeout
    Serial.println("Reconnecting to WiFi...");
    Serial.print(".");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(10000);
    loopsPassed++;
  }
  return WiFi.status() == WL_CONNECTED;
}

String SendHTML() {
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Configuracion General</h2>\n";
  if (String(persistentData.ssid) == "") {
    ptr += "<h2>Configura el ssid de la red WiFi para activar la conexion al router</h2>\n";
  } else if(String(persistentData.gotifyHost) == "" || String(persistentData.gotifyAppToken) == ""){
    ptr += "<h2>Configura las notificaciones para activar la conexion al router</h2>\n";
  }
  ptr += "<br>\n";
  ptr += "<br>\n";
  ptr += "<a href=\"/wifi\">Configurar red WiFi</a> \n";
  ptr += "<br>\n";
  ptr += "<a href=\"/gotify\">Configurar Notificaciones</a> \n";
  ptr += "<br>\n";
  ptr += "<a href=\"/timer\">Configurar Temporizador</a> \n";
  ptr += "<br>\n";
  ptr += "<a href=\"/devicePropertiesHTML\">Configurar Dispositivo</a> \n";
  ptr += "<br>\n";
  ptr += "<a href=\"/ota\">Actualizar firmware</a> \n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/clearEEPROM\">Limpiar memoria</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}

String WifiHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  if (operationMode == 1) {
    ptr += "<h2>Conectado a WiFi con credenciales : \"" + String(persistentData.ssid) + "\"<->\"" + String(persistentData.password) + "\"</h2>\n";
  } else if (operationMode == 0) {
    if(String(persistentData.ssid) != ""){
      ptr += "<h2>Credenciales WiFi configuradas: \"" + String(persistentData.ssid) + "\"<->\"" + String(persistentData.password) + "\"</h2>\n";
      ptr += "<h2>Configura las notificaciones para activar conexion al router</h2>\n";
    } else
      ptr += "<h2>Configuracion de WiFi de la casa</h2>\n";
  }

  ptr += "<form action=\"/wifi/get\"> \n";
  ptr += "SSID:<br>\n";
  ptr += "<input type=\"text\" name=\"ssid\" value=\"" + String(persistentData.ssid) + "\"> \n";
  ptr += "<br>\n";
  ptr += "Contrasena:<br>\n";
  ptr += "<input type=\"text\" name=\"password\" value=\"" + String(persistentData.password) + "\"> \n";
  ptr += "<br><br>\n";
  ptr += "<input type=\"submit\" value=\"Submit\"> \n";
  ptr += "</form> \n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/\">Regresar al inicio</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}

String GotifyHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Configuracion de Gotify</h2>\n";
  ptr += "<form action=\"/gotify/get\"> \n";
  ptr += "Host:Port de gotify:<br>\n";
  ptr += "<input type=\"text\" name=\"gotifyHost\" value=\"" + String(persistentData.gotifyHost) + "\"> \n";
  ptr += "<br>\n";
  ptr += "App Token de gotify:<br>\n";
  ptr += "<input type=\"text\" name=\"gotifyAppToken\" value=\"" + String(persistentData.gotifyAppToken) + "\"> \n";
  ptr += "<br>\n";
  ptr += "Client Token de gotify:<br>\n";
  ptr += "<input type=\"text\" name=\"gotifyClientToken\" value=\"" + String(persistentData.gotifyClientToken) + "\"> \n";
  ptr += "<br><br>\n";
  ptr += "<input type=\"submit\" value=\"Submit\"> \n";
  ptr += "</form> \n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/\">Regresar al inicio</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}

String TimerHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  if (String(persistentData.sleepTime) != "" && String(persistentData.wakeupTime) != "") {
    ptr += "<h2>Hora de dormir actual: \"" + String(persistentData.sleepTime) + "\"</h2>\n";
    ptr += "<h2>Hora de despertar actual: \"" + String(persistentData.wakeupTime) + "\"</h2>\n";
  } else {
    ptr += "<h2>No se han configurado horarios</h2>\n";
  }
  ptr += "<form action=\"/timer/get\"> \n";
  ptr += "Hora de dormir:<br>\n";
  ptr += "<input type=\"text\" name=\"sleepTimeH\" value=\"" + String(getHourFromSaved(persistentData.sleepTime)) + "\"> \n";
  ptr += " : ";
  ptr += "<input type=\"text\" name=\"sleepTimeM\" value=\"" + String(getMinuteFromSaved(persistentData.sleepTime)) + "\"> \n";
  ptr += "<br>\n";
  ptr += "Hora de despertar:<br>\n";
  ptr += "<input type=\"text\" name=\"wakeupTimeH\" value=\"" + String(getHourFromSaved(persistentData.wakeupTime)) + "\"> \n";
  ptr += " : ";
  ptr += "<input type=\"text\" name=\"wakeupTimeM\" value=\"" + String(getMinuteFromSaved(persistentData.wakeupTime)) + "\"> \n";
  ptr += "<br><br>\n";
  ptr += "<input type=\"submit\" value=\"Submit\"> \n";
  ptr += "</form> \n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/\">Regresar al inicio</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}

String devicePropertiesHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Configuracion del Dispositivo</h2>\n";
  
  ptr += "Segundos para ignorar la hora de dormir (Util para configurar el dispositivo):<br>\n";
  ptr += "<input type=\"text\" name=\"wakeUntilSleepSeconds\" value=\"" + String(persistentData.wakeUntilSleepSeconds) + "\"> \n";
  ptr += "<br>\n";

  ptr += "Segundos para esperar que se establezca la conexion WiFi:<br>\n";
  ptr += "<input type=\"text\" name=\"wifiTimeoutSeconds\" value=\"" + String(persistentData.wifiTimeoutSeconds) + "\"> \n";
  ptr += "<br>\n";

  ptr += "Debug mode:<br>\n";
  ptr += "<input type=\"text\" name=\"debugMode\" value=\"" + String(persistentData.debugMode) + "\"> \n";
  ptr += "<br>\n";

  ptr += "<input type=\"submit\" value=\"Submit\"> \n";
  ptr += "</form> \n";
  
  ptr += "<br><br>\n";
  ptr += "<a href=\"/\">Regresar al inicio</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}

String ClearEEPROMHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Borrar todas las configuraciones</h2>\n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/clearEEPROMConfirmed\">Si esta seguro presione este link</a> \n";
  ptr += "<br><br>\n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/\">Regresar al inicio</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}

String ClearEEPROMConfirmHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Cuidado. Esta a punto de borrar todas las configuraciones</h2>\n";
  ptr += "<form action=\"/clearEEPROM/get\"> \n";
  ptr += "<input type=\"submit\" value=\"Submit\"> \n";
  ptr += "</form> \n";
  ptr += "<br><br>\n";
  ptr += "<br><br>\n";
  ptr += "<a href=\"/\">Regresar al inicio</a> \n";
  ptr += "</body></html> \n";
  return ptr;
}
