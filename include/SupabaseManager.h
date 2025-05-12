#ifndef SUPABASE_MANAGER_H
#define SUPABASE_MANAGER_H

#include <Arduino.h>
#include "INA219Manager.h"
#include "MeteoManager.h"
#include "TimeManager.h"

class SupabaseManager {
  public:
    SupabaseManager(const String& url, const String& apiKey);
    void begin();
    bool sendData(const String& deviceId, INA219Manager& ina, MeteoManager& meteo, TimeManager& time, float dutyCycle);

  private:
    String _url;
    String _apiKey;
};

#endif
