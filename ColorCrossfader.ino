#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI
 
#define NumLEDs 1
 
int LEDChannels[NumLEDs][3] = {0};
int SB_CommandMode;
int SB_RedCommand;
int SB_GreenCommand;
int SB_BlueCommand;

/* ------------------------------------------------------------------------------------------------------ */

#define COLOR_STEPS 1023
#define SLOW_ASS_FADE (COLOR_STEPS*4)
#define MEDIUM_FADE (COLOR_STEPS*2)
#define SEMI_FAST_FADE COLOR_STEPS
#define FAST_FADE (COLOR_STEPS/2)

float redCorrectionFactor = 1.0;
float grnCorrectionFactor = 1.0;
float bluCorrectionFactor = 1.0;

// lumos color arrays
int black[3] =  { 0, 0, 0 };
int blue[3]  =  { 0, 0, 1023 };
int purple[3] = { 1023, 0, 1023 };

// default color arrays
//int black[3]    = { 0, 0, 0 };
//int white[3]    = { COLOR_STEPS, COLOR_STEPS, COLOR_STEPS };
//int red[3]      = { COLOR_STEPS, 0, 0 };
//int green[3]    = { 0, COLOR_STEPS, 0 };
//int blue[3]     = { 0, 0, COLOR_STEPS };
//int yellow[3]   = { 409, 972, 0 };
//int dimwhite[3] = { 303, 303, 303 };

// Set initial color
float redVal = black[0];
float grnVal = black[1]; 
float bluVal = black[2];

// Initialize fading behavior
float numTransitions = float(FADE_SPEED);
int wait = 1;      // 1ms internal crossFade delay; increase for slower fades
int hold = 0;       // Optional hold when a color is complete, before the next crossFade
int DEBUG = 1;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 240; // How often should DEBUG report?
int repeat = 3;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat

// Initialize color variables
float prevR = redVal;
float prevG = grnVal;
float prevB = bluVal;

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
  SB_CommandMode = B00; // Write to PWM control registers
  for (int h = 0;h<NumLEDs;h++) {
    SB_RedCommand = red;
    SB_GreenCommand = grn;
    SB_BlueCommand = blu;
    SB_SendPacket();
  }
 
  delayMicroseconds(15);
  digitalWrite(latchpin,HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(latchpin,LOW);
 
  SB_CommandMode = B01; // Write to current control registers
  for (int z = 0; z < NumLEDs; z++) SB_SendPacket();
  delayMicroseconds(15);
  digitalWrite(latchpin,HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(latchpin,LOW);
  
  prevR = red; 
  prevG = grn; 
  prevB = blu;
}
 
float calculateStep(float prevValue, float endValue) {
  Serial.print("prevValue: ");
  Serial.println(prevValue);
  Serial.print("endValue: ");
  Serial.println(endValue);
  Serial.print("numTransitions: ");
  Serial.println(numTransitions);  

  float step = 0.0;
  float diff = endValue - prevValue; // What's the overall gap?
  if (diff) {                      // If its non-zero, 
//    step = numTransitions/step;   //   divide by COLOR_STEPS
    step = diff/numTransitions;
    Serial.print("step: ");
    Serial.println(step);
  } 
  return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1. 
*  (R, G, and B are each calculated separately.)
*/

float calculateVal(float step, float val, int i) {
  val += step;           

  // Defensive driving: make sure val stays in the range 0-COLOR_STEPS
  if (val > COLOR_STEPS) {
    val = COLOR_STEPS;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* crossFade() converts the percentage colors to a 
*  0-COLOR_STEPS range, then loops COLOR_STEPS times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void crossFade(int red, int grn, int blu) {
  int R = redCorrectionFactor * float(red);
  int G = grnCorrectionFactor * float(grn);
  int B = bluCorrectionFactor * float(blu);
  

  float stepR = calculateStep(prevR, R);
  float stepG = calculateStep(prevG, G); 
  float stepB = calculateStep(prevB, B);

  for (int i = 0; i <= numTransitions; i++) {
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);
    
    setColor(redVal, grnVal, bluVal);

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop

    if (DEBUG) { // If we want serial output, print it at the 
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(redVal);
        Serial.print(" / ");
        Serial.print(grnVal);
        Serial.print(" / ");  
        Serial.println(bluVal); 
      } 
      DEBUG += 1;
    }
  }
  
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}
