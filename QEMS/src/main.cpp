#include <Arduino.h>
#include <QEMSDataManager.h>
#include <QEMSDisplay.h>
#include <QEMSTimeManager.h>
#include <QEMSUI.h>
#include <QEMSWebServer.h>
#include <QEMSWiFiManager.h>

#define FORMAT_LITTLEFS_IF_FAILED true

QEMSDataManager *dataManagerCO2;
QEMSDataManager *dataManagerCost;
QEMSTimeManager *timeManager;
QEMSWebServer *webServer;

TaskHandle_t uiTask;
TaskHandle_t loadDataTask;
TaskHandle_t webServerTask;

int lastCo2Value = 0;
int currentCo2Value = 0;
int lastCostValue = 0;
int currentCostValue = 0;

void loadDataTaskCode(void *parameter) {
    for (;;) {

        if (!dataManagerCO2 || !dataManagerCost) {
            delay(1000);
            continue;
        }

        // if no data is available, the upload screen is shown
        if (!dataManagerCO2->isFileAvailable() || !dataManagerCost->isFileAvailable()) {
            // Serial.println("No valid file available, switch to upload mode...");
            nextScreen = ui_Screen_Upload;
            delay(1000);
            continue;
        }

        // perform the reload of new data values, this is done here to avoid blocking the UI task.
        if (!dataManagerCO2->isReady() || !dataManagerCost->isReady()) {
            // Serial.println("Data manager not ready, try to reload data...");

            lv_label_set_text(ui_S1L_Info, "Lade CO2 Daten...");
            nextScreen = ui_Screen_Loading;

            dataManagerCO2->loadDataFromFile();

            lv_label_set_text(ui_S1L_Info, "Lade Kostendaten...");
            dataManagerCost->loadDataFromFile();

            nextScreen = ui_Screen_Data;
            delay(200);
        }

        delay(1000);
    }
}

/**
 * @brief code executed on the second core of the ESP32 to ensure the UI logic is separated from the file upload logic
 */
void uiTaskCode(void *parameter) {
    for (;;) {

        if (webServer && timeManager && dataManagerCO2 && dataManagerCost) { // ensure that the pointers were initialized

            // Update the time values on the display

            char time[10];
            char date[11];

            timeManager->getTime(time);
            timeManager->getDate(date);

            lv_label_set_text(ui_S2L_Time, time);
            lv_label_set_text(ui_S2L_Date, date);

            // The UI is already used during startup. To avoid access to uninitialized classes we need to check them here beforee updating anything
            if (dataManagerCO2->isReady() && dataManagerCost->isReady()) {

                lastCo2Value = currentCo2Value;
                currentCo2Value = dataManagerCO2->getActiveValue();

                if (lastCo2Value != currentCo2Value) {
                    ui_animate_meter_value(co2Indicator, lastCo2Value, currentCo2Value);
                    lv_label_set_text(ui_S2L_CO2_Save, (String(currentCo2Value) + String("%")).c_str());
                    Serial.printf("Change CO2 Meter from [%d] to [%d]\n", lastCo2Value, currentCo2Value);
                }

                lastCostValue = currentCostValue;
                currentCostValue = dataManagerCost->getActiveValue();

                if (lastCostValue != currentCostValue) {
                    ui_animate_meter_value(costIndicator, lastCostValue, currentCostValue);
                    lv_label_set_text(ui_S2L_Cost_Save, (String(currentCostValue) + String("%")).c_str());
                    Serial.printf("Change Cost Meter from [%d] to [%d]\n", lastCostValue, currentCostValue);
                }

                // lv_meter_set_indicator_end_value(meter, co2Indicator, dataManager->getActiveValue());
            }
        }

        if (nextScreen && lv_scr_act() != nextScreen) {
            lv_scr_load_anim(nextScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        }

        lv_timer_handler(); /* let the GUI do its work */
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void webServerTaskCode(void *parameter) {
    for (;;) {
        webServer->handleClient();
        delay(100);
    }
}
void setup() {
    Serial.begin(115200);

    while (!Serial)
        delay(10);

    Serial.println("\n\nQEMS Demo 1.0.0\n");

    Serial.print("Setup: Executing on core ");
    Serial.println(xPortGetCoreID());

    // File System Setup
    // ----------------------------------------------------------------------------------------------------------------

    if (!LittleFS.begin(false)) {
        Serial.println("LITTLEFS mounting was not possible");
        Serial.println("No filesystem found, format file system...");
        if (!LittleFS.begin(true)) { // performs a format
            Serial.println("LITTLEFS mounting was not possible...");
            Serial.println("Formatting failed...");
            return;
        } else {
            Serial.println("File system was formatted...");
        }
    } else {
        Serial.printf("File system mounted %d / %d ...\n", LittleFS.usedBytes(), LittleFS.totalBytes());
    }

    // Display and UI setup
    // ----------------------------------------------------------------------------------------------------------------

    ui_disp_init();
    ui_qems_init();
    xTaskCreatePinnedToCore(uiTaskCode, "UItask", 10000, NULL, 2, NULL, tskNO_AFFINITY);

    // WiFi Setup
    // ----------------------------------------------------------------------------------------------------------------

    lv_label_set_text(ui_S1L_Info, "Verbindung zum WiFI Netzwerk\nwird hergestellt...");
    wifiManager = new QEMSWiFiManager(
        "QEMS Demo", [](WiFiManager *wm) { nextScreen = ui_Screen_WiFi; }, [](WiFiManager *wm) {});
    lv_label_set_text(ui_S3L_IP_Data, WiFi.localIP().toString().c_str());
    lv_label_set_text(ui_S5L_IP, WiFi.localIP().toString().c_str());
    lv_label_set_text(ui_S3L_WiFi_Data, WiFi.SSID().c_str());
    nextScreen = ui_Screen_Loading;

    // Service Setup
    // ----------------------------------------------------------------------------------------------------------------

    timeManager = new QEMSTimeManager();
    dataManagerCO2 = new QEMSDataManager(timeManager, "/co2.csv");
    dataManagerCost = new QEMSDataManager(timeManager, "/costs.csv");
    webServer = new QEMSWebServer(dataManagerCost, dataManagerCO2);

    xTaskCreatePinnedToCore(loadDataTaskCode, "dataTask", 10000, NULL, 1, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(webServerTaskCode, "webServerTask", 10000, NULL, 3, NULL, tskNO_AFFINITY);
}

void loop() {
    // nothing to do here, relies on the tasks managed by ESP.
    delay(60000);
}

