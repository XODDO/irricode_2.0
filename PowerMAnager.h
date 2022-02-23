#ifndef POWERMANAGER_H
#define POWERMANAHER_H

class PowerMAnager{
public:
    PowerMAnager();

bool ManagePower(uint8_t * battery_reader);
// first we read the battery voltage
 uint16_t readBattery;
 bool enuf_pawa = true;

uint8_t high_power_loads[4]; // = {47,49,51,53}; // these are the loads that can be disconnected in case of critical battery level limit exceeded fans etc

uint8_t how_many_loads;

int charging_LED = 5, charged_full = 4;

float battery_voltage = 99.99, panel_OC_voltage = 19.00, insolance = 5.00; //open circuit voltage
float loaded_battery, battery_OC_Voltage, charging_voltage;

 float Butt_Percent = 0.00;

 uint8_t Battery_Percentage = 0;
/*
bool can_charge = false, isCharging = false;
int Batt_Percent  = 0; float Butt_Percent = 0.00;
*/

double factor = 0.054468;

// ---> 18 - 21 days; to get a USB to RJ45 black in color

String powerMessage[5] = {"unset", "unset", "unset", "unset", "unset"};


//<? -- Battery SuperGlobal [Parameters]
          float full = 12.65;
          float half = 12.20;
          float quart= 11.30;
          float empty= 10.80;


        void powerManager();
        void solar();
        


~PowerMAnager();

};



#endif
