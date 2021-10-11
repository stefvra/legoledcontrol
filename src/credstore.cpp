#include "credstore.h"

CredStore::CredStore()
{
    EEPROM.begin(8 + 2 * (max_length + 1));
}

int CredStore::get_ssid_start()
{
    return 8;
}

int CredStore::get_pwd_start()
{
    return 8 + max_length + 1;
}

int CredStore::get_ssid_length()
{
    int length;
    EEPROM.get(0, length);
    return length;
}

int CredStore::get_pwd_length()
{
    int length;
    EEPROM.get(4, length);
    return length;
}

bool CredStore::set_pwd(String pwd)
{
    char c_pwd[100];
    pwd.toCharArray(c_pwd, 100);
    EEPROM.put(get_pwd_start(), c_pwd);
    return EEPROM.commit();
}

bool CredStore::set_ssid(String ssid)
{
    char c_ssid[100];
    ssid.toCharArray(c_ssid, 100);
    EEPROM.put(get_ssid_start(), c_ssid);
    return EEPROM.commit();
}

String CredStore::get_ssid()
{
    int ssid_start = get_ssid_start();
    char ssid[100];
    EEPROM.get(ssid_start, ssid);
    return String(ssid);
}

String CredStore::get_pwd()
{
    int pwd_start = get_pwd_start();
    char pwd[100];
    EEPROM.get(pwd_start, pwd);
    return String(pwd);
}
