int lastHourFetch;
int lastMinuteFetch;

int getHour(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return 12;
  }
  lastHourFetch = timeinfo.tm_hour;
  return lastHourFetch;
}

int getMinute(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return 0;
  }
  lastMinuteFetch = timeinfo.tm_min;
  return lastMinuteFetch;
}

int getHourFromSaved(char* targetTime){
  char result[3];
  char *aux = strdup(targetTime);
  char *token = strtok(aux, ":");
  result[0] = token[0];
  result[1] = token[1];
  result[2] = 0;
  return atoi(result);
}
int getMinuteFromSaved(char* targetTime){
  char result[3];
  const char delimiters[] = ":";
  char *aux = strdup(targetTime);
  char *token = strsep(&aux, delimiters);
  token = strtok(NULL, ":");
  result[0] = token[0];
  result[1] = token[1];
  result[2] = 0;
  return atoi(result);
}
