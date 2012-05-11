/*
 _   _                       _      
| \ | | __ _ _ __   ___   __| | ___ 
|  \| |/ _` | '_ \ / _ \ / _` |/ _ \
| |\  | (_| | | | | (_) | (_| |  __/
|_| \_|\__,_|_| |_|\___/ \__,_|\___|
                                    
 _____                    __        __   _       ____  _ 
| ____|_ __ ___   ___  _ _\ \      / /__| |__   |  _ \/ |
|  _| | '_ ` _ \ / _ \| '_ \ \ /\ / / _ \ '_ \  | |_) | |
| |___| | | | | | (_) | | | \ V  V /  __/ |_) | |  __/| |
|_____|_| |_| |_|\___/|_| |_|\_/\_/ \___|_.__/  |_|   |_|

Reading P1 Companion Standard of Dutch Smart Meter through external RX with an Arduino.
You can't use SoftwareSerial because you need to change the default settings of the USART like partity and stop bits.
EN_TX should be enabled during reading of data, but disabled if you want to upload your sketch!
You need to convert the data signal with an 7404.

Based on emonBase - Documentation: http://openenergymonitor.org/emon/emonbase

Original authors: Trystan Lea and Glyn Hudson
Adjusted for reading P1 port and posting to emonWeb.org: Frank Oxener
Part of the: openenergymonitor.org project
Licenced under GNU GPL V3
http://openenergymonitor.org/emon/license

EtherCard Library by Jean-Claude Wippler and Andrew Lindsay
*/

//#define DEBUG     //comment out to disable serial printing to increase long term stability 
#define UNO       //anti crash wachdog reset only works with Uno (optiboot) bootloader, comment out the line if using delianuova
#define START_PARAMETERS  "auth_token=qqbUzX2V8UKUDwxZyBhR&P1=" //Use auth_token from your own account

#include <avr/wdt.h>
#include <EtherCard.h>  //https://github.com/jcw/ethercard 

// ethernet interface mac address, must be unique on the LAN
byte mymac[6] = { 0x00,0x04,0xA3,0x21,0xC8,0x46};
byte sd;

byte Ethernet::buffer[700];

//Domain name of remote webserver - leave blank if posting to IP address 
char website[] PROGMEM = "emonweb.org";
//static byte hisip[] = { 192, 168, 2, 34};    // un-comment for posting to static IP server (no domain name) 

const int redLED = 6;                     // NanodeRF RED indicator LED
const int requestPin =  4;

int ethernet_error = 0;                   // Etherent (controller/DHCP) error flag
int rf_error = 0;                         // RF error flag - high when no data received 
int ethernet_requests = 0;                // count ethernet requests without reply

int dhcp_status = 0;
int dns_status = 0;

int data_ready=0;                         // Used to signal that emontx data is ready to be sent

boolean start_p1_record;
Stash stash;                              //Use the RAM inside the ENC28J60 Ethernet controller to post data from P1 port
                                          //See http://jeelabs.org/2012/04/11/ethercard-improvements/

//**********************************************************************************************************************
// SETUP
//**********************************************************************************************************************
void setup () {

  //High means off since NanodeRF tri-state buffer inverts signal
  pinMode(redLED, OUTPUT); digitalWrite(redLED,LOW);
  delay(100); digitalWrite(redLED,HIGH);                      // turn off redLED

  Serial.begin(9600);
  UCSR0C = (2<<UPM00)|(0<<USBS0)|(2<<UCSZ00)|(0<<UCPOL0);     // 7-E-1 needed for reading P1

  Serial.println("\n[webClient]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
    Serial.println( "Failed to access Ethernet controller");
    ethernet_error = 1;
  }

  dhcp_status = 0;
  dns_status = 0;
  ethernet_requests = 0;
  ethernet_error=0;

  sd = stash.create();
  stash.print(START_PARAMETERS);

  #ifdef UNO
  wdt_enable(WDTO_8S);
  #endif;

  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, HIGH);

}

//**********************************************************************************************************************
// LOOP
//**********************************************************************************************************************
void loop () {

  #ifdef UNO
  wdt_reset();
  #endif

  dhcp_dns();  // handle dhcp and dns setup - see dhcp_dns tab

  // Display error states on status LED
  if (ethernet_error == 1 || ethernet_requests > 0) digitalWrite(redLED,LOW);
    else digitalWrite(redLED,HIGH);

  //-----------------------------------------------------------------------------------------------------------------
  // Receive data from P1
  //-----------------------------------------------------------------------------------------------------------------
  while (Serial.available() > 0) {
    digitalWrite(redLED, LOW);
    char inChar = (char)Serial.read();
    if (inChar == '/') {
      start_p1_record = true;
    }
    if (start_p1_record == true) {
      stash.print(inChar);
      #ifdef DEBUG
        Serial.write(inChar);
      #endif
    }
    if (inChar == '!') {
      stash.save();
      digitalWrite(redLED, HIGH);
      data_ready = 1;
      start_p1_record = false;
    }
  }

  //-----------------------------------------------------------------------------------------------------------------
  // Send data via ethernet
  //-----------------------------------------------------------------------------------------------------------------
  ether.packetLoop(ether.packetReceive());

  if (data_ready) {
    ethernet_requests ++;
    stash.save();
    Stash::prepare(PSTR("POST http://$F/api HTTP/1.0" "\r\n"
                        "Host: $F" "\r\n"
                        "User-Agent: Nanode EtherCard lib" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"),
                        website, website, stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();

    ethernet_requests = 0;  //Is this really functional??
    ethernet_error = 0;     //Is this really functional??
    data_ready = 0;

    sd = stash.create();
    stash.print(START_PARAMETERS);
  }

  if (ethernet_requests > 10) delay(10000); // Reset the nanode if more than 10 request attempts have been tried without a reply

}
