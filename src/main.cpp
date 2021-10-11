#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "driver.h"
#include "controller.h"
#include "credstore.h"
#include "secrets.h"
//#include <ESP8266WebServer.h>


const char *ssid = SSID;     // your ssid defined in secrets.h
const char *password = PWD; // your password defined in secrets.h

Driver led_driver = Driver();
CredStore store = CredStore();
Controller led_controller = Controller(&led_driver, &store);
String serial_buffer;


//ESP8266WebServer server(80);

 
void setup()
{
    // put your setup code here, to run once:

    Serial.begin(9600);

    WiFi.begin(ssid, password);

    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

    led_controller.init();
    led_driver.init();

    Serial.print("pwd: ");
    Serial.println(store.get_pwd());
    Serial.print("ssid: ");
    Serial.println(store.get_ssid());

    Serial.println("setting credentials...");

    store.set_pwd("paswoord");
    store.set_ssid("netwerknaam");


    Serial.print("pwd: ");
    Serial.println(store.get_pwd());
    Serial.print("ssid: ");
    Serial.println(store.get_ssid());


}

void loop()
{
    // put your main code here, to run repeatedly:

    led_controller.run();
    serial_buffer = Serial.readStringUntil('\n');
    Serial.println("echo: " + serial_buffer);
    delay(500);
}
