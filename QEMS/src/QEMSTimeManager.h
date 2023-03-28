#ifndef QEMS_TIME_MANAGER_H_
#define QEMS_TIME_MANAGER_H_

#include <Arduino.h>
#include <time.h>

/**
 * @brief utility class to handle time related stuff based on an ntp server. The class MUST be instantiated after the connection to a WiFi network was
 * established, otherwise the time cannot be retrieved
 */
class QEMSTimeManager {

  public:
    QEMSTimeManager() { configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); }

    /**
     * @brief returns the current epoch time
     */
    time_t now() {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
        }
        return mktime(&timeinfo);
    }

    /**
     * @brief stores the current time as character array in format HH:mm
     * @param timeBuf the buffer to store the time
     */
    void getTime(char *timeBuf) {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
        }
        strftime(timeBuf, 10, "%H:%M:%S", &timeinfo);
    }

    /**
     * @brief stores the current time as character array in format dd.MM.YYYY
     * @param dateBuf the buffer to store the date
     */
    void getDate(char *dateBuf) {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
        }
        strftime(dateBuf, 11, "%d.%m.%Y", &timeinfo);
    }

  private:
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 3600;
    const int daylightOffset_sec = 3600;
};

#endif
