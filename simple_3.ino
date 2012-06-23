/*
 * A simple sketch that uses WiServer to get the hourly weather data from LAX and prints
 * it via the Serial API
 */

#include <WiServer.h>

#define WIRELESS_MODE_INFRA        1
#define WIRELESS_MODE_ADHOC	   2

#define REQUEST_DELAY_TIME         60000 //ms
#define REQUEST_RETRY_DURATION     30000 //ms
#define LOOP_DELAY_TIME            4 //seconds
#define LIGHT_SHIFT_DELAY_TIME     5000 //ms
#define DEST_PATH                  "/history"
#define TARGET_IP                  {192,168,1,163}
#define TARGET_PORT                8887
#define FADE_SPEED                 SLOW_ASS_FADE


///////START PRIVATE DEFINES; OVERRIDE THESE IN private.h WHICH IS GITIGNORED
#define AP_ESSID                    {"ap_essid"}
#define AP_PASSPHRASE               {"ap_passphrase"} // for open auth, it's ignored but still required to be set
#define AP_SECURITY_TYPE            0 // 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

//These should also go into your private file so they don't catch folks off guard.

//#define DEV_MODE
//#define DEMO_MODE

///////END PRIVATE DEFINES
#include "private.h"


///////START CONDITIONAL OVERRIDES
#ifdef DEMO_MODE

#define FADE_SPEED                 FAST_FADE
#define LIGHT_SHIFT_DELAY_TIME     400 //ms
#define DEST_PATH                  "/demo/coryvirok"

#endif

#ifdef DEV_MODE

#define TARGET_IP                  {192,168,1,147}
//#define DEST_PATH                  "/"
#define TARGET_PORT                8000

#endif
///////END CONDITIONAL OVERRIDES


// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,196};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = AP_ESSID;		// max 32 bytes

unsigned char security_type = AP_SECURITY_TYPE;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = AP_PASSPHRASE;	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
				};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------

// IP Address for www.weather.gov  
uint8 ip[] = TARGET_IP;

#define GET_COLORS_PARAMS ip, TARGET_PORT, "fakedomain.com", DEST_PATH

GETrequest getColors(GET_COLORS_PARAMS);

#define RGB_BUFFER_SIZE 30

int rgbBuffer[RGB_BUFFER_SIZE][3];
int rgbBufferNextPush = 0;
int rgbBufferNextPop = 0;


void setup() {
  // Initialize the pins, etc for megabrite
  setupMegabrite();

  // Initialize WiServer (we'll pass NULL for the page serving function since we don't need to serve web pages) 
  WiServer.init(NULL);
  
  // Enable Serial output and ask WiServer to generate log messages (optional)
  Serial.begin(57600);
  WiServer.enableVerboseMode(false);

  // Have the processData function called when data is returned by the server
  getColors.setReturnFunc(handleData);
  Serial.println("Starting...");
  enableLight();
}


// Time (in millis) when the data should be retrieved 
long updateTime = 0;

// Time (in millis) when the data should be shifted to the light
long lightShiftTime = 0;

boolean got_data = false;

void loop(){
  int* shifted_rgb;
  int i;
  long timer = 0;
  
  // Check if it's time to get an update
  if (millis() >= updateTime) {
    disableLight();
    timer = millis()+REQUEST_RETRY_DURATION;
    Serial.println("fetch");
    got_data = false;
    while(!got_data && millis() < timer) {
          Serial.println("trying");
      getColors.submit();
      //testHandleData();
      
      WiServer.server_task();
      delay(10);
    }
    updateTime +=REQUEST_DELAY_TIME;
    enableLight();
  }
  
  //if (millis() >= lightShiftTime) {
    //Serial.println("Checking light buffer...");
    //shifted_rgb = popRgb();
    while(shifted_rgb = popRgb()){
      i = -1;
      Serial.println("Shifting light...");
      while(++i < 3) Serial.println(shifted_rgb[i]);
      shiftMegabrite(shifted_rgb);
    }
 
    //lightShiftTime = millis()+LIGHT_SHIFT_DELAY_TIME;
  //}
}
