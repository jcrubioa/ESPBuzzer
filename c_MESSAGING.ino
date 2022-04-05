#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <tiny_websockets/message.hpp>
#include <tiny_websockets/client.hpp>
HTTPClient http;

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

void message_to_gotify(String message)
{
  if (sleeping == 0){
    String url = "http://" + gotifyIP + "/message?token=" + String(persistentData.gotifyToken);
    String body = "{\"message\":\"" + message + "\",\"priority\":10, \"title\":\"Atencion\"}";
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
      gotifyIP = resolveServer(persistentData.gotifyHost);
    // Free resources
    http.end();
  } else {
    Serial.println("Notification silenced due to sleep hours: " + message);
  }
}

void notify(String message){
  if (MESSAGING_SYSTEM == 0)
    message_to_gotify(message);
}
