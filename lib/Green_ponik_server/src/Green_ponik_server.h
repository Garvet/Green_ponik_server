#ifndef GREEN_PONIK_SERVER_H
#define GREEN_PONIK_SERVER_H

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <heltec.h>

#include <EEPROM.h>
#include <DFRobot_ESP_PH.h>
#include <GravityTDS.h>
#include <ArduinoJson.h>

#include "index.h"
#define COUNTOF(a) (sizeof(a)/sizeof(a[0]))

#define PH_ESPADC 3660.0   //the esp Analog Digital Convertion value
#define PH_ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 39          //the esp gpio data pin number

#define TDS_ESPADC 4096    //1024 for 10bit ADC;4096 for 12bit ADC
#define TDS_ESPVOLTAGE 3.3 //reference voltage on ADC, default 3.3V on ESP32
#define TDS_PIN 37         //the esp gpio data pin number
#define TDS_EEPROMADDRESS 8
#define TDS_COEFFICIENT 1.0

struct {
    StaticJsonDocument<120> json;
    WebServer *server;

    DFRobot_ESP_PH *ph;
    GravityTDS *tds;

    float temp;
    float ph_value;
    float tds_value;
} GP_pointer;

class GP_server {
private:
    bool create_PH = false; 
    bool create_TDS = false; 

    void creat_PH(DFRobot_ESP_PH *);
    void creat_TDS(GravityTDS *);
    void init_display();
public:
    GP_server() = default;
    ~GP_server();
    GP_server(WebServer *, DFRobot_ESP_PH *new_PH=nullptr, GravityTDS *new_TDS=nullptr);
    void set_fun (WebServer *, DFRobot_ESP_PH *new_PH=nullptr, GravityTDS *new_TDS=nullptr);

    void initialization();

    void scan();
    void oled_print();
};

#endif