#include "Arduino.h"
#include "TankManager.h"

long TankManager::microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}


bool TankManager::check_tank(const uint8_t trigger_and_echo_pins[]){
//float y = 0.0f;


unsigned long duration, depth_in_cm = 0;
   pinMode(trigger_and_echo_pins[0], OUTPUT);  pinMode(trigger_and_echo_pins[1], INPUT);

   digitalWrite(trigger_and_echo_pins[0], LOW);      delayMicroseconds(2);

   digitalWrite(trigger_and_echo_pins[0], HIGH);     delayMicroseconds(150);   digitalWrite(trigger_and_echo_pins[0], LOW);

    
   duration = pulseIn(trigger_and_echo_pins[1], HIGH);
   depth_in_cm = microsecondsToCentimeters(duration);

    
  // Serial.print(depth_in_cm);   Serial.print("cm");   Serial.println();
  //delay(150); // --- two busts 
 //   delay(30);

//.. ze diipa zo worse, the shallower, the better .. //

      if(depth_in_cm == 0){ /*TankMessage = "Too full";*/ TankMessage = "Out";     beyond_quarter = true; }
 else if(depth_in_cm > 0){    // --- unless proven otherwise
          
           if(depth_in_cm <=water_depth_range[1]) beyond_quarter = true;// ---over  90%
      else if(depth_in_cm > water_depth_range[1] && depth_in_cm <= water_depth_range[2])    beyond_quarter = true; // --- over 75%
      else if(depth_in_cm > water_depth_range[2] && depth_in_cm <= water_depth_range[3])    beyond_quarter = true;   // --- about 50%
      else if(depth_in_cm > water_depth_range[3] && depth_in_cm <= water_depth_range[4])    beyond_quarter = false;  // --- about 25%
      else  beyond_quarter = false; //TankMessage = "Empty";   // --- if close to zero
  //TankMessage += "%";

  if(depth_in_cm-int(top_dead_center+0.50) >= 0)   
        accurate_depth = 100.00 - (float(depth_in_cm)-top_dead_center)*100/float(max_depth);
   else accurate_depth = 100.50;

   tank_levo = int(accurate_depth+0.50);
 } 

    return beyond_quarter;
}
