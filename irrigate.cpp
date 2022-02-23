#include "Arduino.h"
#include "irrigate.h"

/*

long compare_db_parameters(int planting_date, int crop_water_demand, int field_area, int valve_solenoid_discharge){
/*
    * Crop properties, 
    * Soil Properties
    * Field Properties
    * Tank Properties
    * Valve Properties
    * 
*

}

unsigned int irrigate::irrigate_now(const uint8_t * irrigation_valve,  unsigned long current_time_in_seconds){ 
  //unsigned int _irrigation_duration;
  

 //the second irrigating whould wait till water seepage thru soil is complete
if(was_irrigating) waiting_time = (5*60);  // 5 minutes
  
if(!valve_is_ON){ //teekako valuvu
      digitalWrite(*irrigation_valve, LOW);  valve_is_ON = true; 
        starting_time_in_seconds = waiting_time + current_time_in_seconds; 
        was_irrigating = true; 
        stopping_time_in_seconds = starting_time_in_seconds + actual_duration;
    
        moi_b4_irri = average_moisture;
        irri_hour = hour;
 }

else if(valve_is_ON){count_down_in_secs = stopping_time_in_seconds - current_time_in_seconds;
                       irrigationPrompt = "Now Open...";
  
if(count_down_in_secs == 0){ valve_is_ON = false; // --- take back to the caller
    digitalWrite(*irrigation_valve, HIGH);   
        irrigationPrompt = "Just Stopped!";
    latest_irri_vol = exec.tank_levo; //keep height for now. otherwise change to liters
    moi_after_irri = moischa(connected_soil_sensors, soil_moisture_signal);
        
    String IrrigationMessage = "Hi" + farmerName + ", I've just irrigated your " + farm_size +
                               " farm in" + farmLocation + "at 6:30am using 50l of water and increased soil water content from" + 
                               String(moi_b4_irri) + "to" + String(moi_after_irri);

  data.send_Message(IrrigationMessage);
  }

}  else count_down_in_secs = 0;  //|| count_down_in_secs >= sizeof(int)

 //else if(hours_of_irrigation <=18) { since 18hours max out the integer of 65,535

 //-- if the time at which we start irrigating is greator than the duration e.g. starting @ 6:58:45 till like 7:20:13 --- 

Serial.println("Irrigator Nzuuno!");

//trackONs++; 
//if(trackONs>=2){irrigationPrompt = "Not Started!";}
Serial.print("Irrigating? ");   Serial.println(irrigationPrompt);
 // (valve_is_ON==true)?"Yes, Now irrigating...":(was_irrigating?"Just stopped irrigating":"Not Yet irrigated-!"));


Serial.print("Time Now: "); Serial.println(current_time_in_seconds);
Serial.print("Starting Time: "); Serial.println(starting_time_in_seconds);
Serial.print("Data fed Duration: "); Serial.println(actual_duration);
//Serial.print("Duration: "); Serial.println(irrigation_duration);

Serial.print("Irrigation ending in: "); Serial.println(count_down_in_secs);
Serial.print("Irrigation ending at: "); Serial.println(stopping_time_in_seconds);


//return String(irrigation_duration); // present the time since first power on in seconds ... if less than 59 or countdown 10, 9, 8...
return count_down_in_secs;
}
*/