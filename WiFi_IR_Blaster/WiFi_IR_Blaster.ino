/*
 * WiFi IR Blaster by Buddy Crotty
 *  Use an ESP8266 module or dev board to receive HTTP GET request
 *  and then send IR codes to an attached IR LED based on those requests.
 *  This works best with another web server acting as a front end that 
 *  sends cURL requests based on which buttons are pressed. 
 *  cURL format: http://ESP8266/IRcode
 */

#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <ESP8266mDNS.h>

const char* ssid = "DIR2B";
const char* password = "%defence:095";
MDNSResponder mdns;

int khz = 38; // 38kHz carrier frequency for both NEC and Samsung

IRsend irsend(4); //an IR led is connected to GPIO4 (pin D2 on NodeMCU)

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

//Insert codes in Global Cache format
unsigned int benqOn[71] = {38000,1,1,340,170,23,19,23,19,23,19,23,19,23,19,23,19,23,19,23,19,23,19,23,19,23,19,23,19,23,61,22,61,23,19,23,19,23,61,23,61,23,61,23,61,23,19,22,20,22,61,23,19,23,19,23,19,23,19,23,19,23,61,23,61,23,19,23,61,22,1619};
unsigned int benqOff[71] = {38000,1,1,343,172,23,19,22,20,22,19,23,19,23,19,23,19,23,19,23,19,23,19,23,20,23,19,23,19,23,61,23,61,23,19,22,20,22,19,23,61,23,61,23,61,23,19,23,19,23,61,23,19,23,61,22,19,23,19,23,19,23,61,23,61,23,19,23,61,23,1637};

void setup() {
  Serial.begin(115200);
  delay(10);

  irsend.begin();
  
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
  Serial.println("WiFi Connected");

  // Start the server
  server.begin();
  Serial.println("HTTP Server Started");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("IRBlasterLR", WiFi.localIP())) {
    Serial.println("MDNS Responder Started");
  }

  Serial.println();
  Serial.println();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  if (req.indexOf("/benqOn") != -1){
      irsend.sendGC(benqOn, 71);   
      Serial.println("Sent benqOn");
  }
  else if (req.indexOf("/benqOff") != -1){
      irsend.sendGC(benqOff, 71);
      delay(2000);
      irsend.sendGC(benqOff, 71);   
      delay(2000);
      irsend.sendGC(benqOff, 71);
      Serial.println("Sent benqOff");
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  
  client.flush();
   
  // Send the response to the client
  //client.print(s);
  client.print("HTTP/1.1 200 OK\r\n");
  delay(1);
  Serial.println("Client Disconnected");
  Serial.println();
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}
