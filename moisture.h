#ifndef MOISTURE_H
#define MOISTURE_H

class moisture{
public:
 moisture();
~moisture();

int no_moisture_sensors;
unsigned int independent_reading[4];

String Average_Moisture;
String moisture_status[2];
float average_moisture = 0.00;
int active_sensors = 0; int connected_devices = 0;

bool plugged_1 = false; bool last_state1 = false;
bool plugged_2 = false; bool last_state2 = false;
bool plugged_3 = false; bool last_state3 = false;
bool plugged_4 = false; bool last_state4 = false;


bool plugged1 = false; bool laststate1 = false;
bool plugged2 = false; bool laststate2 = false;
bool plugged3 = false; bool laststate3 = false;
bool plugged4 = false; bool laststate4 = false;




float compute_moisture_level(const uint8_t connected_soil_sensors[]);
void individual_sensor_values(const uint8_t connected_soil_sensors[]); // return individual readings array[];
long compute_irrigation_duration(long * current_time);
long compare_db_parameters(int planting_date, int crop_water_demand, int field_area, int valve_solenoid_discharge);
//vector<int>PINS;

};
#endif