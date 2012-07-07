/*
 * Use this area for relatively high level constants which more or less come together as "app config" variables.
 * Anything not really relevant to high level behavior should probably go in its respective file.
 * Also, until a better way of doing this is implemented, anything which private.h can override needs to be set here, before the include of private.h.
 */

// Server Config
#define DEST_PATH                  "/history"
#define TARGET_IP                  {192,168,1,163}
#define TARGET_PORT                8887

// Delay and Timing
#define REQUEST_DELAY_TIME         60000 //ms
#define REQUEST_RETRY_DURATION     30000 //ms
#define LOOP_DELAY_TIME            4 //seconds
#define LIGHT_SHIFT_DELAY_TIME     5000 //ms

// Size of Ring Buffer
#define RGB_BUFFER_SIZE 30

// OVERIDE (at least) THESE IN YOUR private.h FILE
#define AP_ESSID                    {"ap_essid"}
#define AP_PASSPHRASE               {"ap_passphrase"} // for open auth, it's ignored but still required to be set
#define AP_SECURITY_TYPE            0 // 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

//#define WIFI_TEST_MODE
//#define MEGABRITE_TEST_MODE

////BEGIN GLOBAL VARIABLES
int rgbBuffer[RGB_BUFFER_SIZE][3];
int rgbBufferNextPush = 0;
int rgbBufferNextPop = 0;

// Time (in millis) when more data should be retrieved 
long updateTime = 0;

// Time (in millis) when the data should be shifted to the light
long lightShiftTime = 0;

boolean got_data = false;
////END GLOBAL VARIABLES


//NOTE: You must create this file, it is gitignored.
//  Override any of the above values in it to avoid accidentally checking in personal/experimental values.
//  It's at least required to override AP_ESSID, AP_PASSPHRASE, AP_SECURITY_TYPE
#include "private.h"

void setup() {
  Serial.begin(57600);
  Serial.println("Setting up modules...");

  // Initialize the pins, etc for megabrite
  setupMegabrite();

  setupWifi();
  
  Serial.println("Starting...");
  enableLight();
}

void loop() {
  int* shifted_rgb;
  int i;
  
  // Check if it's time to get an update
  if (millis() >= updateTime) {
    disableLight();

    fetchData();

    updateTime +=REQUEST_DELAY_TIME;
    enableLight();
  }
  
  while(shifted_rgb = popRgb()){
    i = -1;
    Serial.println("Shifting light...");
    while(++i < 3) Serial.println(shifted_rgb[i]);
    crossFade(shifted_rgb[0], shifted_rgb[1], shifted_rgb[2]);
  }
}
