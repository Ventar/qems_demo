#include <QEMSWiFiManager.h>

QEMSWiFiManager::QEMSWiFiManager(char const *apName, std::function<void(WiFiManager *)> configCallback, std::function<void(WiFiManager *)> connectedCallback)
    : WiFiManager() {

    _connectedCallback = connectedCallback;
    _apName = apName;

    esp_wifi_set_country_code("DE", true);

    WiFi.mode(WIFI_STA);
    const char *menu[] = {"wifi", "exit"};

    setHostname("qems");
    // setConfigPortalBlocking(false);
    //   wifiManager.setConfigPortalTimeout(300);
    //  setCaptivePortalEnable(true);
    setConnectTimeout(20);
    setSaveConnectTimeout(20);
    setDarkMode(true);
    setShowInfoUpdate(false);
    setMenu(menu, 2);
    setMinimumSignalQuality(30);
    setCleanConnect(true);
    // wifiManager.setSaveConfigCallback([]() { ESP.restart(); });
    setAPCallback(configCallback);

    connect();
};

void QEMSWiFiManager::connect() {
    bool res = autoConnect(_apName);

    if (res) {

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        _connectedCallback(this);
    } else {
        Serial.println("");
        Serial.println("CANNOT connect to WiFi");
    }
};
