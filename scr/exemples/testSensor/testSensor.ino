/**
 * This file is an example of using the library.
 * WSsensor Arduino library for Wheather Station sensor receiver.
 * 
 * Copyright (c) 2015 @cosmopaco. All right reserved.
 * 
**/
#include <WSsensor.h>

//Constructor
WSsensor mySensor = WSsensor();

void setup() {
  Serial.begin(115200);
  // Initialize
  // mySensor.enableReceive(2);   //(repeats paquet) Default interrupt 0. => that is pin #2
  mySensor.enableReceive(0 , 2);  //(Interrupt, repeats paquet) Find interrupt pins for Arduino
}
void loop (){
  static int c;
  // When a valid packet has been received return true.
  if(mySensor.avalaible()){
    
    Serial.print(c++);
    Serial.print(" Captures. Sensor whit ID ");
    // The identifier changes each time the batteries are changed.
    int id= mySensor.readId();
    Serial.print(id);
    Serial.print(",channel ");
    int ch= mySensor.readCh();
    Serial.println(ch);    
    // Read the temperature
    //  Returns the value in tenths of degrees
    int temp=mySensor.readTe();
    Serial.print("Temperature reading= ");
    Serial.println(temp);     
    //Convert temperatura to float.
    float tempF;
    tempF= (float)(temp%10)/10 + temp/10; 
    Serial.print("Temperature float= ");     
    Serial.print (tempF);
    Serial.println('C');
    // Read the last value of humidity received.
    int hum= mySensor.readHu();
    Serial.print("Humidity= ");
    Serial.print(hum);
    Serial.println('%');
    Serial.println(); 
      	  
  }
}