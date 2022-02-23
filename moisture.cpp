#include "Arduino.h"
#include "moisture.h"
//#include <vector>



moisture::moisture(){} //constructor empty

float moisture::compute_moisture_level(const uint8_t connected_soil_sensors[]){
    // --- The following is based on the assumption that we know them...
     //  m1 = analogRead(A4);  m2 = analogRead(A7);  m3 = analogRead(A6);  m4 = analogRead(A5);
 no_moisture_sensors = (sizeof(connected_soil_sensors))/(sizeof(connected_soil_sensors[0]));

int m[no_moisture_sensors];
bool plugged[no_moisture_sensors];
bool last_state[no_moisture_sensors];

for(int i=0; i<no_moisture_sensors; i++){
       m[i] = analogRead(connected_soil_sensors[i]);

  if(m[i] >= 5) moisture_status[0] = "!No Power on line : " + String(i);
  else{
    if(m[i] >= 1020) {
        independent_reading[i] = m[i]; 
        moisture_status[0] = String(i) + " isn't plugged in!";  
        plugged[i] = false; last_state[i]=false; }
    else {
      plugged[i] = true;
        if(plugged[i] && !last_state[i]){
            //tone(sound_fx, 2000, 50);
            m[i] = map(m[i], 1023, 100, 0, 100);
            average_moisture += float(m[i]);
            Average_Moisture = String(average_moisture) + "%";
            active_sensors++; connected_devices++;
            moisture_status[1] = Average_Moisture;
        } // else{connected_devices--; active_sensors--; tone(sound_fx, 1500, 50);} runtime flags set
     }
  }

}
 
 return average_moisture;

}
/*
void moisture::individual_sensor_values(const uint8_t connected_soil_sensors[]){ // for the screen that will need all the individual sensor readings...
  Serial.print("Sensor 1: "); Serial.println(analogRead(A2));  
  Serial.print("Sensor 2: "); Serial.println(analogRead(A3));
  Serial.print("Sensor 3: "); Serial.println(analogRead(A4));
  Serial.print("Sensor 4: "); Serial.println(analogRead(A5));

}
*/
long moisture::compute_irrigation_duration(long * current_time){

float flow_rate = 0.00, frequency = 0.00;
float duration = 0.00;
//duration = nozzle_discharge * discharge; 



return 0;
}

long moisture::compare_db_parameters(int planting_date, int crop_water_demand, int field_area, int valve_solenoid_discharge){
/*
    * Crop properties, 
    * Soil Properties
    * Field Properties
    * Tank Properties
    * Valve Properties
    * 
*/

}
 

moisture::~moisture(){} // --- destructor also empty