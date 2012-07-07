// Put all logic related to fading from color to color in here
#define FADE_SPEED SLOW_ASS_FADE
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
int repeat = 3;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat
 
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
  

  float stepR = calculateStep(redVal, R);
  float stepG = calculateStep(grnVal, G); 
  float stepB = calculateStep(bluVal, B);

  for (int i = 0; i <= numTransitions; i++) {
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    setColor(redVal, grnVal, bluVal);

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop
  }
  
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}
