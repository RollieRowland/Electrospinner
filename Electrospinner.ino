//Two stepper contol
//1 pwm control

//calculate steps per second
//targetV / SPM = steps per second
#include <Servo.h>

#define AXISENA 3
#define AXISSTP 4
#define AXISDIR 5
#define SYRINGEENA 6
#define SYRINGESTP 7
#define SYRINGEDIR 8
#define SPINNERPWM 9

Servo spinnerControl;

long axisPosition    = 0;//steps
long syringePosition = 0;

int axisSPM    = 80;//steps per millimeter
int syringeSPM = 50 * 32;

float axisTargetV = 0.0f;
float syringeTargetV = 0.0f;
float spinnerTargetV = 0.0f;

float axisStepInterval = 0.0f;//seconds incrementing before next step
float syringeStepInterval = 0.0f;

bool axisHasStepped = false;
bool syringeHasStepped = false;

bool axisDirection = true;

long previousMillis;

void setup() {
  pinMode(AXISENA, OUTPUT);
  pinMode(AXISSTP, OUTPUT);
  pinMode(AXISDIR, OUTPUT);
  pinMode(SYRINGEENA, OUTPUT);
  pinMode(SYRINGESTP, OUTPUT);
  pinMode(SYRINGEDIR, OUTPUT);

  spinnerControl.attach(SPINNERPWM);

  previousMillis = millis();

  axisTargetV = 40;//mm/s
  syringeTargetV = 0.5;//mm/s
  SetSpinnerVelocity(1000);
}

void loop() {
  float aP = (float)axisPosition / (float)axisSPM;
  float sP = (float)syringePosition / (float)syringeSPM;

  //axis is out of range, flip direction, maintain velocity
  if (aP < 0 || aP > 200){
    FlipAxisDirection();
  }

  //syringe is out of range, disable
  if(sP < 0 || aP > 80){
    spinnerTargetV = 0.0f;
  }
  
  Control(aP, sP);
}

void Control(float absAxisPos, float absSyringePos){
  long currentTime = millis();
  float dT = (float)(currentTime - previousMillis) / 1000.0f;

  float axisStepInterval = ((float)axisTargetV / (float)axisSPM) * dT;
  float syringeStepInterval = ((float)syringeTargetV / (float)syringeSPM) * dT;

  if (axisHasStepped){
    StepAxisOff();
  }
  else if (axisIntervalCounter < axisStepInterval){
    axisIntervalCounter += dT;
  }
  else{
    axisIntervalCounter = 0;
    StepAxisOn();
  }
  
  if (syringeHasStepped){
    StepSyringeOff();
  }
  else if (syringeIntervalCounter < syringeStepInterval){
    syringeIntervalCounter += dT;
  }
  else{
    syringeIntervalCounter = 0;
    StepSyringeOn();
  }
  
  previousMillis = currentTime;
}

void FlipAxisDirection(){
  if (axisDirection){
    digitalWrite(AXISDIR, LOW);
    axisDirection = false;
  }
  else {
    digitalWrite(AXISDIR, HIGH);
    axisDirection = true;
  }
}

void StepAxisOn(){
  digitalWrite(AXISSTP, HIGH);
  axisHasStepped = true;
}

void StepAxisOff(){
  digitalWrite(AXISSTP, LOW);
  axisHasStepped = false;
}

void StepSyringeOn(){
  digitalWrite(SYRINGESTP, HIGH);
  syringeHasStepped = true;
}

void StepSyringeOff(){
  digitalWrite(SYRINGESTP, LOW);
  syringeHasStepped = false;
}

void SetSpinnerVelocity(float v){
  int velMap = map(v, 0, 10000, 1000, 2000);
  spinnerControl.writeMicroseconds(velMap);
}
