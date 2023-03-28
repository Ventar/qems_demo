#ifndef QEMS_WEB_SERVER_H_
#define QEMS_WEB_SERVER_H_

#include <LittleFS.h>
#include <QEMSDataManager.h>
#include <WebServer.h>

/**
 * Utility class to handle file related operations via web browser to provide fake data to the display.
 *
 */
class QEMSWebServer {

  public:
    /**
     * @brief Creates a new ESP32 web server and configures the methods to handle incoming HTTP request.
     *
     */
    QEMSWebServer(QEMSDataManager *costManager, QEMSDataManager *co2Manager) : _co2Manager(co2Manager), _costManager(costManager) {
        webServer = new WebServer(80);

        webServer->on("/", [this]() { webServer->send(200, "text/html", createWebPage()); });

        webServer->on("/favicon.ico", [this]() { webServer->send(404, "text/plain", ""); });

        webServer->on("/delete", [this]() {
            deleteFile(webServer->arg(0));
            webServer->sendHeader("Location", String("/"), true);
            webServer->send(302, "text/plain", "");
        });

        webServer->on("/format", [this]() {
            format();
            webServer->sendHeader("Location", String("/"), true);
            webServer->send(302, "text/plain", "");
        });

        webServer->on(
            "/upload", HTTP_POST, [this]() { webServer->sendHeader("Connection", "close"); }, [this]() { upload(); });

        webServer->onNotFound([this]() { stream(webServer->uri()); });

        webServer->begin();
        Serial.println("HTTP Server started");
    }

    /**
     * Method to handle incoming client requests; has to be called in one of the CPU loops
     */
    void handleClient() { webServer->handleClient(); }

    bool isUploadInProgress() { return uploadInProgress; };

  private:
    QEMSDataManager *_costManager;

    QEMSDataManager *_co2Manager;

    /**
     * The actual webserver.
     */
    WebServer *webServer;

    /**
     * Tries to stream the passed path as a plain text file to the client.
     */
    void stream(String path) {

        path.toLowerCase();

        File dataFile = LittleFS.open(path.c_str());

        if (!dataFile) // file was not found, do nothing
            return;

        if (webServer->streamFile(dataFile, "text/plain") != dataFile.size()) {
            Serial.println("Sent less data than expected!");
        }

        dataFile.close();
    }

    /**
     Formats the internal file system
     */
    void format() {
        Serial.print("Format FS");
        LittleFS.format();

        File initFile = LittleFS.open(String("/version.txt").c_str(), FILE_WRITE);
        initFile.write((const uint8_t *)String("1.0.0").c_str(), 5);
        initFile.close();
    }

    /**
     * Deletes a file from the file System
     */
    void deleteFile(String name) {
        Serial.print("Delete ");
        Serial.println(name);
        LittleFS.remove((String("/") + name).c_str());
    }

    bool uploadInProgress = false;
    File uploadFile;

    /**
     * Handles the upload of a new data file.
     */
    void upload() {
        HTTPUpload &upload = webServer->upload();

        if (uploadInProgress == false) {
            uploadInProgress = true;
            String name = String(upload.filename);
            name.toLowerCase();
            uploadFile = LittleFS.open((String("/") + name).c_str(), FILE_WRITE);

            if (!uploadFile) {
                Serial.println("failed to open file for writing");
                uploadInProgress = false;
                return;
            }
        }

        /*
                Serial.print("Try to store: ");
                Serial.print(upload.currentSize);
                Serial.print(" of ");
                Serial.print(upload.totalSize);
                Serial.print(" bytes; status = ");
                Serial.print(upload.status);
                Serial.print(" -> ");
                */

        if (upload.status == UPLOAD_FILE_WRITE) {
            size_t writtenBytes = uploadFile.write(upload.buf, upload.currentSize);

            if (writtenBytes != upload.currentSize) {
                Serial.printf("%d - failed to write\n", writtenBytes);
                uploadInProgress = false;
                return;
            }

        } else if (upload.status == UPLOAD_FILE_END) {
            uploadFile.close();
            Serial.printf("End file upload, remaining usage %d / %d bytes\n", LittleFS.usedBytes(), LittleFS.totalBytes());

            webServer->sendHeader("Location", String("/"), true);
            webServer->send(302, "text/plain", "");

            if ((String("/") + String(upload.filename)) == _co2Manager->getFileName()) {
                _co2Manager->loadDataFromFile();
            }

            if ((String("/") + String(upload.filename)) == _costManager->getFileName()) {
                _costManager->loadDataFromFile();
            }

            uploadInProgress = false;
        } else {
            Serial.printf("Started file upload of [%s]...\n", String(upload.filename));
        }
    }

    /**
     * Creates the web page that is rendered when the server receives a GET to the root /. The page displays available files and allows to upload new ones.
     */
    String createWebPage() {
        File root = LittleFS.open("/");

        if (!root) {
            Serial.println("ERROR: Cannot open file system...try to perform a format !!!");
            LittleFS.format();
            return "ERROR: Cannot open file system...try to perform a format !!!";
        }

        String response = "";

        // no support for directories in this simple demo application
        while (true) {
            File entry = root.openNextFile();
            if (!entry) {
                // no more files
                break;
            }

            response += String("&nbsp&nbsp&nbsp&nbsp&nbsp*&nbsp<a href='") + String(entry.name()) + String("'>") + String(entry.name()) +
                        String("</a>&nbsp&nbsp&nbsp&nbsp");
            response += String("<a href='delete?file=") + String(entry.name()) + String("'>[delete]</a>") + String("</br>");

            entry.close();
        }

        String s = String("<b>Available files </b>:</br>") + response + String("</br></br><b>Usage:</b></br></br>&nbsp&nbsp&nbsp&nbsp") + LittleFS.usedBytes() +
                   String(" / ") + LittleFS.totalBytes() +
                   String("&nbsp&nbsp&nbsp&nbsp<a href='format'>[format]</a>&nbsp&nbsp(all data will be deleted)</br></br>") +
                   String("<b>Upload file:</b> </br>&nbsp&nbsp&nbsp&nbsp") + uploadScript;

        Serial.print("Generated Response: ");
        Serial.println(s);
        return s;
    }

    /**
     * JS script for the file upload.
     */
    String uploadScript = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
                          "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
                          "<input type='file' name='update'>"
                          "<input type='submit' value='Upload'>"
                          "</form>"
                          "<div id='prg'>progress: 0%</div>"
                          "<script>"
                          "$('form').submit(function(e){"
                          "e.preventDefault();"
                          "var form = $('#upload_form')[0];"
                          "var data = new FormData(form);"
                          " $.ajax({"
                          "url: '/upload',"
                          "type: 'POST',"
                          "data: data,"
                          "contentType: false,"
                          "processData:false,"
                          "xhr: function() {"
                          "var xhr = new window.XMLHttpRequest();"
                          "xhr.upload.addEventListener('progress', function(evt) {"
                          "if (evt.lengthComputable) {"
                          "var per = evt.loaded / evt.total;"
                          "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
                          "}"
                          "}, false);"
                          "return xhr;"
                          "},"
                          "success:function(d, s) {"
                          "console.log('success!');"
                          "location.reload();"
                          "},"
                          "error: function (a, b, c) {"
                          "}"
                          "});"
                          "});"
                          "</script>";
};

#endif