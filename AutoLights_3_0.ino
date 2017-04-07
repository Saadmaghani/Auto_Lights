/*  Code taken from SpectrumProject_AutoLights_1_1
    Changes in Threshold:
     3/29/2015: added a cover and threshold 450->350
    Changes:
     switchLastReading = HIGH when switchReading == High and not when Light turns on
     
*/

int relayPin=3;
//int relayStatPin =8;
int pirPin = 7;
int ldrPin = 0;
int switchPin = 2;
int led13 = 13;

int calibrationTime = 30; 

int pirReading;
int ldrReading;
boolean lockLow = true; //boolean to indicate the system to 
boolean takeFirstLowTime;  //boolean to indicate the system to take the first time when motion is not detected
boolean relaystatus = false;
boolean switchReading;
boolean switchlastReading;

int threshold = 350;
long unsigned int pause = 10000;
long unsigned int firstLowIn; //to store the time when the first time no motion is detected 

#define NORM 10
#define SWITCHON 11
#define MOTIONHIGH 12
#define MOTIONLOW 13
int alState;

void setup(){  
  alState=NORM;
  Serial.begin(9600);
  pinMode(relayPin,OUTPUT);
  //pinMode(relayStatPin,OUTPUT);
  pinMode(led13,OUTPUT);
  pinMode(pirPin,INPUT);
  pinMode(switchPin,INPUT);
  takeFirstLowTime=false;
  Serial.print("Current Threshold: ");
  Serial.println(threshold);
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(i);
    delay(1000);
  }
  for (int i =0;i<5;i++){
    digitalWrite(led13,HIGH);
    delay(50);
    digitalWrite(led13,LOW);
    delay(50);
  }
}


void loop(){
  //Serial.println(analogRead(ldrPin));
//  Serial.println(alState);
 // Serial.println(digitalRead(switchPin));
  switch (alState) {
    case NORM:{
      switchReading = digitalRead(switchPin);
      pirReading=digitalRead(pirPin);
      ldrReading = analogRead(ldrPin);
      Serial.println(ldrReading);
      if(switchReading==HIGH){
        switchlastReading = HIGH;
        alState=SWITCHON;
      }
      else{
        if(switchlastReading !=NULL){
          if(switchlastReading== HIGH && switchReading==LOW){ //basically could be broken off to another state called switchoff
            switchlastReading = LOW;
            if((ldrReading>threshold)&&(relaystatus)){
              digitalWrite(relayPin,LOW);
              //digitalWrite(relayStatPin,LOW);
              relaystatus=false;
            }   
          }
        }
        else{
          switchlastReading = switchReading;
        }
        alState = (pirReading==HIGH)? MOTIONHIGH : MOTIONLOW;
      }
      break;}

    case SWITCHON:{
      switchReading = digitalRead(switchPin);
      if(!relaystatus){
        digitalWrite(relayPin,HIGH);
        //digitalWrite(relayStatPin,HIGH);
        Serial.println("RELAY ON");
        relaystatus=true;
      }
      if(switchReading==LOW) alState=NORM;
      break;}

    case MOTIONHIGH:{
       //    Serial.println("mov detected");
       if((ldrReading<threshold)&&(!relaystatus)){
        digitalWrite(relayPin,HIGH);
        //digitalWrite(relayStatPin,HIGH);
        relaystatus=true;
        Serial.println("light on");
      
      if(lockLow){
        lockLow=false;
      }
      if(!takeFirstLowTime){
        takeFirstLowTime=true;
      }
      alState = NORM;
      break;}

    case MOTIONLOW:{
      if(takeFirstLowTime){
        Serial.println("no movement detected for first time");
        firstLowIn = millis();          //save the time of the transition from high to LOW
        takeFirstLowTime = false;       //make sure this is only done at the start of a LOW phase
      }
      if(!lockLow && millis() - firstLowIn > pause){  
      //makes sure this block of code is only executed again after 
      //a new motion sequence has been detected
      lockLow = true;
      if(relaystatus){
        Serial.println("light off");
        digitalWrite(relayPin,LOW);
        //digitalWrite(relayStatPin,LOW);
        relaystatus=false;
      }
    }
    alState = NORM;
    break;}
  }
}
}
