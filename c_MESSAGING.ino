#include <ArduinoJson.h>
#include <HTTPClient.h>

using namespace websockets;

HTTPClient http;

using namespace websockets;
void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
        wsconnected = "true";
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
        wsconnected = "false";
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
        wsconnected = "true";
    }
}

bool setupWebsocketClient(){
  ntfyIP = resolveServer(persistentData.ntfyHost);
  if (ntfyIP == "")
    esp_restart();
  
  String websockets_server_host = "ws://" + ntfyIP + "/" + persistentData.ntfyAlertTopic + "/ws";
  String origin = "http://" + ntfyIP;
  const uint16_t websockets_server_port = 80; // Enter server port

  wsclient.onEvent(onEventsCallback);

  wsclient.onMessage([&](WebsocketsMessage message){
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message.data().c_str());
    const char* value = doc["title"];
    Serial.print("Got Message: ");
    Serial.println(message.data());
    Serial.print("message value: ");
    Serial.println(value);
    if (String(value) == "Ha llegado un cliente"){
      playSound();
    }
  });

  wsclient.addHeader("Origin", origin);
  bool connected = wsclient.connect(websockets_server_host);

  return connected;
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

String resolveServer(String hostname){
  for(int i =0; i < 10; i++){
    IPAddress answer = MDNS.queryHost(hostname);
    if(answer.toString() == "0.0.0.0") {
      delay(250);
      continue;
    } else {
      return ip2Str(answer);
    }
  }
  Serial.println("Failed fetching mdns");
  return "";
}

void messageToNtfy(String title, String message, String onClick, String topic, String tags, int priority)
{
  if (sleeping == 0){
    String url = "http://" + ntfyIP + "/";
    String body = "{";
      body += "\"title\":\"" + title + "\",";
      body += "\"message\":\"" + message + "\",";
      body += "\"topic\":\"" + topic + "\",";
      body += "\"tags\":" + tags + ",";
      body += "\"priority\":" + String(priority);
    if (onClick != "")
      body += ",\"click\":\"" + onClick + "\"";
    body += "}";
    http.begin(url);  
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(body);
    String response = http.getString();
    
    Serial.print("HTTP URL: ");
    Serial.println(url);

    Serial.print("HTTP Body: ");
    Serial.println(body);
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    Serial.print("HTTP Response: ");
    Serial.println(response);
    if (httpResponseCode < 0)
      ntfyIP = resolveServer(persistentData.ntfyHost);
    // Free resources
    http.end();
  } else {
    Serial.println("Notification silenced due to sleep hours: " + message);
  }
}

void notify(String title, String message, String onClick, String topic, String tags, int priority){
  if (MESSAGING_SYSTEM == 0)
    messageToNtfy(title, message, onClick, topic, tags, priority);
}

void notifyDeviceReady(){
  notify("Dispositivo de alarma remota activado", 
    "Usa esta direccion para configurar el dispositivo: http://" + ip2Str(WiFi.localIP()), 
    "http://" + ip2Str(WiFi.localIP()), 
    "config",
    "[\"battery\", \"gear\"]", 
    3);
}
