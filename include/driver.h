#pragma once
#include <Arduino.h>
#include <Wire.h>


struct output_request {
    int index;
    bool state;
};

class Driver
{
    const int amount = 8;
    int scl_pin, sda_pin;
    bool debug = true;
    byte MCP_adress, IO_register, GPIO_register;
    bool *states;

    int power_of_2(int exponent);
    byte generate_byte();
    bool transmit(byte adress, byte reg, byte mes);

public:
    Driver();
    ~Driver();
    void init();
    bool activate_output(int n);
    bool deactivate_output(int n);
    bool set_output(int n, bool state);
    bool set_output(output_request r);
    bool send_states_to_driver();
    String byte_string();
    void serial_print_states();
};