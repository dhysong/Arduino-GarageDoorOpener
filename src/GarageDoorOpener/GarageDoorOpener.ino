#include <SPI.h>
#include <Ethernet.h>
#include <VirtualWire.h>

#define maxLength 25

#define doorstatusCycle 10000U
#define serverCycle 1U

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);
String inString = String(maxLength);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);
const int triggerPin1 = 8;
const int triggerPin2 = 9;

unsigned long doorStatusLastMillis = 0;
unsigned long serverLastMillis = 0;

String door1OpenState = "Open";
String door2OpenState = "Open";

boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle) 
{
  unsigned long currentMillis = millis();
  if(currentMillis - *lastMillis >= cycle)
  {
    *lastMillis = currentMillis;
    return true;
  }
  else
    return false;
}

void setup() {
  Serial.begin(9600);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2400);	 // Bits per sec
  vw_set_rx_pin(6);           // We will be receiving on pin 7 (Mega) ie the RX pin from the module connects to this pin. 

  vw_rx_start();       // Start the receiver PLL running
  
  //Connect to opener
  pinMode(triggerPin1, OUTPUT);
  digitalWrite(triggerPin1, HIGH);
  pinMode(triggerPin2, OUTPUT);
  digitalWrite(triggerPin2, HIGH);
  Serial.println("*** Ready.");
  
}

void loop() {  
  
  if(cycleCheck(&doorStatusLastMillis, doorstatusCycle))
  {    
     uint8_t buf[VW_MAX_MESSAGE_LEN];
     uint8_t buflen = VW_MAX_MESSAGE_LEN;
     
     if (vw_get_message(buf, &buflen)) // Non-blocking
      {
  	int i;
  
    	// Message with a good checksum received, dump it.
    	char Sensor1CharMsg[buflen]; 
        Serial.println(buflen);
    	for (i = 0; i < buflen; i++)
    	{
    	    //Serial.print(buf[i]);            
            Sensor1CharMsg[i] = char(buf[i]);
    
    	}    
        door1OpenState = (Sensor1CharMsg);
        door1OpenState.trim();
        Serial.println(door1OpenState);
      }
  }
  
  if(cycleCheck(&serverLastMillis, serverCycle))
  {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
      Serial.println("new client");    
      
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          if (inString.length() < maxLength) {
            inString += c;
          } 
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/javascript; charset=utf-8");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connnection: Keep-Alive");
            client.println();
                     
            if (inString.indexOf("?door=1") > -1) {
                Serial.println("*** Triggering door1.");
                digitalWrite(triggerPin1, LOW);
                delay(1000);
                digitalWrite(triggerPin1, HIGH);
                Serial.println("*** Complete.");
                //client.println("{\"Door1\": \"" + door1OpenState + "\", \"Door2\": \"" + door2OpenState + "\"}");
            }
            else if (inString.indexOf("?door=2") > -1) {
                Serial.println("*** Triggering door2.");
                digitalWrite(triggerPin2, LOW);
                delay(1000);
                digitalWrite(triggerPin2, HIGH);
                Serial.println("*** Complete.");
                //client.println("{\"Door1\": \"" + door1OpenState + "\", \"Door2\": \"" + door2OpenState + "\"}");
            }
            else if (inString.indexOf("?status") > -1) {
                Serial.print("Door 1 state: ");
                Serial.println(door1OpenState);
                
                if(door1OpenState.indexOf("Open") >=0){ //Hack
                  door1OpenState = "Open";
                }
                else{
                  door1OpenState = "Closed";
                }
                
                //door1OpenState = "Closed";
                client.println("{ \"Door1\": \"" + door1OpenState + "\", \"Door2\": \"Open\" }");
            }
            
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } 
          else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      //delay(1);
      inString = "";
      // close the connection:
      client.stop();
      Serial.println("client disonnected");
    }
  }
}
 


