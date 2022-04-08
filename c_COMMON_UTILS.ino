#include <ESPmDNS.h>

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
