#ifndef WSN_STORAGE_H
#define WSN_STORAGE_H

#include <Arduino.h>
#include <LittleFS.h>


namespace WSNStorage {
  static const char *FILE_PATH = "/measurements.csv";

  inline bool begin() {
    if (!LittleFS.begin()) {
      Serial.println(F("[Storage] LittleFS mount failed."));
      Serial.println(F("[Storage] Check the ESP8266 flash layout and filesystem configuration."));
      return false;
    }

    bool fileExists = LittleFS.exists(FILE_PATH);
    File file = LittleFS.open(FILE_PATH, "a");
    if (!file) {
      Serial.println(F("[Storage] Could not open measurements.csv."));
      return false;
    }

    if (!fileExists || file.size() == 0) {
      file.println(F("elapsed_seconds,node_id,temperature_c,humidity_percent,packet_counter"));
    }

    file.close();
    Serial.println(F("[Storage] LittleFS ready."));
    return true;
  }

  inline bool append(uint32_t elapsedSeconds,
                     uint32_t nodeId,
                     float temperatureC,
                     float humidityPercent,
                     uint32_t packetCounter) {
    File file = LittleFS.open(FILE_PATH, "a");
    if (!file) {
      Serial.println(F("[Storage] Could not open measurements.csv for append."));
      return false;
    }

    const size_t written = file.printf(
      "%lu,%lu,%.1f,%.1f,%lu\n",
      (unsigned long)elapsedSeconds,
      (unsigned long)nodeId,
      temperatureC,
      humidityPercent,
      (unsigned long)packetCounter
    );

    file.flush();
    file.close();

    if (written == 0) {
      Serial.println(F("[Storage] Failed to write measurement."));
      return false;
    }

    return true;
  }

  inline void printFileToSerial() {
    File file = LittleFS.open(FILE_PATH, "r");
    if (!file) {
      Serial.println(F("[Storage] Could not open measurements.csv for reading."));
      return;
    }

    Serial.println(F("----- measurements.csv -----"));
    while (file.available()) {
      Serial.write(file.read());
    }
    Serial.println(F("\n----- end of file -----"));
    file.close();
  }

  inline bool removeFile() {
    return LittleFS.remove(FILE_PATH);
  }
}

#endif // WSN_STORAGE_H
