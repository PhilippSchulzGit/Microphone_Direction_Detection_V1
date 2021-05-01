#include <Servo.h>

// analog inputs for all microphones
#define MICROPHONE1 A0
#define MICROPHONE2 A1
#define MICROPHONE3 A2
#define MICROPHONE4 A3
#define MICROPHONE5 A6
#define SERVOPIN 5
#define SCALEFACTOR 1000

// object of the servo class
Servo directionServo;

// servo enable
bool servoEnable = 1;

// angle of the sound source
float angle = 0;

// ----------------1 Initialize arrays and variables-----------------
// amplitudes of all microphones
// NOTE: If these arrays are changed the number of elements given to calculateAverage() AND shiftDataArray() in section 5. must be changed!
int amplitudes1[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes2[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes3[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes4[] = {0,0,0,0,0,0,0,0,0,0};
int amplitudes5[] = {0,0,0,0,0,0,0,0,0,0};

float average1 = 0; // average over amplitude array 1
float average2 = 0; // average over amplitude array 2
float average3 = 0; // average over amplitude array 3
float average4 = 0; // average over amplitude array 4
float average5 = 0; // average over amplitude array 5
// maximum values of amplitude of all microphones, normalized for each average values
float maximum[] = {0,0,0,0,0};
// time over which the amplitudes of the microphones should be sampled
int samplingTime = 100; // sampling time in [ms]
// array of maxmimum index for determining direction from each cycle until samplingTime has been passed
int maximumIndexCounter = 0;
int maximumIndex[150];  // <----- this array size needs to be adjusted when the sampling time is changed
// rule of thumb for adjustment: samplingTime * 1.5 for safety, as one cycle takes about 1ms


// method to shift all values of an array 1 index right, meaning arr[len-1] (last value) will be overwritten
void shiftDataArray(int arr[],int len) {
  int temp = arr[len-1];
  for(int i=len-1;i>0;i--) {
    arr[i] = arr[i-1];
  }
  arr[0] = temp;
}

// method to find the highest value in a given array and return the index
int findMaximumIndex(float arr[],int arrSize) {
  int index = 0;
  float currentMax = 0;
  for(int i=0;i<arrSize;i++){
    if(arr[i]>currentMax){
      currentMax = arr[i];
      index = i;
    }
  }
  return index;
}

// method to calculate the average of a given array with given length from values greater than 0
float calculateAverage(int arr[],int arrLen){
  float count = 0;  // number of values greater than 0
  float sum = 0;    // sum of all numbers greater than 0
  for(int i=0;i<arrLen;i++){
    if(arr[i]>0){
      sum+=float(arr[i]);
      count++;
    }
  }
  return sum/count;
}

// method to determine the angle based on counters for each microphone
float calculateAngle(int counter1,int counter2, int counter3, int counter4, int counter5) {
  //------------------------------TODO: implement function--------------------
  float counts[] = {counter1,counter2,counter3,counter4,counter5};
  /**
  Serial.print(counts[0]);
  Serial.print(",");
  Serial.print(counts[1]);
  Serial.print(",");
  Serial.print(counts[2]);
  Serial.print(",");
  Serial.print(counts[3]);
  Serial.print(",");
  Serial.println(counts[4]);*/
  float finalAngle = 0;
  int maxIndex = findMaximumIndex(counts,5);
  finalAngle = maxIndex*45;
  return finalAngle;
}

// method to determine the angle of the sound source
void determineAngle() {
  // ----------------4.1 save current time stamp-----------------
  int beginTime = millis();
  maximumIndexCounter = 0;  // reset maximumIndexCounter
  do{
    // ---------------------4.2 read analog values-----------------------
    int microphone1 = analogRead(MICROPHONE1);
    int microphone2 = analogRead(MICROPHONE2);
    int microphone3 = analogRead(MICROPHONE3);
    int microphone4 = analogRead(MICROPHONE4);
    int microphone5 = analogRead(MICROPHONE5);
    // ----------4.3 calculate maximum deviation from average------------
    // if the average for each microphone is not assigned (first cycle of the program)
    if(average1==0 && average2==0 && average3==0 && average4==0 && average5==0) {
      average1 = microphone1;
      average2 = microphone2;
      average3 = microphone3;
      average4 = microphone4;
      average5 = microphone5;
    }
    maximum[0] = abs((microphone1-average1)/average1);
    maximum[1] = abs((microphone2-average2)/average2);
    maximum[2] = abs((microphone3-average3)/average3);
    maximum[3] = abs((microphone4-average4)/average4);
    maximum[4] = abs((microphone5-average5)/average5);
    // for debugging purposes
    Serial.print(maximum[0]);
    Serial.print(",");
    Serial.print(maximum[1]);
    Serial.print(",");
    Serial.print(maximum[2]);
    Serial.print(",");
    Serial.print(maximum[3]);
    Serial.print(",");
    Serial.println(maximum[4]);
    // ----------4.4 shift amplitude arrays and store new amplitude------
    // shift arrays
    shiftDataArray(amplitudes1,10);
    shiftDataArray(amplitudes2,10);
    shiftDataArray(amplitudes3,10);
    shiftDataArray(amplitudes4,10);
    shiftDataArray(amplitudes5,10);
    // store new amplitudes
    amplitudes1[0]=microphone1;
    amplitudes2[0]=microphone2;
    amplitudes3[0]=microphone3;
    amplitudes4[0]=microphone4;
    amplitudes5[0]=microphone5;
    // --------------------4.5 calculate new averages--------------------
    average1 = calculateAverage(amplitudes1,10);
    average2 = calculateAverage(amplitudes2,10);
    average3 = calculateAverage(amplitudes3,10);
    average4 = calculateAverage(amplitudes4,10);
    average5 = calculateAverage(amplitudes5,10);
    
    // ----------------4.6 determine highest deviation-------------------
    // determine current highest deviation and save microphone number to array
    maximumIndex[maximumIndexCounter]=findMaximumIndex(maximum,5);
    maximumIndexCounter++;  // increment maximumIndexCounter
    
    // --------------------4.7 time < samplingTime?----------------------
  }while((millis()-beginTime)<samplingTime);
  // reset maximum occurence counters used in angle determination
  int counter1=0;
  int counter2=0;
  int counter3=0;
  int counter4=0;
  int counter5=0;
  // loop over maxmimumIndex array and count occurences of each microphone
  for(int i=0;i<maximumIndexCounter;i++){
    int currentNumber = maximumIndex[i];
    if(currentNumber==0){       // microphone1
      counter1++;
    }else if(currentNumber==1){ // microphone2
      counter2++;
    }else if(currentNumber==2){ // microphone3
      counter3++;
    }else if(currentNumber==3){ // microphone4
      counter4++;
    }else if(currentNumber==4){ // microphone5
      counter5++;
    }
  }
  // -----------------------4.8 determine angle--------------------------
  angle = calculateAngle(counter1,counter2,counter3,counter4,counter5);
  //plotNormal();
}

void plotNormal() {
  Serial.print(amplitudes1[0]);
  Serial.print(",");
  Serial.print(amplitudes2[0]);
  Serial.print(",");
  Serial.print(amplitudes3[0]);
  Serial.print(",");
  Serial.print(amplitudes4[0]);
  Serial.print(",");
  Serial.print(amplitudes5[0]);
  Serial.print(",");
  Serial.println(angle);
}

void setup() {
  // -----------------------2 Initialize IO-pins---------------------
  Serial.begin(9600);
  // set up analog inputs
  pinMode(MICROPHONE1,INPUT);
  pinMode(MICROPHONE2,INPUT);
  pinMode(MICROPHONE3,INPUT);
  pinMode(MICROPHONE4,INPUT);
  pinMode(MICROPHONE5,INPUT);
  
  // ------------------------3 servoEnable?-------------------------
  if(servoEnable) {
    // --------------------3.1 Initialize Servo---------------------
    directionServo.attach(SERVOPIN);
    directionServo.write(90);
  }
}

void loop() {
  determineAngle();
  // do something with angle
  if(servoEnable) {
    directionServo.write(angle);
  }
  // 11. endCondition?
  // in this case the process repeats itself indefinetely, because it is just for tests
  // wait for delay
  delay(500);
}
