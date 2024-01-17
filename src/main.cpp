#include <WiFi.h>
#include <ESP32Servo.h>
const char *ssid = "ESP32ap";
const char *password = "12345678";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// Variable to store the HTTP request
String header;
// Set web server port number to 80
WiFiServer server(80);
// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
String command = "stand";
String condition = "stop";
// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

#ifdef ESP32
#include <ESP32Servo.h>

// #include <GoBLE.hpp>
#include "gait.h"
#define Back_L1 12
#define Back_L2 14
#define Back_L3 27

#define Front_L1 25
#define Front_L2 33
#define Front_L3 32

#define Front_R1 5
#define Front_R2 18
#define Front_R3 19

#define Back_R1 15
#define Back_R2 2
#define Back_R3 4

#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Configuring access point...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Wait 100 ms for AP_START...");
  delay(100);
  
  Serial.println("Set softAPConfig");
  IPAddress Ip(192, 168, 1, 2);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Front_Left1.attach(Front_L1);
  Front_Left2.attach(Front_L2);
  Front_Left3.attach(Front_L3);
  
  Front_Right1.attach(Front_R1);
  Front_Right2.attach(Front_R2);
  Front_Right3.attach(Front_R3);

  Back_Left1.attach(Back_L1);
  Back_Left2.attach(Back_L2);
  Back_Left3.attach(Back_L3);

  Back_Right1.attach(Back_R1);
  Back_Right2.attach(Back_R2);
  Back_Right3.attach(Back_R3);
  stand();

  delay(2000);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {          
    currentTime = millis();
    previousTime = currentTime;                   // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /Forward") >= 0) {
              Serial.println("Forward");
              command = "Forward";
              // digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /Backward") >= 0) {
              Serial.println("Backward");
              command = "Backward";
              // digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /TurnLeft") >= 0) {
              Serial.println("TurnLeft");
              command = "TurnLeft";
              // digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /TurnRight") >= 0) {
              Serial.println("TurnRight");
              command = "TurnRight";
              // digitalWrite(output27, LOW);
            } else if (header.indexOf("GET /Stop") >= 0){
              Serial.println("Stop");
              command = "Stop";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
               
            client.println("<p><a href=\"/Forward\"><button class=\"button\">Forward</button></a></p>");
            client.println("<p><a href=\"/Backward\"><button class=\"button\">Backward</button></a></p>");
            client.println("<p><a href=\"/Stop\"><button class=\"button\">Stop</button></a></p>");
            client.println("<p><a href=\"/TurnLeft\"><button class=\"button\">TurnLeft</button></a></p>");
            client.println("<p><a href=\"/TurnRight\"><button class=\"button\">TurnRight</button></a></p>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    //if condition
    //forward
  }
  if(command == "Forward"){
    Forward(1);
  }else if(command == "Backward"){
    Backward(1);
  }else if(command == "Stop"){
    // resetF();
    // resetB();
    stand();
  }else if(command == "TurnLeft"){
    TurnLeft();
  }else if(command == "TurnRight" ){
    TurnRight();
  }
  if(i == 1){
    resetF();
  }else if(j == 1){
    resetB();
  }
}



