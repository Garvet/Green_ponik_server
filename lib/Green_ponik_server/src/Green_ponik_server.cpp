#include "Green_ponik_server.h"


void handleRoot() {
    GP_pointer.server->send(200, "text/html", MAIN_page);
}

void handleNotFound() {
    GP_pointer.server->send(404, "text/html", ERR_page);
}

void get_data() {
    char buf[120];
    char str[12];

    snprintf_P(str, COUNTOF(str), PSTR("%2.2f C"), GP_pointer.temp);
    GP_pointer.json["DFRobot"]["temperature"] = str;

    snprintf_P(str, COUNTOF(str), PSTR("%2.4f"), GP_pointer.ph_value);
    GP_pointer.json["DFRobot"]["ph"] = str;

    snprintf_P(str, COUNTOF(str), PSTR("%3.2f"), GP_pointer.tds_value);
    GP_pointer.json["DFRobot"]["tds"] = str;

    serializeJsonPretty(GP_pointer.json, buf);
    GP_pointer.server->send(200, "text/html", buf);
    GP_pointer.json.clear();
}


void get_ph() {
    char buf[120];
    char str[12];

    snprintf_P(str, COUNTOF(str), PSTR("%2.4f"), GP_pointer.ph_value);
    GP_pointer.json["DFRobot"]["ph"] = str;

    serializeJsonPretty(GP_pointer.json, buf);
    GP_pointer.server->send(200, "text/html", buf);
    GP_pointer.json.clear();
}

void get_tds() {
    char buf[120];
    char str[12];

    snprintf_P(str, COUNTOF(str), PSTR("%3.2f"), GP_pointer.tds_value);
    GP_pointer.json["DFRobot"]["tds"] = str;


    serializeJsonPretty(GP_pointer.json, buf);
    GP_pointer.server->send(200, "text/html", buf);
    GP_pointer.json.clear();
}


void GP_server::initialization() {
    GP_pointer.server->on("/", handleRoot);
    GP_pointer.server->on("/get_data", get_data);
    GP_pointer.server->on("/get_ph", get_ph);
    GP_pointer.server->on("/get_tds", get_tds);
    GP_pointer.server->onNotFound(handleNotFound);
    GP_pointer.server->begin();
}


void GP_server::creat_PH(DFRobot_ESP_PH *new_PH) {
    if (new_PH != nullptr)
        GP_pointer.ph = new_PH;
    else {
        GP_pointer.ph = new DFRobot_ESP_PH;
        pinMode(PH_PIN,INPUT);
        GP_pointer.ph->begin();
        create_PH = true;
    }
}

void GP_server::creat_TDS(GravityTDS *new_TDS) {
    if (new_TDS != nullptr)
        GP_pointer.tds = new_TDS;
    else {
        GP_pointer.tds = new GravityTDS;
        float Kvalue = TDS_COEFFICIENT;
        byte *pp = (byte*)&(Kvalue);
        for(int i = 0; i < sizeof(Kvalue); i++) 
            EEPROM.write(TDS_EEPROMADDRESS+i, pp[i]);
        GP_pointer.tds->setPin(TDS_PIN);
        GP_pointer.tds->setAref(TDS_ESPVOLTAGE);  
        GP_pointer.tds->setAdcRange(TDS_ESPADC);
        GP_pointer.tds->setKvalueAddress(TDS_EEPROMADDRESS);
        GP_pointer.tds->begin();
        create_TDS = true;
    }
}

GP_server::GP_server(WebServer *new_server, DFRobot_ESP_PH *new_PH, GravityTDS *new_TDS) {
    GP_pointer.server = new_server;
    creat_PH(new_PH);
    creat_TDS(new_TDS);
    init_display();
}

void GP_server::set_fun(WebServer *new_server, DFRobot_ESP_PH *new_PH, GravityTDS *new_TDS){
    GP_pointer.server = new_server;
    creat_PH(new_PH);
    creat_TDS(new_TDS);
    init_display();
    initialization();
}

GP_server::~GP_server() {
    if (create_PH && GP_pointer.ph != nullptr)
        delete GP_pointer.ph;
    if (create_TDS && GP_pointer.tds != nullptr)
        delete GP_pointer.tds;
}

void GP_server::scan() {
    float ph_voltage;
    GP_pointer.temp = 25; //temperature = readTemperature(); 
    
    ph_voltage = analogRead(PH_PIN) / PH_ESPADC * PH_ESPVOLTAGE;
    GP_pointer.ph_value = GP_pointer.ph->readPH(ph_voltage, GP_pointer.temp);
    //GP_pointer.ph_value = 0.0;

    GP_pointer.tds->setTemperature(GP_pointer.temp);  
    GP_pointer.tds->update();
    GP_pointer.tds_value = GP_pointer.tds->getTdsValue();
}

void GP_server::init_display() {
    Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Disable*/);
    Heltec.display->flipScreenVertically();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->clear();
    Heltec.display->display();
}

void GP_server::oled_print() {
    char str[20];
    IPAddress ip = WiFi.localIP();
    
    Heltec.display->clear();

    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, ("WiFi: " + WiFi.SSID()));
    snprintf_P(str, COUNTOF(str), PSTR("IP: %d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
    Heltec.display->drawString(0, 12, str);

    Heltec.display->setFont(ArialMT_Plain_16);
    snprintf_P(str, COUNTOF(str), PSTR("PH:  %2.4f"), GP_pointer.ph_value);
    Heltec.display->drawString(0, 24, str);
    snprintf_P(str, COUNTOF(str), PSTR("DTS: %3.2f"), GP_pointer.tds_value);
    Heltec.display->drawString(0, 40, str);

    Heltec.display->display();
}