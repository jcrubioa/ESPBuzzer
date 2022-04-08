String SendHTML() {
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Configuracion General</h2>\n";
  if (String(persistentData.ssid) == "") {
    ptr += "<h2>Configura el ssid de la red WiFi para activar la conexion al router</h2>\n";
  } else if(String(persistentData.ntfyHost) == "" || String(persistentData.ntfyConfigTopic) == ""){
    ptr += "<h2>Configura las notificaciones para activar la conexion al router</h2>\n";
  }
  ptr += "<br>\n";
  ptr += "<br>\n";
  ptr += "<a href=\"/wifi\">Configurar red WiFi</a> \n";
  ptr += "<br>\n";
  ptr += "<a href=\"/ntfy\">Configurar Notificaciones</a> \n";
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

String NtfyHTML(){
  String ptr = "<!DOCTYPE HTML><html><body>\n";
  ptr += "<h2>Configuracion de ntfy</h2>\n";
  ptr += "<form action=\"/ntfy/get\"> \n";
  ptr += "Host:Port de ntfy:<br>\n";
  ptr += "<input type=\"text\" name=\"ntfyHost\" value=\"" + String(persistentData.ntfyHost) + "\"> \n";
  ptr += "<br>\n";
  ptr += "Ntfy canal de alertas:<br>\n";
  ptr += "<input type=\"text\" name=\"ntfyAlertTopic\" value=\"" + String(persistentData.ntfyAlertTopic) + "\"> \n";
  ptr += "<br>\n";
  ptr += "Ntfy canal de configuracion:<br>\n";
  ptr += "<input type=\"text\" name=\"ntfyConfigTopic\" value=\"" + String(persistentData.ntfyConfigTopic) + "\"> \n";
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
