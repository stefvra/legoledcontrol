#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>


byte MCP_adress = 0x20; 
byte IO_register = 0x00;
byte GPIO_register = 0x0A;
byte GPIO_state = 0x00;

int led, state;

WiFiServer server(80);//Service Port



void send_pin_state() {
  Wire.beginTransmission(MCP_adress);
  Wire.write(GPIO_register);            // select IO register
  Wire.write(GPIO_state);            // sends instruction byte  
  Wire.endTransmission();     // stop transmitting
  
}



void set_pins_to_output() {
  Wire.beginTransmission(MCP_adress);
  Wire.write(IO_register);            // select IO register
  Wire.write(0x00);            // set all pins to output 
  Wire.endTransmission();     // stop transmitting
}



void turn_off_LED(int led) {
  byte and_byte = ~(0x01 << led);
  GPIO_state &= and_byte;
  send_pin_state();
  
}

void turn_on_LED(int led) {
  byte or_byte = 0x01 << led;
  GPIO_state |= or_byte;
  send_pin_state();
  
}



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");
  
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");


  Wire.begin(0, 2);
  set_pins_to_output();

}

void loop() {
  // put your main code here, to run repeatedly:
 
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  
  Serial.println("new client");
  while(!client.available()){
  delay(1);
  }
  
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  led = request.substring(5, 6).toInt();
  state = request.substring(7, 8).toInt();
  Serial.println(request);
  client.flush();


  if (state) {
    turn_on_LED(led);
  } else {
    turn_off_LED(led);
  };


  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>"); 
  client.println("<p>request received:</p>");
  client.println("<p>" + request + "</p>");
  client.println("<p>led:" + String(led) + "</p>");
  client.println("<p>state:" + String(state) + "</p>");
  client.println("<p>" + String(GPIO_state) + "</p>");
  client.println("</html>");


  
    

}