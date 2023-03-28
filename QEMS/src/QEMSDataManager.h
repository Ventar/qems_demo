#ifndef QEMS_DATA_MANAGER_H_
#define QEMS_DATA_MANAGER_H_

#include <LittleFS.h>
#include <QEMSTimeManager.h>
#include <time.h>

#define RECORD_CNT 120

/**
 * @brief utility class to load data records from uploaded csv files to provide the data to the display.
 */
class QEMSDataManager {

    /**
     * Data record from the file.
     */
    struct Record {
        time_t time; // epoch timestamp
        int value;   // value from 0 .. 100
    };

  public:
    QEMSDataManager(QEMSTimeManager *timeManager, String dataFile) : _timeManager(timeManager), _dataFile(dataFile) {

        File f = LittleFS.open(_dataFile);
        if (f) {
            _fileAvailable = true;
            f.close();
        } else {

        }
    }

    int getActiveValue() {

        time_t now = _timeManager->now();

        for (uint8_t i = 0; i < RECORD_CNT; i++) {
            if (_dataPoints[i].time > now && i > 0) {
                return _dataPoints[i].value;
            }
        }

        Serial.printf("No data available for [%s]\n", _dataFile);
        _ready = false;

        return 0;
    }

    bool isFileAvailable() { return _fileAvailable; }

    /**
     * @brief loads data records from the uploaded CSV file.
     */
    void loadDataFromFile() {

        // If we already load data, we do not need to do anything here.
        if (_loadInProgress) {
            return;
        }

        _ready = false;
        _loadInProgress = true;

        // open the file with the data to create records from.
        File dataFile = LittleFS.open(_dataFile);
        // take the current time.
        time_t now = _timeManager->now();

        if (!dataFile) {
            Serial.println("Could not open data file, skip processing...");
            _fileAvailable = false;
            _loadInProgress = false;
            return;
        } else {
            Serial.printf("Opened data file [%s], process data...\n", _dataFile);
            _fileAvailable = true;
        }

        // we store exactly x records which are not in the past.

        String r;
        uint8_t recordPointer = 0; // pointer to the record that is currently stored
        while (dataFile.available()) {

            r = dataFile.readStringUntil('\n'); // read the next line

            String data = r.substring(20, 25);
            data.replace(',', '.');
            int value = (int)(data.toFloat() * 100); // convert the String value into an integer value

            struct tm ts = {0};
            strptime(r.substring(0, 19).c_str(), "%d.%m.%Y %H:%M:%S", &ts);
            time_t epoch_ts = mktime(&ts); // convert the timestamp from the String into an epoch timestamp for easier handling

            // record is in the future, store it ffu
            if (epoch_ts > now && recordPointer < RECORD_CNT) {
                _dataPoints[recordPointer] = {epoch_ts, value};
                recordPointer++;
            }
            
        }

        dataFile.close();
        Serial.printf("Updated data records for [%s], loaded records = %d\n", _dataFile, recordPointer);

        if (recordPointer >= RECORD_CNT) { // check if we have enough dat aloaded
            _fileAvailable = true;
            _ready = true;
        } else {
            _fileAvailable = false;
            _ready = false;
        }

        _loadInProgress = false;
    }

    bool isReady() { return _ready && _fileAvailable; }

    String getFileName() { return _dataFile; }

  private:
    /**
     * If the data manager is ready to provide data.
     */
    bool _ready = false;

    /**
     * Flag that indicates that the data is currently loaded, needed to avoid multiple imports at once when the method is called from different CPU cores
     */
    bool _loadInProgress = false;

    /**
     * If the file was found in the file system.
     */
    bool _fileAvailable = false;

    /**
     * The file to load the data from
     */
    String _dataFile;

    /**
     * @brief provides access to the current time
     */
    QEMSTimeManager *_timeManager;

    /**
     * @brief contains the first 100 data points in the future starting from the last parsing action.
     */
    Record _dataPoints[RECORD_CNT] = {0, 0};
};

#endif
