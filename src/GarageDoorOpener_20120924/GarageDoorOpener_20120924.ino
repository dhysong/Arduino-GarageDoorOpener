
#include <SPI.h>
#include <Ethernet.h>
#include <VirtualWire.h>
#define maxLength 25

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
  
   uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;
    
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
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<script src=\"//ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js\"></script>");
          client.println("</head>");
          client.println("<body>");

          if (vw_get_message(buf, &buflen)) // Non-blocking
          {
      	        int i;
      
        	// Message with a good checksum received, dump it.
        	Serial.print("Distance: ");
        	
        	for (i = 0; i < buflen; i++)
        	{
        	    Serial.write(buf[i]);
        	    Serial.print(" ");
        	}
        	Serial.println("");
          }
          
          client.println("<div style=\"width:300px; height: 400px; background-color: #000; -moz-border-radius: 15px; border-radius: 15px;\">");
          client.println("<div onclick=\"document.location=$(location).attr('pathname')+'?1';\" style=\"cursor: pointer; margin-left: 12px; margin-top: 10px; float: left; width: 132px; height: 200px; background-color: #CCC; font-size: 48pt; text-align: center; -moz-border-radius: 5px; border-radius: 5px; font-family: arial\">I</div>");
          client.println("<div onclick=\"document.location=$(location).attr('pathname')+'?2';\" style=\"cursor: pointer; margin-left: 12px; margin-top: 10px; float: left; width: 132px; height: 200px; background-color: #CCC; font-size: 48pt; text-align: center; -moz-border-radius: 5px; border-radius: 5px; font-family: arial\">II</div>");
          client.println("</div>");
          client.println("</div>");
          client.println("</body>");
          client.println("</html>");
          
          if (inString.indexOf("?1") > -1) {
              Serial.println("*** Opening door1.");
              digitalWrite(triggerPin1, LOW);
              delay(1000);
              digitalWrite(triggerPin1, HIGH);
              Serial.println("*** Complete.");
          }
          else if (inString.indexOf("?2") > -1) {
              Serial.println("*** Opening door2.");
              digitalWrite(triggerPin2, LOW);
              delay(1000);
              digitalWrite(triggerPin2, HIGH);
              Serial.println("*** Complete.");
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
    delay(1);
    inString = "";
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


