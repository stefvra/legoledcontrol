#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "driver.h"
#include "credstore.h"
#include <Regexp.h>


class Controller
{
    bool debug = true;
    Driver* driver;
    CredStore* store;
    WiFiServer* server;
    const unsigned int max_nr_paths = 10;


public:
    Controller(Driver* _driver, CredStore* _store, bool _debug = true);
    ~Controller();
    void init();
    String dispatch_request(String request);
    void parse_request(String request, String* paths, int& n_paths);
    String process_driver_request(String* paths, int n_paths);
    String process_store_request(String* paths, int n_paths);
    void run();
};