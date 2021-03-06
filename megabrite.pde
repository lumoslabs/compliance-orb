// Put all hardware-related megabrite logic in here

#ifndef MEGABRITE_TEST_MODE


//#define MEGABRITE_REWIRED (suggested to put this in your private.h file rather than uncomment)

#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI

#define old_enablepin 6 // This pin is meaningless if it hasn't been rewired, which is a good thing if MEGABRITE_REWIRED is false

#ifdef MEGABRITE_REWIRED
//#define clockpin 13 //13 // CI
#define enablepin 6 //10 // EI
//#define latchpin 5 //9 // LI
//#define datapin 11 //11 // DI

#define old_enablepin 10 // This is the pin which overlaps with the diamondback wifi enablepin. We can (in theory) use this to disable the wifi.
#endif


int SB_CommandMode;
int SB_RedCommand;
int SB_GreenCommand;
int SB_BlueCommand;
 
void SB_SendPacket() {
  if (SB_CommandMode == B01) {
   SB_RedCommand = 120;
   SB_GreenCommand = 100;
   SB_BlueCommand = 100;
  }

  SPDR = SB_CommandMode << 6 | SB_BlueCommand>>4;
  while(!(SPSR & (1<<SPIF)));
  SPDR = SB_BlueCommand<<4 | SB_RedCommand>>6;
  while(!(SPSR & (1<<SPIF)));
  SPDR = SB_RedCommand << 2 | SB_GreenCommand>>8;
  while(!(SPSR & (1<<SPIF)));
  SPDR = SB_GreenCommand;
  while(!(SPSR & (1<<SPIF)));
}

void setColor(int red, int grn, int blu) {
  digitalWrite(latchpin, LOW);
  delayMicroseconds(15);

  SB_CommandMode = B00; // Write to PWM control registers  
  SB_RedCommand = red;
  SB_GreenCommand = grn;
  SB_BlueCommand = blu;
  SB_SendPacket();
  
 
  delayMicroseconds(15);
  digitalWrite(latchpin,HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(latchpin,LOW);
 
  SB_CommandMode = B01; // Write to current control registers
  SB_SendPacket();
  delayMicroseconds(15);
  digitalWrite(latchpin,HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(latchpin,LOW);
}

bool SB_SetupPending = true;
//lazy load the setup until its needed so we don't get weird lights on startup
void ensureMegabriteSetup() {
  if(SB_SetupPending) {
    Serial.println("Setting up megabrite lazily...");
    SB_SetupPending = false;

    pinMode(old_enablepin, OUTPUT);
    pinMode(datapin, OUTPUT);
    pinMode(latchpin, OUTPUT);
    pinMode(enablepin, OUTPUT);
    pinMode(clockpin, OUTPUT);
    SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);

    digitalWrite(latchpin, LOW);
    digitalWrite(enablepin, LOW);
  }
}

int lastShift[3] = {0,0,0};

bool lightEnabled = false;

void enableLight(){
  if (!lightEnabled) {
    lightEnabled = true;
    ensureMegabriteSetup();
    
    digitalWrite(old_enablepin, HIGH);
    digitalWrite(enablepin, LOW);
    
    //TODO: delegate to crossfader here to resume any paused transitions
  }
}

void disableLight(){
  if (lightEnabled) {
    lightEnabled = false;
    digitalWrite(enablepin, HIGH);
    digitalWrite(old_enablepin, LOW);
  }
}

#else

void setColor(int red, int grn, int blu) {
  Serial.print("RGB: ");
  Serial.print(redVal);
  Serial.print(" / ");
  Serial.print(grnVal);
  Serial.print(" / ");  
  Serial.println(bluVal); 
  delayMicroseconds(60); //To mimick the original setColor function (total of 60 microseconds of delay)
}

void enableLight() {}

void disableLight() {}

#endif