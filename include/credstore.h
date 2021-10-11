#pragma once
#include <Arduino.h>
#include <ESP_EEPROM.h>

class CredStore {
    private:
    const int max_length = 100;
    int get_ssid_start();
    int get_pwd_start();
    int get_ssid_length();
    int get_pwd_length();
    public:
    CredStore();
    bool set_ssid(String ssid);
    bool set_pwd(String pwd);
    String get_ssid();
    String get_pwd();

};