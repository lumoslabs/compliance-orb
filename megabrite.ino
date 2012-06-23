#define clockpin 13 // CI
//#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI

//#define clockpin 13 //13 // CI
#define enablepin 6 //10 // EI
//#define latchpin 5 //9 // LI
//#define datapin 11 //11 // DI

#define old_enablepin 10

 
//void WriteLEDArray() {
// 
//    SB_CommandMode = B00; // Write to PWM control registers
//    for (int h = 0;h<NumLEDs;h++) {
//	  SB_RedCommand = LEDChannels[h][0];
//	  SB_GreenCommand = LEDChannels[h][1];
//	  SB_BlueCommand = LEDChannels[h][2];
//	  SB_SendPacket();
//    }
// 
//    delayMicroseconds(15);
//    digitalWrite(latchpin,HIGH); // latch data into registers
//    delayMicroseconds(15);
//    digitalWrite(latchpin,LOW);
// 
//    SB_CommandMode = B01; // Write to current control registers
//    for (int z = 0; z < NumLEDs; z++) SB_SendPacket();
//    delayMicroseconds(15);
//    digitalWrite(latchpin,HIGH); // latch data into registers
//    delayMicroseconds(15);
//    digitalWrite(latchpin,LOW);
// 
//}

void setupMegabrite() {
  int initRgb[3] = {0,0,0};
  //pinMode(2, OUTPUT);
  //analogWrite(2, 255);
  //WiFi.init();
  //ZG2100_CSoff();
  //WiFi.reset();
  //LED3_on();
  //analogWrite(2, 0);   
  pinMode(old_enablepin, OUTPUT);
  pinMode(datapin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(enablepin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
   
  digitalWrite(latchpin, LOW);
  
  //enableLight();
  //shiftMegabrite(initRgb);
  //disableLight();
}

int lastShift[3] = {0,0,0};

void enableLight(){
  digitalWrite(old_enablepin, HIGH);
  digitalWrite(enablepin, LOW);
  //reShift();
}

void disableLight(){
  digitalWrite(enablepin, HIGH);
  digitalWrite(old_enablepin, LOW);
}

void reShift() {
  //Serial.println("reShifting...");
  //outputRgb(lastShift);
  shiftMegabrite(lastShift);
}
 
void shiftMegabrite(int* rgb) {
  int i = 3;
  while(i--) {
    //LEDChannels[0][i] = rgb[i];
    lastShift[i] = rgb[i];
  }
  crossFade(rgb[0],rgb[1],rgb[2]);
  //WriteLEDArray();
}
