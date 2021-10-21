// Allow OTA software updates
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

// WiFi debugging
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wifi_internal.h>

// We've  declared the project ID in the main sketch
extern String projectTitle;
extern String projectDesc;

// We've declared the WiFi credentials in the main sketch
extern const char *SSID;
extern const char *PASSWORD;

// This is the standard web server for the ESP32 (on port 80)
AsyncWebServer server(80);

// Convert the WiFi (error) response to a string we can understand
const char *wl_status_to_string(wl_status_t status)
{
    switch (status)
    {
    case WL_NO_SHIELD:
        return "WL_NO_SHIELD";
    case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
    case WL_CONNECTED:
        return "WL_CONNECTED";
    case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    default:
        return "UNKNOWN";
    }
}

// This is what me must do to get it all running
void otaInit()
{

    printf("Setting ESP32 as Station, no power savings, SSID %s\n", SSID);

    // WiFI debugging
    esp_log_level_set("wifi", ESP_LOG_VERBOSE);
    //esp_wifi_internal_set_log_level(WIFI_LOG_VERBOSE);
    //esp_wifi_internal_set_log_mod(WIFI_LOG_MODULE_ALL, WIFI_LOG_SUBMODULE_ALL, true);

    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect(true, true);
        printf("WiFi was previously connected - now disconnected\n");
    }

    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.setSleep(false);
    
    if (WiFi.begin(SSID, PASSWORD))
    {
        printf("WiFi begin success.\n");
    }
    //WiFi.setTxPower(WIFI_POWER_5dBm);
    delay(2000);

    printf("Connecting to WiFi .");
    uint8_t wifiStatus = WiFi.waitForConnectResult();

    // Successful connection?
    if ((wl_status_t)wifiStatus != WL_CONNECTED)
    {
        printf("WiFi Status: %s, exiting\n", wl_status_to_string((wl_status_t)wifiStatus));
        while (1)
            ;
    }

    // Just some information about the Wifi
    printf(
        "\nRunning on SSID %s with IP %s, Channel %d, MAC address %s\n",
        (char *)WiFi.SSID().c_str(),
        (char *)(WiFi.localIP()).toString().c_str(),
        WiFi.channel(),
        (char *)WiFi.macAddress().c_str());

    // This gets printed when you type the IP address followed by /whoami
    server.on("/whoami", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", projectDesc); });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  printf("Sending response started\n");
                  request->send(
                      200,
                      "text/html",
                      R"(
        <!DOCTYPE HTML>
        <head><title>)" +
                          String(projectTitle) +
                          R"(</title></head>
        <body style='font-size:1.1em;'>
          <h3>Arduino ESP OTA Home Page</h3>

          <div>Congratulations! You've reached your )" +
                          String(projectTitle) +
                          R"( ESP32/8266 device on IP )" +
                          String((char *)(WiFi.localIP()).toString().c_str()) +
                          R"(</div>

         <fieldset style="margin:20px 5px 20px 5px;border:1px;border-radius:5px;border:1px solid darkgray;background-color:white">
          <legend style="font-size:0.8em;">Project description:</legend>
            <div style="font-size:0.8em;background-color:#EEEEEE;padding:5px;border-radius:10px;">)" +
                          String(projectDesc) +
                          R"(</div>
          </fieldset>

          <p>To update your device from the Arduino IDE choose:
          <ul>
            <li><b>Sketch->Export compiled Binary</b>
            <li>then find the sketch folder by choosing 
            <li><b>Sketch->Show Sketch Folder</b> and then
            <li>use the 'update' link below to navigate to your '.bin' file to upload.
          </ul>
          </p>
          
          <p> To update your code type this into your address bar:<br /> 
            <a href='http://)" +
                          String((char *)(WiFi.localIP()).toString().c_str()) +
                          R"(/update' target='_blank'>http://)" + String((char *)(WiFi.localIP()).toString().c_str()) +
                          R"(/update</a> <br />
            or just click this link.
          </p>
        </body>
        </html>
      )");
                  printf("Sending response ends\n");
              });

    // Starting Async OTA web server
    AsyncElegantOTA.begin(&server);
    server.begin();
}