void notifyCustomerArrival(){
  notify(0, ntfyIP, "Ha llegado un cliente", 
    "No lo hagas esperar", 
    "", 
    "alerts", 
    "[\"loudspeaker\", \"walking_man\"]", 
    3);
}

void notifyCustomerArrivalDebug(String optDistance){
  notify(0, ntfyIP, "Ha llegado un cliente " + optDistance, 
    "No lo hagas esperar", 
    "", 
    "alerts", 
    "[\"loudspeaker\", \"walking_man\"]", 
    3);
}

void notifyDeviceReady(String localIpAddress){
  notify(0, ntfyIP, "Dispositivo de deteccion de clientes activado", 
    "Usa esta direccion para configurar el dispositivo: http://" + localIpAddress, 
    "http://" + localIpAddress, 
    "config",
    "[\"battery\", \"gear\"]", 
    3);
}

void playSound(){
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
}
