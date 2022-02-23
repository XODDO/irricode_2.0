#ifndef TANKMANAGER_H
#define TANKMANAGER_H

class TankManager{
public:

bool beyond_quarter = false;
const int speed_of_sound = 340; // --- m/s

float top_dead_center = 16.85; float accurate_depth = 0.00;
const uint8_t water_depth_range[5] = {24, 48, 72, 96, 120}; // 24 first figure --- // --- actual distance from top to surface in cm --- 
String TankMessage = ".";

 long microsecondsToCentimeters(long microseconds);
 bool check_tank(const uint8_t trigger_and_echo_pins[]);

    unsigned int tank_levo = 0, max_depth = 120, radius = 55;

};

#endif
