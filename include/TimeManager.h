#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <WiFi.h>
#include <time.h>

class TimeManager {
public:
  TimeManager(const char* ntpServer = "pool.ntp.org", long gmtOffset_sec = -10800, int daylightOffset_sec = 0);
  void begin();
  String getFormattedTime();

private:
  const char* _ntpServer;
  long _gmtOffset_sec;
  int _daylightOffset_sec;
};

#endif