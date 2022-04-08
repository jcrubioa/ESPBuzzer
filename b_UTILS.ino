float readDistance(int trigPin, int echoPin) {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  return duration * SOUND_SPEED / 2;
}

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
