#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "driver.h"
#include "controller.h"
#include "credstore.h"
#include "secrets.h"
#include <ESP8266WebServer.h> // use this lib!!
//#include <ESP8266mDNS.h>
#include <uri/UriBraces.h>

// config for AP mode
#define AP_SSID "ESP8266"
#define AP_PASS "magicword"
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);


Driver led_driver = Driver();
CredStore store = CredStore();
//Controller led_controller = Controller(&led_driver, &store);
String serial_buffer;


// config for STA mode
String ssid = store.get_ssid();     // your ssid defined in secrets.h
String password = store.get_pwd(); // your password defined in secrets.h




ESP8266WebServer server(80);

 
void setup()
{
    // open serial communication

    Serial.begin(9600);

    // setup access point
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.print("AP Server IP address: ");
    Serial.println(WiFi.softAPIP());
    //WiFi.softAPConfig(local_IP, gateway, subnet);


    // connect to Wifi network
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


    
    //led_controller.init();
    led_driver.init();

    server.on(F("/"), []() {
        server.send(200, "text/plain", "hello from esp8266!");
    });

    server.on(UriBraces("/led/enable/{}"), []() {
        int index = server.pathArg(0).toInt();
        led_driver.activate_output(index);
        server.send(200, "text/plain", "LED activated");
    });

    server.on(UriBraces("/led/disable/{}"), []() {
        int index = server.pathArg(0).toInt();
        led_driver.deactivate_output(index);
        server.send(200, "text/plain", "LED deactivated");
    });

    server.on("/store/get_ssid", []() {
        server.send(200, "text/plain", store.get_ssid());
    });

    server.on("/store/get_pwd", []() {
        server.send(200, "text/plain", store.get_pwd());
    });

    server.on(UriBraces("/store/set_pwd/{}"), []() {
        String pwd = server.pathArg(0);
        store.set_pwd(pwd);
        server.send(200, "text/plain", "pwd set");
    });

    server.on(UriBraces("/store/set_ssid/{}"), []() {
        String ssid = server.pathArg(0);
        store.set_ssid(ssid);
        server.send(200, "text/plain", "ssid set");
    });

    server.on("/wifi/get_sta_ip", []() {
        server.send(200, "text/plain", WiFi.localIP().toString());
    });


    server.begin();


}

void loop()
{
    // put your main code here, to run repeatedly:

    //led_controller.run();
    server.handleClient();

    delay(500);
}
