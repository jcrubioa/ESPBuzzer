int getHour(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return 12;
  }
  return timeinfo.tm_hour;
}

int getMinute(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return 0;
  }
  return timeinfo.tm_min;
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

float min_of_two(int x, int y) {
   return (x < y) ? x : y;
}

float min_of_three(int m, int n, int p) {
   return min_of_two(min_of_two(m, n), p);
}

float max_of_two(int x, int y) {
   return (x > y) ? x : y;
}

float max_of_three(int m, int n, int p) {
   return max_of_two(max_of_two(m, n), p);
}

void fetchTimes(struct timeData* times){
  times->currentHour = getHour();
  times->currentMinute = getMinute();
  times->sleepHour = getHourFromSaved(persistentData.sleepTime);
  times->sleepMinute = getMinuteFromSaved(persistentData.sleepTime);
  times->wakeupHour = getHourFromSaved(persistentData.wakeupTime);
  times->wakeupMinute = getMinuteFromSaved(persistentData.wakeupTime);
}

// This uses the three possible combinations that are sleeping(CSW, SCW, WSC)
// C=current, W=wakeup, S=sleep
bool calculateTimeToSleep(timeData times){
  int C = (times.currentHour * 60 + times.currentMinute) % 1440;
  int S = (times.sleepHour * 60 + times.sleepMinute) % 1440;
  int W = (times.wakeupHour * 60 + times.wakeupMinute) % 1440;

  int min = min_of_three(C, S, W);
  int max = max_of_three(C, S, W);

  bool result;
  if(C == W)
    return 0;
  else if (C == S)
    return 1;
  else
    return (min ==  C && max == S) ||
           (min == S && max == W) ||
           (min == W && max == C);
}

int calculateMinutesToSleep(timeData times){
  int hour = getHour();
  int minute = getMinute();
  int hourToWakeup = getHourFromSaved(persistentData.wakeupTime);
  int minuteToWakeup = getMinuteFromSaved(persistentData.wakeupTime);

  int minuteOfTheDayCurrent = (times.currentHour * 60 + times.currentMinute) % 1440;
  int minuteOfTheDayWakeUp = (times.wakeupHour * 60 + times.wakeupMinute) % 1440;
  
  return (1440 - (minuteOfTheDayCurrent - minuteOfTheDayWakeUp)) % 1440;
}
