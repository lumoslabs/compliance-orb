// Put all wifi logic into this file

#ifndef WIFI_TEST_MODE

#include <WiServer.h>

#define WIRELESS_MODE_INFRA        1
#define WIRELESS_MODE_ADHOC        2

unsigned char local_ip[] = LOCAL_IP; // IP address of WiShield
unsigned char gateway_ip[] = GATEWAY_IP; // router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};  // subnet mask for the local network
const prog_char ssid[] PROGMEM = AP_ESSID;    // max 32 bytes

unsigned char security_type = AP_SECURITY_TYPE; // 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = AP_PASSPHRASE;  // max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, // Key 0
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Key 1
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Key 2
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // Key 3
        };

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

uint8 ip[] = TARGET_IP;

#define GET_COLORS_PARAMS ip, TARGET_PORT, "fakedomain.com", DEST_PATH
GETrequest getColors(GET_COLORS_PARAMS);

void setupWifi() {
  // Enable Serial output and ask WiServer to generate log messages (optional)
  WiServer.enableVerboseMode(false);
}

void fetchData() {
  long timer;

  Serial.println("starting fetchData");
  got_data = false;

  WiServer.init(NULL);
  getColors.init(GET_COLORS_PARAMS);
  getColors.setReturnFunc(handleData);

  while(!got_data) {
    timer = millis()+REQUEST_RETRY_DURATION;
    request_finished = false;
    Serial.println("sending request");

    getColors.submit();
    Serial.println("request queued");
    while(!request_finished){
      if(millis() > timer){
        Serial.println("Timeout!");
        return;
      }
      WiServer.server_task();
    }
  }
  if(got_data) Serial.println("got it!");
}

#else ////START TEST CODE

void setupWifi() {}

void fetchData() {
  fakeFetchData();
}

#endif

void fakeFetchData() {
  char* data = "HTTP/1.1 200 OK\nX-Frame-Options: sameorigin\nX-XSS-Protection: 1; mode=block\nContent-Type: text/html;charset=utf-8\nContent-Length: 35\nConnection: keep-alive\nServer: thin 1.3.1 codename Triple Espresso\n\n%%%1023,0,0|0,1023,0|0,0,1023&&&";
  handleData(data, 236);
}