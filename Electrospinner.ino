//Two stepper contol
//1 pwm control

//calculate steps per second
//targetV / SPM = steps per second
#include <Servo.h>
#include <digitalWriteFast.h>

#define AXISENA 9
#define AXISSTP 5
#define AXISDIR 4
#define SYRINGEENA 8
#define SYRINGESTP 7
#define SYRINGEDIR 6

#define SPINNERPWM 10

Servo spinnerControl;

long axisPosition    = 0;//steps
long syringePosition = 0;

int axisSPM    = 400;//steps per millimeter
long syringeSPM = 200L * 51L * 32L;

float axisTargetV = 40.0f;
float syringeTargetV = 0.5f;
float spinnerTargetV = 0.0f;

float axisIntervalCounter = 0.0f;
float syringeIntervalCounter = 0.0f;

float axisStepInterval    = 1.0f / (axisSPM * axisTargetV);
float syringeStepInterval = 1.0f / (syringeSPM * syringeTargetV);

bool axisHasStepped = false;
bool syringeHasStepped = false;

bool axisDirection = true;

float dT;
bool calcDT = false;
long currentTime;

void setup() {
  pinModeFast(AXISENA, OUTPUT);
  pinModeFast(AXISSTP, OUTPUT);
  pinModeFast(AXISDIR, OUTPUT);
  pinModeFast(SYRINGEENA, OUTPUT);
  pinModeFast(SYRINGESTP, OUTPUT);
  pinModeFast(SYRINGEDIR, OUTPUT);

  Serial.begin(115200);

  spinnerControl.attach(SPINNERPWM);

  SetSpinnerVelocity(1000);

  digitalWriteFast(AXISENA, LOW);
  digitalWriteFast(SYRINGEENA, LOW);
  digitalWriteFast(AXISDIR, HIGH);
}

void loop() {
  if(calcDT == false){
    currentTime = micros();
  }
  
  int aP = axisPosition / (int)axisSPM;
  int sP = syringePosition / (int)syringeSPM;

  //axis is out of range, flip direction, maintain velocity
  if (aP < 0){
    digitalWriteFast(AXISDIR, HIGH);
    axisDirection = true;
  }
  else if(aP > 20){
    digitalWriteFast(AXISDIR, LOW);
    axisDirection = false;
  }

  //syringe is out of range, disable
  if(sP < 0 || aP > 80){
    spinnerTargetV = 0.0f;
  }
  
  Control();
  
  if (calcDT == false){
    dT = ((float)(micros() - currentTime)) / 1000000.0f;
    calcDT = true;
  }
}

void Control(){
  if (axisIntervalCounter < axisStepInterval){
    axisIntervalCounter += dT;
  }
  else{
    axisIntervalCounter = 0;
    StepAxis();
    
    if(axisDirection){
      axisPosition += 1;
    }
    else{
      axisPosition -= 1;
    }
  }
  
  if (syringeIntervalCounter < syringeStepInterval){
    syringeIntervalCounter += dT;
  }
  else{
    syringeIntervalCounter = 0;
    StepSyringe();
    syringePosition += 1;
  }
}

void StepAxis(){
  digitalWriteFast(AXISSTP, HIGH);
  delayMicroseconds(2);
  digitalWriteFast(AXISSTP, LOW);
}

void StepSyringe(){
  digitalWriteFast(SYRINGESTP, HIGH);
  delayMicroseconds(2);
  digitalWriteFast(SYRINGESTP, LOW);
}

void StepAxisOn(){
  digitalWriteFast(AXISSTP, HIGH);
  axisHasStepped = true;
}

void StepAxisOff(){
  digitalWriteFast(AXISSTP, LOW);
  axisHasStepped = false;
}

void StepSyringeOn(){
  digitalWriteFast(SYRINGESTP, HIGH);
  syringeHasStepped = true;
}

void StepSyringeOff(){
  digitalWriteFast(SYRINGESTP, LOW);
  syringeHasStepped = false;
}

void SetSpinnerVelocity(float v){
  int velMap = map(v, 0, 10000, 1000, 2000);
  spinnerControl.writeMicroseconds(velMap);
}
