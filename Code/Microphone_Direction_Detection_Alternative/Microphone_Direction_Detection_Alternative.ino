/**
 *          Note: This program is an alternative to the program used in test 3
 *          Here, background noise is filtered out to prevent the servo from moving, even though no real inpuit was given
 */

#include <Servo.h>

// analog inputs for all microphones
#define MICROPHONE1 A0
#define MICROPHONE2 A1
#define MICROPHONE3 A2
#define MICROPHONE4 A3
#define MICROPHONE5 A6
#define SERVOPIN 5        // PWM pin of the servo
#define SAMPLINGTIME 100  // sampling time in [ms]
#define MICROPHONECOUNT 5 // number of microphones used

// object of the servo class
Servo directionServo;
// servo enable
bool servoEnable = 1;

// angle of the sound source
float servoAngle = 90;

// code for sampling 1 analog input taken from https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
// (25.12.2020,21:52)
// modified for usage in a function and using 5 analog inputs

// array containing newest analog values
int amplitudes[MICROPHONECOUNT];
// array containing minimum values for all analog inputs
int minimumAmplitudes[MICROPHONECOUNT];
// array containing maximum values for all analog inputs
int maximumAmplitudes[MICROPHONECOUNT];
// array containing all voltages from microphones
float voltages[MICROPHONECOUNT];

// method for handling the newest analog reading
// microphoneNumber: the index of the used microphone (for reading/writing from/to arrays)
void handleAnalogReading(int microphoneNumber){
  int amplitude = amplitudes[microphoneNumber]; // get current reading from the amplitudes array
  if(amplitude<1024) {  // only handle valid readings
    if(amplitude > maximumAmplitudes[microphoneNumber]) { // if a new highest number was recorded
      maximumAmplitudes[microphoneNumber] = amplitude;  // save maximum value to array
    }else if(amplitude < minimumAmplitudes[microphoneNumber]) { // if a new lowest number was recorded
      minimumAmplitudes[microphoneNumber] = amplitude;  // save minimum value to array
    }
  }
}

// method for handling the sampling process
void sampleAmplitudes(){
   // reset minimum and maximum arrays
   for(int i=0;i<MICROPHONECOUNT;i++){
      minimumAmplitudes[i] = 0;
      maximumAmplitudes[i] = 0;
   }
   unsigned long startTime= millis();  // save timestamp of beginning
   do{
    // read current analog values
    amplitudes[0] = analogRead(MICROPHONE1);
    amplitudes[1] = analogRead(MICROPHONE2);
    amplitudes[2] = analogRead(MICROPHONE3);
    amplitudes[3] = analogRead(MICROPHONE4);
    amplitudes[4] = analogRead(MICROPHONE5);
    // handle current analog values
    for(int i=0;i<MICROPHONECOUNT;i++){
      handleAnalogReading(i);
    }
   }while(millis() - startTime < SAMPLINGTIME);
   // calculate voltages for each microphone
    for(int i=0;i<MICROPHONECOUNT;i++){
      int peakToPeak = maximumAmplitudes[i] - minimumAmplitudes[i]; // calculate peak to peak difference of amplitudes
      voltages[i] = (peakToPeak * 5.0) / 1024; // convert to volts (analog input gives value between 0-1024 corresponding to 0-5 V
    }
}

// method for determining the angle of the sound source from peak to peak voltages
void determineServoAngle(){
  // 1. find maximum value from peak to peak voltages
  float maximum = 0;
  int maximumIndex = 0;
  for(int i=0;i<MICROPHONECOUNT;i++){
    if(voltages[i]>maximum){ // if a new maximum is found
      maximum = voltages[i];
      maximumIndex = i;
    }
  }
  // 2. determine if background noise or actual signal
  if(maximum>= 3.2){
    // 3. set angle according to found maximum (in 45° steps for now)
    servoAngle = 45 * maximumIndex;
  }
}

void setup() 
{
  Serial.begin(9600);
  // set up analog inputs
  pinMode(MICROPHONE1,INPUT);
  pinMode(MICROPHONE2,INPUT);
  pinMode(MICROPHONE3,INPUT);
  pinMode(MICROPHONE4,INPUT);
  pinMode(MICROPHONE5,INPUT);
  if(servoEnable) { // initialize servo if it should be active
    directionServo.attach(SERVOPIN);
    directionServo.write(servoAngle); // center servo
    delay(500);
    directionServo.detach();
  }
}

void loop() 
{
  // sample all microphones
  sampleAmplitudes();
  // print mean voltages of each microphone to Serial
  for(int i=0;i<MICROPHONECOUNT;i++){
    Serial.print(voltages[i]*100);
    Serial.print(",");
  }
  Serial.println("");
  determineServoAngle();
  if(servoEnable){  // turn servo if it is enabled
    directionServo.attach(SERVOPIN);
    directionServo.write(servoAngle);
    delay(300);
    directionServo.detach();
  }
  delay(100);  // wait 0.4 s
}
