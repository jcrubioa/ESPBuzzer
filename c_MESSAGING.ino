#include <ArduinoJson.h>
#include <HTTPClient.h>

using namespace websockets;

HTTPClient http;

using namespace websockets;

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
    wsconnected = "true";
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
    wsconnected = "false";
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
    wsconnected = "true";
  }
}

bool setupWebsocketClient(String ntfyIP) {
  String websockets_server_host = "ws://" + ntfyIP + "/" + persistentData.ntfyAlertTopic + "/ws";
  String origin = "http://" + ntfyIP;
  const uint16_t websockets_server_port = 80; // Enter server port

  wsclient.onEvent(onEventsCallback);

  wsclient.onMessage([&](WebsocketsMessage message) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message.data().c_str());
    const char* value = doc["title"];
    Serial.print("Got Message: ");
    Serial.println(message.data());
    Serial.print("message value: ");
    Serial.println(value);
    if (String(value) == "Ha llegado un cliente") {
      playSound();
    }
  });

  wsclient.addHeader("Origin", origin);
  bool connected = wsclient.connect(websockets_server_host);

  return connected;
}

void messageToTelegram(String message)
{
  //adding all number, your api key, your message into one complete url
  //bot->sendMessage(String(persistentData.telegramChatID), message, "");
}

void messageToNtfy(String ntfyIp, String title, String message, String onClick, String topic, String tags, int priority)
{
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
}

void notify(int messagingSystem, String ntfyIP, String title, String message, String onClick, String topic, String tags, int priority) {
  if (sleeping == 0) {
    if (messagingSystem == 0)
      messageToNtfy(ntfyIP, title, message, onClick, topic, tags, priority);
    else if (messagingSystem == 1)
      messageToTelegram(message);
  } else {
    Serial.println("Notification silenced due to sleep hours: " + message);
  }
}
