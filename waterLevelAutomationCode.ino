#include <NewPing.h>
#define tankLevel 300
#define higherLimitOfWaterLevel 60
#define lowerLimitOfWaterLevel 30
#define relayOn 10
#define trigPin 9
#define echoPin 8
#define moterStatus 5
#define highLevel 4
#define lowLevel 3
#define checkerLed 2
double waterLevelValue=0;
void setup() {
  pinMode(moterStatus, OUTPUT);
  pinMode(checkerLed, OUTPUT);
  pinMode(lowLevel, OUTPUT);
  pinMode(highLevel, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayOn, OUTPUT);
  Serial.begin(9600);
}
double returnWaterLevel(){
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  double  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  double distance = duration*0.034/2;
  //Serial.println(distance);  
  delay(100);
  return ((tankLevel-distance)/tankLevel)*100;
}
void loop() {
  digitalWrite(checkerLed, HIGH);
  delay(1000);
  Serial.println("START");
  waterLevelValue=returnWaterLevel();
  Serial.print("waterLevelValue  : ");
  Serial.println(waterLevelValue);
  delay(1000);
  if(waterLevelValue<lowerLimitOfWaterLevel){
   digitalWrite(lowLevel, HIGH);
   delay(1000);
   digitalWrite(moterStatus, HIGH);
   digitalWrite(relayOn,LOW);
   delay(1000);
   digitalWrite(lowLevel, LOW);
   delay(1000);
   Serial.println("Moter ON");
   delay(1000);
  }
  else if(waterLevelValue>higherLimitOfWaterLevel){
   digitalWrite(highLevel, HIGH);
   delay(1000);
   digitalWrite(moterStatus, LOW);
   digitalWrite(relayOn,HIGH);
   delay(1000);
   digitalWrite(highLevel, LOW);
   delay(1000);
   Serial.println("Moter OFF");
   delay(1000);
  }
  digitalWrite(checkerLed, LOW);
  delay(1000);
}
