#include "Arduino.h"
#include "PowerMAnager.h"

PowerMAnager::PowerMAnager(){}

/*
*   task 1 Read battery voltage 
*   task 2 Display updated battery voltage in % in status bar and in V on power 
*   task 3 Disconnect loads if 30% and beep when < 20% 
*   task 4 Read solar voltage and take appropriate action  
*/


bool PowerMAnager::ManagePower(uint8_t * battery_reader){

  high_power_loads[0] = 46; high_power_loads[1] = 47; high_power_loads[2] = 48; high_power_loads[3] = 49;
  how_many_loads = 4; 

  readBattery = 0; loaded_battery = 0.00;   

  readBattery   = analogRead(*battery_reader); loaded_battery = float(readBattery);
    
    if(readBattery == 0){ powerMessage[3] = "Plug in Battery!"; /*digitalWrite(power_indica, HIGH);*/} // to avoid isnan() --- just in case
    else{
        loaded_battery = 13.00 * (loaded_battery/267.00); //ceil(); floor();//rounds to int round(); 
    //  loaded_battery = map(loaded_battery, 0.0000, 232.0000, 0.0000, 12.7500);

//--- now to check battery status 10.5 -> 0% and 12.9 -> 100% map(loaded_battery, 10.4, 12.9, 0, 100);
    if(loaded_battery < 11.00){   //if below  10.5 disconnect loads, & display backlight except MCU of course
        
// --- first ensure no high power load is ON
   // for(int i=0; i<how_many_loads; i++){  digitalWrite(high_power_loads[i], LOW); } //show the Loads off Red LED

    // Possible ranges of battery standards:  under 6, 6 to 9, 9 to 11
           if(loaded_battery <= 5.99){powerMessage[3] = "No Battery!";}
        
     else  if(loaded_battery>=6.000 && loaded_battery <=9.000){ //unacceptable battery level
                 powerMessage[3] = "Wrong Battery!"; }
            
        else{ //battery present but critically low btn 9 to 11V
          Butt_Percent = map(loaded_battery, 9.000, 11.00, 0.000, 10.00);
           }
    }
   
  else {  // if atleast 11.00000001V still available on battery, its okay keep on...
         
               if(loaded_battery >= full){ Butt_Percent = (loaded_battery/12.90)*100; enuf_pawa = true;} // --- full battery
           
          else if(loaded_battery >= half && loaded_battery < full){ Butt_Percent  =  (50*(loaded_battery-half)/(full-half))+50;   enuf_pawa = true;} // --- linear interpolation
           
          else if(loaded_battery >= quart && loaded_battery < half){ Butt_Percent  = (25*(loaded_battery-quart)/(half-quart))+25;   enuf_pawa = false; }
           
          else if(loaded_battery > empty && loaded_battery < quart){ Butt_Percent  = (25*(loaded_battery-empty)/(quart-empty));  enuf_pawa = false;   } //empty

                  Butt_Percent += 0.50;  // ---  0.5 to not truncate
          
          
        } //close critically low battery
       Battery_Percentage = int(Butt_Percent);
    } // close readBattery != 0



return enuf_pawa;

}


PowerMAnager::~PowerMAnager(){}
