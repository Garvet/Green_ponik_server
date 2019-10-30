#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <EEPROM.h>

WebServer server(80);
const char* ssid = "**********";
const char* pass = "**********";

const char* ssid_cr = "Green_ponik";
#define IP_S IPAddress(192, 168, 4, 255)
IPAddress ip_serv(192, 168, 4, 1);

#include <Green_ponik_server.h>
GP_server gp_server;

int count_delay = 0;

void setup() {
    EEPROM.begin(32);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    uint connect = 5;
    while((WiFi.waitForConnectResult() != WL_CONNECTED) && (connect>0)) {
        delay(10);
        connect--;
    }
    if(connect == 0) {
        WiFi.config(ip_serv, IP_S, IPAddress(255, 255, 255, 0), ip_serv); // Задание IP-адреса
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid_cr);
    }

    gp_server.set_fun(&server);
}

void loop() {
    gp_server.scan();
    if(count_delay == 0) {
        gp_server.oled_print();
    }
    server.handleClient();
    count_delay = (count_delay+1)%1000;
    delay(1);
}