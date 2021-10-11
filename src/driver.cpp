#include "driver.h"


int Driver::power_of_2(int exponent)
{
    int power = 1;
    for (int n = 0; n < exponent; n++)
        power *= 2;
    return power;
}

byte Driver::generate_byte()
{
    byte result = 0;
    for (int n = 0; n < amount; n++)
    {
        if (states[n])
            result += power_of_2(n);
    };
    return result;
}

bool Driver::transmit(byte adress, byte reg, byte mes)
{
    bool transmission_succeeded;
    if (debug)
    {
        Serial.println("starting transmission...");
        Serial.print("adress: ");
        Serial.println(adress);
        Serial.print("register: ");
        Serial.println(reg);
        Serial.print("message: ");
        Serial.println(mes);
    };
    Wire.beginTransmission(adress);
    Wire.write(reg);                                             // select IO register
    Wire.write(mes);                                             // set all pins to output
    transmission_succeeded = !(bool)(int)Wire.endTransmission(); // stop transmitting

    if (transmission_succeeded)
    {
        Serial.println("Transmission succeeded!");
    }
    else
    {
        Serial.println("Transmission not succeeded!");
    }
    return transmission_succeeded;
}

Driver::Driver()
{
    states = new bool[amount];
    for (int n = 0; n < amount; n++)
        states[n] = false;

    if (debug)
        serial_print_states();

    MCP_adress = 0x20;
    GPIO_register = 0x0A;
    IO_register = 0x00;
    scl_pin = 2;
    sda_pin = 0;
}

Driver::~Driver()
{
    Serial.println("Destructor executed");
    delete[] states;
}

void Driver::init()
{
    Wire.begin(sda_pin, scl_pin);
    byte IO_output_state = 0x00;
    while (!transmit(MCP_adress, IO_register, IO_output_state))
    {
        Serial.println("Initialization not succeeded");
        delay(100);
    };

    Serial.println("Initialization succeeded");
}

bool Driver::activate_output(int n)
{
    return set_output(n, true);
}

bool Driver::deactivate_output(int n)
{
    return set_output(n, false);
}

bool Driver::set_output(int n, bool state)
{
    if (debug)
    {
        Serial.println("old states:");
        serial_print_states();
    }
    states[n] = state;
    if (debug)
    {
        Serial.println("new states:");
        serial_print_states();
    }
    return send_states_to_driver();
}

bool Driver::set_output(output_request r)
{
    return set_output(r.index, r.state);
}

bool Driver::send_states_to_driver()
{
    byte IO_state_byte = generate_byte();
    bool transmission_succeeded;
    transmission_succeeded = transmit(MCP_adress, GPIO_register, IO_state_byte);
    return transmission_succeeded;
}

String Driver::byte_string()
{

    String byte_str = String(generate_byte(), BIN);

    if (debug)
    {
        Serial.print("byte string generated: ");
        Serial.println(byte_str);
    };

    return byte_str;

}

void Driver::serial_print_states()
{
    for (int n = 0; n < amount; n++)
    {
        Serial.print("output: ");
        Serial.print(n);
        Serial.print(", state: ");
        Serial.println(states[n]);
    }
}
