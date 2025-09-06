/*
  All Time related methods are here
*/

//Day of the week
const char* weekday_D[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
//Month
const char* month_M[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

// for debugging
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo, 15000)) {  // Wait for 5-sec for time to synchronise
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
}

void setTimezone(String timezone){
  Serial.printf("Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1); // Now adjust the TZ. Clock settings are adjusted to show the new local time
  tzset();
}

// for debug time settings, e.g. 1 minute before dst
void setTimeManual(int yr, int month, int mday, int hr, int minute, int sec, int isDst){
  struct tm tm;

  tm.tm_year = yr - 1900;   // Set date
  tm.tm_mon = month-1;
  tm.tm_mday = mday;
  tm.tm_hour = hr;      // Set time
  tm.tm_min = minute;
  tm.tm_sec = sec;
  tm.tm_isdst = isDst;  // 1 or 0
  time_t t = mktime(&tm);
  Serial.printf("Setting time: %s", asctime(&tm));
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);
}

bool InitTime(String timezone){
  struct tm timeinfo;
  char time_output[30], day_output[30], update_time[30];
  Serial.println("Setting up time");
  configTime(0, 0, NTP_SERVER, NTP_SERVER_2); // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo, 30000)) {  // Wait for 15-sec for time to synchronise)){
    Serial.println("Failed to obtain time");
    return false;
  }
  Serial.println("Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);

  // now get the corrected data
  if(!getLocalTime(&timeinfo, 15000)) {  // Wait for 15-sec for time to synchronise)){
    Serial.println("Failed to obtain time");
    return false;
  }

  currentHour = timeinfo.tm_hour;
  currentMin = timeinfo.tm_min;
  currentSec = timeinfo.tm_sec;
  
      sprintf(day_output, "%s %02u %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
  
    strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo);
    sprintf(time_output, "%s", update_time);
  
  dateString = day_output;
  timeString = time_output;
  return true;
}

bool getCurrentDateTimeString() {
  struct tm timeinfo;
  char time_output[30], day_output[30], update_time[30];
// now get the corrected data
  if(!getLocalTime(&timeinfo, 15000)) {  // Wait for 15-sec for time to synchronise)){
    Serial.println("Failed to obtain time");
    return false;
  }

  currentHour = timeinfo.tm_hour;
  currentMin = timeinfo.tm_min;
  currentSec = timeinfo.tm_sec;
      sprintf(day_output, "%s %02u %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
    strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo);
    sprintf(time_output, "%s", update_time);
  dateString = day_output;
  timeString = time_output;
  return true;
}
