const int trigPin2 = 5;
const int echoPin2 = 18;

const int trigPin1 = 2;
const int echoPin1 = 4;

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
