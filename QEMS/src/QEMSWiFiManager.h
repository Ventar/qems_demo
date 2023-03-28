#ifndef CUSTOM_WM_H_
#define CUSTOM_WM_H_

#include <WiFiManager.h>

/**
 * Extension to the WiFiManager class with some default configuration used in multiple projects.
 *
 */
class QEMSWiFiManager : public WiFiManager {

  public:
    /**
     * Creates a new custom WiFiManager.
     * @param apName the name of the access point to start if no connection to an existing WIFI network was possible
     * @param configCallback callback triggered when the configuration modes was entered
     * @param connectedCallback callback triggered when the connection to the WIFI network was established
     *
     */
    QEMSWiFiManager(char const *apName, std::function<void(WiFiManager *)> configCallback, std::function<void(WiFiManager *)> connectedCallback);

    /**
     * Performs the connection or starts a network to configure the device
     */
    void connect();

  private:
    /**
     * The access point name to use for the portal in case no connection could be established
     */
    char const *_apName;

    /**
     * Callback triggered, when the connection was successfully established.
     */
    std::function<void(WiFiManager *)> _connectedCallback;
};



#endif