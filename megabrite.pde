// Put all hardware-related megabrite logic in here

#ifndef MEGABRITE_TEST_MODE

//#define MEGABRITE_REWIRED (suggested to put this in your private.h file rather than uncomment)

#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI

#ifdef MEGABRITE_REWIRED
//#define clockpin 13 //13 // CI
#define enablepin 6 //10 // EI
//#define latchpin 5 //9 // LI
//#define datapin 11 //11 // DI
#endif

#define old_enablepin 10 // This is the pin which overlaps with the diamondback wifi enablepin. We can (in theory) use this to disable the wifi.
 
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

void setupMegabrite() {
  pinMode(old_enablepin, OUTPUT);
  pinMode(datapin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(enablepin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);

  //This doesn't seem like a good place for this, moved it to the start of setColor
  //digitalWrite(latchpin, LOW);
}

int lastShift[3] = {0,0,0};

void enableLight(){
  digitalWrite(old_enablepin, HIGH);
  digitalWrite(enablepin, LOW);
  delayMicroseconds(15);
  //TODO: delegate to crossfader here to resume any paused transitions
}

void disableLight(){
  digitalWrite(enablepin, HIGH);
  digitalWrite(old_enablepin, LOW);
  delayMicroseconds(15);
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

void setupMegabrite() {}

void enableLight() {}

void disableLight() {}

#endif