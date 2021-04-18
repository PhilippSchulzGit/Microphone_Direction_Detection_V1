#include <Servo.h>

// analog inputs for all microphones
#define MICROPHONE1 A0
#define MICROPHONE2 A1
#define MICROPHONE3 A2
#define MICROPHONE4 A3
#define MICROPHONE5 A6
#define SCALEFACTOR 1000

// object of the servo class
Servo directionServo;

// amplitudes of all microphones
int amplitudes[] = {0,0,0,0,0};
// array containing normalized amplitudes
float normalized[] = {0,0,0,0,0};
// size of Amplitudes array
int amplitudeNumber = 5;
// maximum value and index of amplitudes
int max_A = 0;
int max_i = 0;

// method for normalizing all amplitudes to the greatest amplitude
void normalizeAmplitudes() {
  // 1. determine maximum amplitude
  getMaximumAmplitude();
  // 2. divide all other amplitudes by the found maximum amplitude
  for(int i=0;i<amplitudeNumber;i++) {
    normalized[i] = float(amplitudes[i]) * SCALEFACTOR / float(max_A);
  }
}

void getMaximumAmplitude() {
  max_A = 0;
  max_i = 0;
  for(int i=0;i<amplitudeNumber;i++) {
    if(abs(amplitudes[i])>max_A) {
      max_A = amplitudes[i];
      max_i = i;
    }
  }
}

void plotNormal() {
  Serial.print(amplitudes[0]);
  Serial.print(",");
  Serial.print(amplitudes[1]);
  Serial.print(",");
  Serial.print(amplitudes[2]);
  Serial.print(",");
  Serial.print(amplitudes[3]);
  Serial.print(",");
  Serial.println(amplitudes[4]);
}

void plotNormalized() {
  normalizeAmplitudes();
  Serial.print(normalized[0]);
  Serial.print(",");
  Serial.print(normalized[1]);
  Serial.print(",");
  Serial.print(normalized[2]);
  Serial.print(",");
  Serial.print(normalized[3]);
  Serial.print(",");
  Serial.println(normalized[4]);
}

void setup() {
  Serial.begin(9600);
  // set up analog inputs
  pinMode(MICROPHONE1,INPUT);
  pinMode(MICROPHONE2,INPUT);
  pinMode(MICROPHONE3,INPUT);
  pinMode(MICROPHONE4,INPUT);
  pinMode(MICROPHONE5,INPUT);
}

void loop() {
  amplitudes[0] = analogRead(MICROPHONE1);
  amplitudes[1] = analogRead(MICROPHONE2);
  amplitudes[2] = analogRead(MICROPHONE3);
  amplitudes[3] = analogRead(MICROPHONE4);
  amplitudes[4] = analogRead(MICROPHONE5);
  plotNormal();
  //plotNormalized();
}
