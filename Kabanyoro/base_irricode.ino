
/*   MANAGE BUTTON PRESSES & UPDATE SCREEN BASED ON BUTTON PRESSES OR STATUS CHANGES   */
  String Kit_ID = "MakRIF II IrriKit 001"; 
//String Kit_ID = "MakRIF II IrriKit 002", "MakRIF II IrriKit 003";

#include "U8glib.h"
#include <EEPROM.h>
#include "PowerMAnager.h"
#include "TankManager.h"
#include <SoftwareSerial.h>
#include "irrigate.h"
#include "DHT.h"
#define DHTTYPE DHT22   // DHT 11


const uint8_t DHTPIN_ =  A10; //inactive 

DHT sensor(DHTPIN_, DHTTYPE); //const uint8_t temp_sensor = 29;

#include <SPI.h>
#include <SD.h>
#define CS 53

File local_storage;
//String Message;

String datalogPrompt = "not yet saved";

bool save_icon = true, send_icon = true;

// --- GSM SMS and Internet 
bool message_sent = false;

bool sent = false;  bool already_sent = false;
bool save = false;
bool started = false;
unsigned int message_counter; // 65,000 messages should be the max:: 40 years but EEPROM should load not more than 9,999 [6yrs]

String message;


bool evening = false;
bool night = false;

irrigate bosco;


SoftwareSerial SIM(69, 68); // RX, TX  3,2 but RX, TXchange interrupts on MEGA::=>59

void updateSerial(){  delay(500);
  while (Serial.available()){SIM.write(Serial.read()); } //Forward what Serial received to Software Serial Port
  
  while (SIM.available())   { Serial.write(SIM.read());  }//Forward what Software Serial received to Serial Port
  //else {Serial.println("SIM not available!");}
}           


String numberDB[5] = {"AT+CMGS=\"+256759991993\"", "AT+CMGS=\"+256757330484\"" "AT+CMGS=\"+256755279145\"", "AT+CMGS=\"+256701853322\"", "AT+CMGS=\"+256751712004\""};

bool OFF = false, ON = true;

// --- MULTI-STAGE PARADIGM --- //

U8GLIB_ST7920_128X64_1X LCD(18, 16, 17);  

PowerMAnager run_the;
TankManager exec;
float moisture_now = 0.00;


const uint8_t tank_level_sensorPin[2] = {14, 15}; // --- Display.object.TankMessage[0]; 
const uint8_t tank_signal[3] = {10, 11, 12}; // --- to show different levels: empty, half and full
const uint8_t pumpPin = A6;
const uint8_t connected_soil_sensors[4] = {A1, A2, A3, A4};
const uint8_t soil_moisture_signal[4] = {26, 28, 31, 33};

const uint8_t valvePin = A7; //46, 48, 49;
const int indicator = 13;
uint8_t battery_pin = A5;

//uint8_t power = 47; // 49;
uint8_t ledPw = 45;
//bit-wise one; 
bool valve_is_ON = false; bool was_irrigating = false;  String irrigationPrompt = "Valve Not Opened";

bool pump_is_ON = false; String Pump_Prompt = "Not Connected!";
bool cancelled = false;

uint8_t water_percent = 0;

String Pumping;

// STRING OF STATUSES --/-- STATI / STATUSI ... all for the screen and Quick Action

bool power_status = false; // high if true and low otherwise ... nothing to run... throw some notification to the screen
bool tank_status = false; // if critiko -- beeeeep beeeep where is the dam pump ??????
bool plugged = true; // to check any sensors plugged in or out
bool is_on   = true; // to update status of water pump or valve

uint8_t latest_irri_vol = 0;

const uint8_t power = 49; //,  back = 43, set = 45;
//const uint8_t up = 37, down = 35, left = 41, Okay = 33, right = 22; 
const uint8_t up = 43, down = 39, left = 37, Okay = 35, right = 41, back = 22, set = 30;
const uint8_t back_light = 24;
const uint8_t dim = 20, full_beam = 21;
//const uint8_t dim = 26, full_beam = 24;
const uint8_t power_indica = 8, danger = 9;

uint8_t sound_fx = 25;
 
unsigned long x = 0, y = 0;
unsigned waiting_welcome = 0;

//screen navigation and highlighting selectaz
uint8_t selected_menu_option = 1;

  const byte SleepNow = 8;
  byte current_Screen = 0;
  bool backlit = false;
  bool button_pressed_ = false;
  bool now_state = false;
  uint8_t move_sideways = 1;
  bool sending = false;

int eventHandler(byte _btn);
void scan_buttons();
void updateScreen(byte * screen, byte * sec);

unsigned long dim_screen = 0, return_home = 0;

// TIME HANDLER //

// -- clock code
uint8_t pulsePin = 7;
unsigned int pulse;

uint8_t second, minute, hour, day=0, month=0, Y1=20, Y2=21; 
uint16_t year = 0;
String zero = "0";

uint32_t a = 1; String free_run;

class disp{
  private: int x;
 /* unsigned l = 2;
    byte alpha = 0;
    uint8_t  abc1 = 0;
    uint16_t abc2 = 0;
    uint32_t abc3 = 0;
    uint64_t abc4 = 0;
*/
    
  public:

void settings(){
  LCD.firstPage();
    do{
  LCD.setFont(u8g_font_9x18B); // header --- u8g_font_6x10
  LCD.setPrintPos(20, 12);   LCD.print("Settings "); LCD.drawDisc(98, 7, 4); LCD.drawDisc(99,8,4);
   // body
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(1, 25);   LCD.print("Personalize this System to ");  
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(1, 35);   LCD.print("match your Preferences");
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(0, 48);   LCD.print("Press Set or Skip to Cancel"); 
   
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Set");   
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Skip");
  
  LCD.drawHLine(0, 53, 25); LCD.drawHLine(0, 63, 25);      LCD.drawVLine(0, 53, 11); LCD.drawVLine(25, 53, 11);
  LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
    }while(LCD.nextPage());
    

  }

bool changed = false;

uint64_t blinker = 0; uint8_t shift = 1;
byte temp_hr = 0; byte temp_min = 0; 

void set_time(){
  LCD.firstPage();
    do{
  LCD.setFont(u8g_font_9x18B); // header --- u8g_font_6x10
  LCD.setPrintPos(10, 12);   LCD.print("Set the Time "); 
   // body
 // LCD.setPrintPos(1, 25);   LCD.print("HH : MM");  
     LCD.setFont(u8g_font_unifont);
     LCD.setPrintPos(40, 28);   LCD.print("HH"); LCD.print(" :  "); LCD.print("MM");
     LCD.setFont(u8g_font_9x18B);  
      LCD.setPrintPos(40,45);
      LCD.print(temp_hr>9?temp_hr:("0"+String(temp_hr))); LCD.print(" : "); LCD.print(temp_min>9?temp_min:("0"+String(temp_min))); 

         if(shift == 1) {LCD.drawFrame(38,32,22,15);}
    else if(shift == 2){LCD.drawFrame(82,32,22,15);}

      /*
     if(blinker >= 0 && blinker <=20){LCD.setPrintPos(40,45);
       LCD.print(temp_hr>9?temp_hr:("0"+String(temp_hr)));  
     }
     else if(blinker > 20 && blinker < 40){LCD.setPrintPos(40,45); LCD.print("_");}

     if(blinker == 40){blinker = 0;}
     
     LCD.setPrintPos(60,45); LCD.print(" : "); LCD.print(temp_min>9?temp_min:("0"+String(temp_min)));
   */
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Save");   
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");
  
  LCD.drawFrame(0,53,27,11);
  LCD.drawFrame(101,53, 26, 11); 
  }while(LCD.nextPage());

}



uint8_t shifta = 1;
byte temp_date = 1; byte temp_month = 1; uint16_t temp_year=2010; 

void set_date(){
  LCD.firstPage();
    do{
    LCD.setFont(u8g_font_9x18B); // header --- u8g_font_6x10
    LCD.setPrintPos(10, 12);   LCD.print("Set the Date "); 
   // body
     LCD.setFont(u8g_font_helvR08);
     LCD.setPrintPos(5, 28);   LCD.print("Date"); LCD.print(" / "); LCD.print("Month");  LCD.print(" / "); LCD.print("Year");
     LCD.setFont(u8g_font_7x13B);  
     LCD.setPrintPos(10,45);
     LCD.print(temp_date>9?temp_date:("0"+String(temp_date))); LCD.print(" / "); 
     LCD.print(temp_month>9?temp_month:("0"+String(temp_month))); LCD.print(" / "); 
     LCD.print(temp_year); 

         if(shifta == 1) LCD.drawFrame(6,32,21,15);
    else if(shifta == 2) LCD.drawFrame(42,32,20,15);
    else    LCD.drawFrame(76,32,35,15);
    
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Save");   
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(104, 62); LCD.print("Back");
  
  LCD.drawFrame(0,53,27,11);
  LCD.drawFrame(101,53, 26, 11); 
  }while(LCD.nextPage());

}

uint8_t selekta = 1;

void set_crop(){

  LCD.firstPage();
  do{
LCD.setFont(u8g_font_7x13B); // header --- u8g_font_6x10
      LCD.setPrintPos(5, 10);   LCD.print("Choose Your Crop");  
     // LCD.setFont(u8g_font_helvR08); LCD.print("["); LCD.print(included_crops);  LCD.print("]");
   // body
  
    if(selekta == 1){ 
      LCD.drawHLine(1, 12, 126); LCD.drawHLine(1, 25, 126);      LCD.drawVLine(0, 13, 12); LCD.drawVLine(127, 13, 12); //--outer border
      LCD.drawHLine(1, 13, 127); LCD.drawHLine(1, 24, 127);      LCD.drawVLine(1, 13, 12); LCD.drawVLine(126, 13, 12); //--inner border
      LCD.drawBox(2, 14, 125, 10);   
// --- highlit txt
  LCD.setColorIndex(0); LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);  LCD.print("Beans"); LCD.setColorIndex(1); 
   
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 36);   LCD.print("Maize");
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 49);   LCD.print("Flowers"); 
      }

     else if(selekta == 2){
      LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("Beans");
      LCD.drawHLine(1, 25, 126); LCD.drawHLine(1, 38, 126);      LCD.drawVLine(0, 26, 12); LCD.drawVLine(127, 26, 12); //--outer border
      LCD.drawHLine(1, 26, 127); LCD.drawHLine(1, 37, 127);      LCD.drawVLine(1, 26, 12); LCD.drawVLine(126, 26, 12); //--inner border
      LCD.drawBox(2, 27, 125, 10);   
// --- highlit txt
  LCD.setColorIndex(0); LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 36);  LCD.print("Maize"); LCD.setColorIndex(1); 
       
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 49);   LCD.print("Flowers"); 
     }

     else {
       LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("Beans");
       LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 36);  LCD.print("Maize");
       LCD.drawHLine(1, 38, 126); LCD.drawHLine(1, 51, 126);      LCD.drawVLine(0, 39, 12); LCD.drawVLine(127, 39, 12); //--outer border
       LCD.drawHLine(1, 39, 127); LCD.drawHLine(1, 50, 127);      LCD.drawVLine(1, 39, 12); LCD.drawVLine(126, 39, 12); //--inner border
       LCD.drawBox(2, 40, 125, 10);   
// --- highlit txt
  LCD.setColorIndex(0); LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 49);   LCD.print("Flowers");   LCD.setColorIndex(1); 
      
  }

   
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Select");   
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");
  
  LCD.drawFrame(0,53,36,11);
  LCD.drawFrame(101,53, 26, 11); 
  }while(LCD.nextPage());

  
}

uint8_t move_box = 1; String temp_crop; String temp_speed;
void irrigation_interval(){
      //  temp_crop = EEPROM.read(10);
if(selekta == 1) {temp_crop = "Beans";}
else if(selekta == 2){temp_crop = "Maize";}
else {temp_crop = "Flowers";}

  
  LCD.firstPage();
  do{
LCD.setFont(u8g_font_7x13B); // header --- u8g_font_6x10
      LCD.setPrintPos(2, 10);   LCD.print("Set Irrigating Speed"); 
 LCD.setFont(u8g_font_helvR08);   LCD.setPrintPos(15,22); LCD.print("( for "); LCD.print(temp_crop); LCD.print(")");
  LCD.setFont(u8g_font_tpss);
  
 

  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Select");   
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");

     if(move_box == 1) {temp_speed = "Slow"; LCD.drawRFrame(0,28,30,16, 2);  LCD.drawFrame(1,29,28,14);
                                LCD.setFont(u8g_font_helvB08); LCD.setPrintPos(4, 40); LCD.print("Slow"); 
                                LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(40, 40); 
                                LCD.print("Medium"); LCD.setPrintPos(97, 40); LCD.print("Fast");}
     
else if(move_box == 2) {temp_speed = "Medium"; LCD.drawRFrame(36,28,47,16, 2); LCD.drawFrame(37,29,45,14); //darker highlight
                                LCD.setFont(u8g_font_helvB08); LCD.setPrintPos(40, 40); LCD.print("Medium"); 
                                LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(2, 40); LCD.print("Slow");
                                LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(97, 40); LCD.print("Fast");}

else  {temp_speed = "Fast"; LCD.drawRFrame(93,28,30,16, 2); LCD.drawFrame(94,29,28,14); // bolder highlight
                                LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(40, 40); LCD.print("Medium"); 
                                LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(2, 40); LCD.print("Slow");
                                LCD.setFont(u8g_font_helvB08); LCD.setPrintPos(97, 40); LCD.print("Fast"); }

  
  LCD.drawFrame(0,53,36,11);
  LCD.drawFrame(101,53, 26, 11); 
  
  }while(LCD.nextPage());
}

void confirm_changes(){
LCD.firstPage();
  do{

LCD.drawRFrame(3,0,124,52, 3);
LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(12, 10);   LCD.print("Confirm Settings"); 

LCD.setFont(u8g_font_helvR08);   LCD.setPrintPos(5,20); LCD.print("Time is :  "); LCD.setFont(u8g_font_helvB08);   LCD.print(temp_hr); LCD.print(" : "); LCD.print(temp_min);
LCD.setFont(u8g_font_helvR08);   LCD.setPrintPos(5,30); LCD.print("Date is :  "); LCD.setFont(u8g_font_helvB08);   LCD.print(temp_date); LCD.print("/"); LCD.print(temp_month); LCD.print("/"); LCD.print(temp_year);                             
LCD.setFont(u8g_font_helvR08);   LCD.setPrintPos(5,40); LCD.print("Crop is :  ");  LCD.setFont(u8g_font_helvB08);   LCD.print(temp_crop);
LCD.setFont(u8g_font_helvR08);   LCD.setPrintPos(5,50); LCD.print("Speed is : ");  LCD.setFont(u8g_font_helvB08);   LCD.print(temp_speed);

LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Confirm");   
LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");

  LCD.drawFrame(0,53,45,11);
  LCD.drawFrame(101,53, 26, 11); 
      
  }while(LCD.nextPage());
  
}

uint8_t move_boxa = 1;
void save_changes(){
  LCD.firstPage();
  do{
LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(12, 10);   LCD.print("Confirm Settings"); 
LCD.setFont(u8g_font_helvR08);   

LCD.drawRFrame(3,0,124,52, 3);
LCD.setPrintPos(5,20); LCD.print("Time: ");  
LCD.setPrintPos(5,30); LCD.print("Date: "); 
LCD.setPrintPos(5,40); LCD.print("Crop: "); 
LCD.setPrintPos(5,50); LCD.print("Speed: "); 

LCD.setColorIndex(0);LCD.drawBox(10, 10, 110, 50);LCD.setColorIndex(1);
LCD.drawRFrame(10, 10, 110, 50, 4); 

LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(28, 30);   LCD.print("Save Settings"); 

if(move_boxa == 1){
LCD.setFont(u8g_font_lucasfont_alternate);   
LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(84, 49); LCD.print("Back");

  LCD.drawBox(18,40,36,12); LCD.setColorIndex(0); LCD.setPrintPos(22, 49); LCD.print("Save"); LCD.setColorIndex(1);
  LCD.drawFrame(76,40, 36, 12); 
}

      else {
        LCD.setFont(u8g_font_lucasfont_alternate);   
LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(84, 49); LCD.print("Save");

  LCD.drawBox(18,40,36,12); LCD.setColorIndex(0); LCD.setPrintPos(22, 49); LCD.print("Back"); LCD.setColorIndex(1);
  LCD.drawFrame(76,40, 36, 12); 
      }
  }while(LCD.nextPage());
  
  }

void configuring(){
  LCD.firstPage();
  do{
LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(12, 10);   LCD.print("Confirm Settings"); 
LCD.setFont(u8g_font_helvR08);   

LCD.drawRFrame(3,0,124,52, 3);
LCD.setPrintPos(5,20); LCD.print("Time: ");  
LCD.setPrintPos(5,30); LCD.print("Date: "); 
LCD.setPrintPos(5,40); LCD.print("Crop: "); 
LCD.setPrintPos(5,50); LCD.print("Speed: "); 

LCD.setColorIndex(0);LCD.drawBox(10, 10, 110, 50);LCD.setColorIndex(1);
LCD.drawRFrame(10, 10, 110, 50, 4); 
 
  LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(28, 30);   LCD.print("Configuring..."); 

  LCD.setFont(u8g_font_lucasfont_alternate); 

  LCD.drawBox(18,40,76,12); LCD.setColorIndex(0); LCD.setPrintPos(42, 49); LCD.print("Saved"); LCD.setColorIndex(1);
      
  }while(LCD.nextPage());
  
  
}

void welcome(){
  LCD.firstPage();
  do{
    LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(32, 16);   LCD.print("Welcome!"); 
    //LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(0,0); LCD.print("Welcome! U home Now");

   LCD.setFont(u8g_font_helvR08);  
   LCD.setPrintPos(5,36);     LCD.print(temp_hr); LCD.print(" : "); LCD.print(temp_min);
   LCD.setPrintPos(80,36);    LCD.print(temp_date); LCD.print("/"); LCD.print(temp_month); LCD.print("/"); LCD.print(temp_year);                             

  LCD.setPrintPos(1,50); LCD.print("Set to irrigate: "); LCD.setPrintPos(76,50);  LCD.print("at Speed"); LCD.setFont(u8g_font_helvB08);
  LCD.setPrintPos(5,60);  LCD.print(temp_crop);
  LCD.setPrintPos(80,60); LCD.print(temp_speed);

    
  }while(LCD.nextPage());

  if(waiting_welcome == 70){current_Screen = 9;}
  }


void sleep(byte hh_mm_ss[], byte dd_mm_yy[], unsigned int * MSG){ // --- Screen 8

      LCD.firstPage();
            do{
              LCD.setPrintPos(10, 10);  LCD.setFont(u8g_font_helvR08);  LCD.print("Tap Menu to Wake      "); 
              if(*MSG < 100 ){LCD.drawRFrame(108,0,20,15,3); LCD.drawRFrame(109,1,18,14,2);  LCD.drawTriangle(115,15,121,15,118,19);}
              else {LCD.drawRFrame(98,0,30,15,3); LCD.drawRFrame(99,1,39,14,2);  LCD.drawTriangle(115,15,121,15,118,19);} 
              LCD.setPrintPos(112,12); LCD.print(*MSG); 
              LCD.setFont(u8g_font_courB08); LCD.setPrintPos(110,12);
              // a triangle at the bottom side
              LCD.setPrintPos(28, 32); LCD.setFont(u8g_font_7x13B);  LCD.print("Sleeping...");  //LCD.setClipWindow(35, 11, 74, 27);
              LCD.drawHLine(9,34,6); LCD.drawHLine(6,25,3); LCD.drawHLine(16,25,3);
              LCD.drawCircle(12,28,10);  //--- x __ x sleepy face
              //LCD.setPrintPos(6,30); LCD.setFont(u8g_font_helvR08); LCD.print("x"); LCD.setPrintPos(16,30); LCD.print("x"); 

              LCD.setPrintPos(72, 52);  LCD.setFont(u8g_font_7x13B); //time
              LCD.print(hh_mm_ss[0]>9?hh_mm_ss[0]:("0"+String(hh_mm_ss[0]))); 
              LCD.print(":"); LCD.print(hh_mm_ss[1]>9?hh_mm_ss[1]:("0"+String(hh_mm_ss[1]))); 
              LCD.print(":"); LCD.print(hh_mm_ss[2]>9?hh_mm_ss[2]:("0"+String(hh_mm_ss[2]))); //tick[2] 
             
              LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(78, 62); LCD.print(dd_mm_yy[0]); LCD.print("/"); LCD.print(dd_mm_yy[1]); LCD.print("/20"); LCD.print(dd_mm_yy[2]);
              LCD.setPrintPos(2, 62);  LCD.setFont(u8g_font_helvB08);  LCD.print("Menu"); //shade the menu bar 
              LCD.drawFrame(0,53,32,11);

  }while(LCD.nextPage());
             //LARGE FORMAT CLOCK
            
      }  
    
uint8_t set_option = 1;

void Setting() {
  LCD.firstPage();
  do{   // header --- u8g_font_6x10
  LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(30, 10);   LCD.print("Settings"); LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(110, 10); LCD.print(set_option); 
     // body

     if(set_option < 4){

  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("1. Set Time"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 37);   LCD.print("2. Send Settings"); // Add SMS Receiver
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 51);   LCD.print("3. Send Data"); 
  //LCD.drawHLine(8, 10, 104); 
  //LCD.setFont(u8g_font_fub14); //LCD.print(171/0xa0);
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Select");   //LCD.setPrintPos(104, 63); LCD.print("Back");

  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
  LCD.drawFrame(0,53,36,11);  LCD.drawFrame(101,53, 26, 11);

    if(set_option == 1){
  
//highlighter...
LCD.drawHLine(1, 12, 126); LCD.drawHLine(1, 25, 126);      LCD.drawVLine(0, 13, 12); LCD.drawVLine(127, 13, 12); //--outer border
LCD.drawHLine(1, 13, 127); LCD.drawHLine(1, 24, 127);      LCD.drawVLine(1, 13, 12); LCD.drawVLine(126, 13, 12); //--inner border

  }
else if(set_option == 2){
  
//highlighter...
LCD.drawHLine(1, 26, 126); LCD.drawHLine(1, 39, 126);      LCD.drawVLine(0, 27, 12); LCD.drawVLine(127, 27, 12); //--outer border
LCD.drawHLine(1, 27, 127); LCD.drawHLine(1, 38, 127);      LCD.drawVLine(1, 27, 12); LCD.drawVLine(126, 27, 12); //--inner border

  }
 else {
  
//highlighter...
    LCD.drawHLine(1, 40, 126); LCD.drawHLine(1, 53, 126);      LCD.drawVLine(0, 41, 12); LCD.drawVLine(127, 41, 12); //--outer border
    LCD.drawHLine(1, 41, 127); LCD.drawHLine(1, 52, 127);      LCD.drawVLine(1, 41, 12); LCD.drawVLine(126, 41, 12); //--inner border

      }
if(sending)
  if(dim_screen <= 10){ 
    digitalWrite(soil_moisture_signal[0], 1); 
    digitalWrite(soil_moisture_signal[1], 1);
    digitalWrite(soil_moisture_signal[2], 1);
    digitalWrite(soil_moisture_signal[3], 1);
    notification_popup(18, 8, "IrriKit Data", "Compiling Data...", "View", "Quit");}
  else if(dim_screen > 10 && dim_screen <= 20){notification_popup(18, 8, "IrriKit Data", "Sending Data...", "View", "Stop");}
  else if(dim_screen > 20 && dim_screen <= 30) notification_popup(18, 8, "IrriKit Data", "Data Sent", "View", "Exit");
}
     else { /**/} // NO Option 4, 5, 6
     
  }while(LCD.nextPage());
  // ... select and cancel should be exactly of the same font & in the same position at all times ... 


}


uint8_t shift_right = 1;


void reset_time(){
  LCD.firstPage();
    do{
      

  LCD.setFont(u8g_font_9x18B); // header --- u8g_font_6x10
  LCD.setPrintPos(8, 12);   LCD.print("Reset the Time "); 
   // body
 // LCD.setPrintPos(1, 25);   LCD.print("HH : MM");  
     LCD.setFont(u8g_font_unifont);
     LCD.setPrintPos(40, 28);   LCD.print("HH"); LCD.print(" :  "); LCD.print("MM");
     LCD.setFont(u8g_font_9x18B);  
     LCD.setPrintPos(40,45);
     LCD.print(temp_hr>9?temp_hr:("0"+String(temp_hr))); LCD.print(" : "); LCD.print(temp_min>9?temp_min:("0"+String(temp_min))); 

    if(shift_right == 1) {LCD.drawFrame(38,32,22,15);}
    else if(shift_right == 2){LCD.drawFrame(82,32,22,15);}
  
      
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Save");   
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");
  
  LCD.drawFrame(0,53,27,11);
  LCD.drawFrame(101,53, 26, 11); 
  }while(LCD.nextPage());

}

void time_is_set(){
  LCD.firstPage();
  do{

LCD.setColorIndex(0);LCD.drawRBox(18, 8, 92, 48,4);LCD.setColorIndex(1);
LCD.drawRFrame(18, 8, 92, 48, 4); 

LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(27, 26);   LCD.print("Time is Set"); 

LCD.setFont(u8g_font_lucasfont_alternate);    

  
      
  if(move_sideways == 1){
   LCD.drawBox(24,40,34,12); LCD.setColorIndex(0); LCD.setPrintPos(28, 49); LCD.print("Save"); LCD.setColorIndex(1);
   LCD.setPrintPos(78, 49); LCD.print("Quit"); LCD.drawFrame(70, 40, 34, 12); 
  }
  else{
      LCD.drawBox(70,40,34,12); LCD.setColorIndex(0); LCD.setPrintPos(76, 49); LCD.print("Quit"); LCD.setColorIndex(1);
      LCD.setPrintPos(26, 49); LCD.print("Save"); LCD.drawFrame(22, 40, 34, 12);
    }

//}

  }while(LCD.nextPage());
  
}


void notification_popup(int x=18, int y=8, String title="Title", String body="Body", String btn1 = "", String btn2 = "", int button_count=1){
// x_pos otherwise default
// y_pos otherwise default
// Header
// Body
// Call to Action buttons

uint8_t side_shift = 1;

//the ka boxis
LCD.setColorIndex(0); LCD.drawRBox(x, y, 92, 48,4); LCD.setColorIndex(1); LCD.drawRFrame(x, y, 92, 48, 4); //the notification box

//Eddy Mutwe
LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(40, 20);   /*LCD.print(which);*/ LCD.print(title); //LCD.drawHLine(19,21,90);

//The Body
LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(22,30); LCD.print(body);

//The Mapeesa
if(button_count == 2){ }// One big center button
if(side_shift == 1){ } // when the selector is left
  LCD.drawRBox(24,38,34,12,2); LCD.setColorIndex(0); LCD.setPrintPos(28, 48); LCD.print(btn1); LCD.setColorIndex(1);
  LCD.setPrintPos(76, 48); LCD.print(btn2); LCD.drawRFrame(70, 38, 34, 12, 2); 
  LCD.setFont(u8g_font_lucasfont_alternate);//}
/*
  else{
      LCD.drawBox(70,40,34,12); LCD.setColorIndex(0); LCD.setPrintPos(76, 49); LCD.print(btn2); LCD.setColorIndex(1);
      LCD.setPrintPos(26, 49); LCD.print(btn1); LCD.drawFrame(22, 40, 34, 12);
    }  */
}

uint8_t scroll = 1;

void Home(byte hour_min_sec[], byte soil_Tank_field[], float Battery_Voltage, float Solar_intensity, float Batt_Partager, String PumpPrompt, String IrrigationPrompt, unsigned int * MSG, unsigned int * count_down){ // --- screen 9
//title bar -- 1.Network bars, 2.Saved Prompt icon, 3.
    
    //title bar
    uint8_t Battery_Percentage = (int)Batt_Partager;

    LCD.firstPage();
    do{ // -- the status bar --- time, daynight, battery...

   // CLOCK //

  LCD.setFont(u8g_font_04b_03b); LCD.setPrintPos(95, 6);   
  LCD.print((hour_min_sec[0]>=0 && hour_min_sec[0]<=9)?(zero+String(hour_min_sec[0])):hour_min_sec[0]); LCD.print(":");
  LCD.print((hour_min_sec[1]>=0 && hour_min_sec[1]<=9)?(zero+String(hour_min_sec[1])):hour_min_sec[1]); LCD.print(":");
  LCD.print((hour_min_sec[2]>=0 && hour_min_sec[2]<=9)?(zero+String(hour_min_sec[2])):hour_min_sec[2]);

// --- SAVED AND SENT ICONS --- //
  LCD.drawFrame(12,1,8,6); LCD.drawTriangle(12,1,18,1,16,4); //LCD.setPrintPos(37,6);  //LCD.print("SMS Box");
  
  String figurer;
  if(*MSG != 0){
         if(message_counter>99) figurer = "99+"; 
       else figurer = String(*MSG);
     LCD.setPrintPos(22,7);   LCD.print(figurer);
  }
  
  LCD.drawVLine(39,0,3);   //  LCD.drawVLine(31,0,6); 
  LCD.drawHLine(36,5,7); LCD.drawHLine(36,6,7); LCD.drawLine(37,2,39,4); LCD.drawLine(41,2,39,4); // --- LCD.drawLine(32,12,30,7); //LCD.setPrintPos(43,6);  //LCD.print("Arrow");
  

  //  NETWORK BARS   //
  LCD.drawVLine(50,4,2); LCD.drawVLine(51,4,2); 
  LCD.drawVLine(53,2,4); LCD.drawVLine(54,2,4); 
  LCD.drawVLine(56,0,6); LCD.drawVLine(57,0,6); 
  
  
// --- bool save_icon = true, send_icon = true;
// --- if(save_icon) {LCD.drawPixel()}  else {} // save failed!
// --- if(send_icon) {LCD.drawPixel();} else {} // power fail

  // TANK //
  //LCD.setPrintPos(0, 6); LCD.setFont(u8g_font_04b_03); LCD.print("Tank"); // --- LCD.print(soil_tank_field[1]);
  LCD.drawHLine(3,0,3);

  LCD.drawFrame(0,1,9,6);
       if(soil_Tank_field[1] >= 0 && soil_Tank_field[1] <= 10){LCD.drawFrame(0,1,9,6);}
       if(soil_Tank_field[1] > 10 && soil_Tank_field[1] < 25) LCD.drawBox(1,5,7,1);  // if quart 
  else if(soil_Tank_field[1] >= 25 && soil_Tank_field[1] < 50) LCD.drawBox(1,4,7,3); // if half
  else if(soil_Tank_field[1] >= 50 && soil_Tank_field[1] < 75) LCD.drawBox(1,3,7,4); // if 3-quarter
  else if(soil_Tank_field[1] > 75){LCD.drawBox(1,2,7,5);} // if full

  //LCD.setPrintPos(2, 14); LCD.print(soil_Tank_field[1]); 
    
   /*to be removed! LCD.setPrintPos(105, 20); LCD.print(dim_screen);*/
/*
    LCD.setPrintPos(2, 6); LCD.setFont(u8g_font_04b_03); LCD.print((hour_min_sec[0]>=6 && hour_min_sec[0] <= 18) ?"Day":"Night"); 
   
                    //the circle or the crescent??
                    //if(day)
    LCD.drawCircle(30,3,3); //the circle -50-
    if(hour_min_sec[0]>=6 && hour_min_sec[0] <= 18){ // show sun's fingers if its day and moon if its night (x,y)
      LCD.drawPixel(28,0); LCD.drawPixel(33,0);  //---top fingers
      LCD.drawPixel(35,3); LCD.drawPixel(36,3); LCD.drawPixel(24,3); LCD.drawPixel(25,3);  // --- middle fingers
      LCD.drawPixel(27,7); LCD.drawPixel(33,7); LCD.drawPixel(34,8); LCD.drawPixel(26,8);  //--bottom fingers
    }
   */  
    //battery configurations
       LCD.setFont(u8g_font_5x7);
       LCD.drawFrame(80, 1, 12, 5);  LCD.drawPixel(79,3); LCD.drawPixel(79,2); LCD.drawPixel(79,4); // box if full and frame if empty 
       if(Battery_Voltage >= 11.00) { 
                   if(Battery_Voltage > 12.75) LCD.drawBox(80, 1, 12, 5); // battery full
              else if(Battery_Voltage > 12.50 && Battery_Voltage <= 12.75) LCD.drawBox(82, 1, 10,5); // 75%
              else if(Battery_Voltage > 12.20 && Battery_Voltage <= 12.50) LCD.drawBox(86, 1, 6, 5); //50%
              else if(Battery_Voltage > 11.50 && Battery_Voltage <= 12.20) LCD.drawBox(89, 1, 3, 5); //25%
              else {LCD.drawBox(90,1,2,5); }//LCD.setPrintPos(85,6); LCD.print("!");} // super low
          
       LCD.setPrintPos(62, 6); LCD.print(Battery_Percentage); if(Battery_Percentage<100)LCD.print("%"); // battery percentage
                              
                                  }
      else {LCD.setPrintPos(62, 6); LCD.print("Low"); } //empty battery



//---title
    LCD.setFont(u8g_font_tpssb);  LCD.setPrintPos(36, 21);   LCD.print("irri-kit UI "); LCD.print(a);

// --- the MAIN SECTION
    LCD.drawFrame(10,22,105,30);LCD.drawFrame(12,24,101,26); // --- ebili muno bichyyuse every maybe 5seconds -> slide away --- 
    

    String  tank_levolo;
   String tanka = String(soil_Tank_field[1])+"%";
   String soyilo = String(soil_Tank_field[0])+"%";
   if(soil_Tank_field[1]==0) {tanka = "No Signal from Tank!";   tank_levolo = "";} //:String(soil_Tank_field[1]+"%");
   else {tank_levolo = "Tank Level: ";}
   if(soil_Tank_field[0]==0) {soyilo = "Sensor Out!"; }//:String(soil_Tank_field[0]+"V");
    
    if(scroll == 1){  
    LCD.drawVLine(110,25,10); LCD.drawVLine(111,25,10); // --- scroll bar
    LCD.setFont(u8g_font_helvR08);   
    LCD.setPrintPos(15, 34);   LCD.print("Soil Moisture: "); LCD.setFont(u8g_font_timR08); LCD.print(soyilo); 
    LCD.setFont(u8g_font_helvR08); 
    LCD.setPrintPos(15, 46);   LCD.print("Valve: ");  LCD.setFont(u8g_font_timR08); LCD.print(IrrigationPrompt); if(*count_down>0){LCD.print(*count_down);}
       
    LCD.drawTriangle(60,58, 70,58, 64,63); // --- scroll down
  }
    
  else if(scroll == 2){//then display the 2 already pre-customized prompts 
       LCD.drawVLine(110,32,10); LCD.drawVLine(111,32,10); // --- scroll bar
       LCD.setFont(u8g_font_helvR08);  // // u8g_font_helvR08
       LCD.setPrintPos(15, 34);   LCD.print("Pump: ");  LCD.setFont(u8g_font_timR08);  LCD.print(PumpPrompt); 
       LCD.setFont(u8g_font_helvR08); 
       LCD.setPrintPos(15, 46);   LCD.print(tank_levolo); LCD.setFont(u8g_font_timR08);LCD.print(tanka); 
    LCD.drawTriangle(60,58, 70,58, 64,63); // --- scroll down
   }

   else if(scroll == 3){ // soil_Tank_field[2]==0?"unplugged":soil_Tank_field[2]; // then disp batt and solar status
    LCD.drawVLine(110,39,10); LCD.drawVLine(111,39,10); // --- scroll bar
    LCD.setFont(u8g_font_helvR08);   String tempu = (soil_Tank_field[2])==0?"Not in!":String(soil_Tank_field[2])+"C";
    LCD.setPrintPos(14, 34);   LCD.print("Battery Volt: ");  LCD.setFont(u8g_font_timR08);LCD.print(Battery_Voltage); LCD.print("V");// battery voltage 
    LCD.setFont(u8g_font_helvR08);
    LCD.setPrintPos(14, 46);   LCD.print("Field Temp: ");  LCD.setFont(u8g_font_timR08); LCD.print(tempu);  //
    LCD.drawTriangle(65,58, 60,63, 69,63); // --- scroll up
  }

    

    LCD.setFont(u8g_font_timR08);  LCD.setPrintPos(92, 52); ////very nice font: u8g_font_fixed_v0
//    Y2 = year%100;
//    LCD.print(day); LCD.print("/");LCD.print(month<10?(zero+String(month)):month); LCD.print("/"); LCD.print(Y2); //print full year on Sleep Screen or on setting time LCD.print(year);

// --- THE TASK BAR
//LCD.setContrast(10);
  LCD.setFont(u8g_font_lucasfont_alternate);  //font for the nav bar
  LCD.drawBox(0,55,30,9);
  LCD.setPrintPos(2, 63);  LCD.setColorIndex(0); LCD.print("Menu");   //LCD.setPrintPos(104, 63); LCD.print("Back");

LCD.setColorIndex(1);
LCD.drawBox(98,55,30,10);
LCD.setPrintPos(100, 63); LCD.setColorIndex(0); LCD.print("Sleep");
  
  // ... select and cancel should be exactly of the same font & in the same position at all times ... 
//LCD.setContrast(255);
LCD.setColorIndex(1);
//if(waiting_welcome == 100){current_Screen = 9;} //if idle for a minute, switch of disp and then to sleep screen

//if(sent){if(flash_sent <= 50){sent_notification(); flash_sent = 0;}}


    if(*count_down>0){
      String title = "VALVE";
LCD.setColorIndex(0);
LCD.drawRBox(18, 8, 92, 48,4);LCD.setColorIndex(1);LCD.drawRFrame(18, 8, 92, 48, 4); //the notification box

LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(30, 26);   /*LCD.print(which);*/ LCD.print(IrrigationPrompt); 
LCD.setPrintPos(20,32); LCD.print(*count_down);
LCD.drawRBox(24,36,34,12,2); LCD.setColorIndex(0); LCD.setPrintPos(32, 46); LCD.print("OK"); LCD.setColorIndex(1);
LCD.setPrintPos(72, 46); LCD.print("STOP"); LCD.drawRFrame(70, 36, 34, 12, 2); 
LCD.setFont(u8g_font_lucasfont_alternate);
    }

  /*  if(pump_is_ON){
      String title = "PUMP";
      LCD.setColorIndex(0);
LCD.drawRBox(18, 8, 92, 48,4);LCD.setColorIndex(1);LCD.drawRFrame(18, 8, 92, 48, 4); //the notification box
LCD.setPrintPos(20, 15); LCD.setFont(u8g_font_04b_03b); LCD.print(title); 
LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(30, 24);   LCD.print(Pump_Prompt); 
LCD.setFont(u8g_font_04b_03b);
LCD.setPrintPos(20,32); LCD.print(tanka);
LCD.setFont(u8g_font_helvB08);
LCD.drawRBox(24,36,34,12,2); LCD.setColorIndex(0); LCD.setPrintPos(32, 46); LCD.print("OK"); LCD.setColorIndex(1);
LCD.setPrintPos(72, 46); LCD.print("STOP"); LCD.drawRFrame(70, 36, 34, 12, 2); 
LCD.setFont(u8g_font_lucasfont_alternate);

    }*/

    //if message sent


    
    }while(LCD.nextPage());
}


uint8_t dj_selekta = 1;

  void menu(){ // --- Screen 2 

  
 LCD.firstPage();
 do{
  LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(40, 10);   LCD.print("Menu"); 
  LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(122,10); LCD.print(dj_selekta);  // header --- u8g_font_6x10
   
  //LCD.setContrast(10);
 LCD.setFont(u8g_font_lucasfont_alternate);  //font for the nav bar
  //LCD.setPrintPos(104, 63); LCD.print("Back");


LCD.drawBox(102,55,26,10);
LCD.setPrintPos(104, 63); LCD.setColorIndex(0); LCD.print("Back");
  
  // ... select and cancel should be exactly of the same font & in the same position at all times ... 
//LCD.setContrast(255);
  LCD.setColorIndex(1);
//if(waiting_welcome == 100){current_Screen = 9;} //if idle for a minute, switch of disp and then to sleep screen

/*
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(2, 62);   LCD.print("Select"); // footer   
  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
   
  LCD.drawFrame(0, 53, 36, 11);
  LCD.drawFrame(101, 53, 26, 11);
*/
   // body

if(dj_selekta < 4){//flash first screen
  
  
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("1   All Sensors"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 36);   LCD.print("2   Overhead Tank"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 49);   LCD.print("3   Farm & Soil"); 
  
  LCD.drawTriangle(60,58, 70,58, 64,63); // --- scroll down
  LCD.setFont(u8g_font_lucasfont_alternate);  LCD.drawBox(0,55,40,9);
  LCD.setPrintPos(2, 63);  LCD.setColorIndex(0); LCD.print("Select");  LCD.setColorIndex(1);
  
if(dj_selekta==1){
  LCD.drawVLine(126,14,11); LCD.drawVLine(127,14,11); // -- scroll bar
//highlighter...
LCD.drawHLine(3, 24, 5); LCD.drawHLine(3,25,5); // --underLine
LCD.drawHLine(13, 12, 94); LCD.drawHLine(12, 25, 95);      LCD.drawVLine(11, 13, 12); LCD.drawVLine(107, 13, 12); //--outer border
LCD.drawHLine(12, 13, 96); LCD.drawHLine(12, 24, 96);      LCD.drawVLine(12, 13, 12); LCD.drawVLine(106, 13, 12); //--inner border
  }
  
else if(dj_selekta == 2){
   LCD.drawVLine(126,27,11); LCD.drawVLine(127,27,11); // -- scroll bar
//highlighter...
LCD.drawHLine(3, 37, 5); LCD.drawHLine(3,38,5); // --underLine
LCD.drawHLine(13, 25, 94); LCD.drawHLine(12, 38, 95);      LCD.drawVLine(11, 26, 12); LCD.drawVLine(107, 26, 12); //--outer border
LCD.drawHLine(12, 26, 96); LCD.drawHLine(12, 37, 96);      LCD.drawVLine(12, 26, 12); LCD.drawVLine(106, 26, 12); //--inner border


  }
  
 else if(dj_selekta == 3){
  LCD.drawVLine(126,41,11); LCD.drawVLine(127,41,11);
  //highlighter...
  LCD.drawHLine(3, 50, 5); LCD.drawHLine(3,51,5); // --underLine
  LCD.drawHLine(13, 38, 94); LCD.drawHLine(12, 51, 95);      LCD.drawVLine(11, 39, 12); LCD.drawVLine(107, 39, 12); //--outer border
  LCD.drawHLine(12, 39, 96); LCD.drawHLine(12, 50, 96);      LCD.drawVLine(12, 39, 12); LCD.drawVLine(106, 39, 12); //--inner border
  }

}
else { // if the second scrolled screen is in view port

  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("4  Irrigation Valve"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 37);   LCD.print("5  Submersible Pump"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 51);   LCD.print("6  Settings"); 
  LCD.drawTriangle(65,58, 60,63, 69,63); // --- scroll up

/*
//highlighter...
LCD.drawHLine(1, 40, 126); LCD.drawHLine(1, 53, 126);      LCD.drawVLine(0, 41, 12); LCD.drawVLine(127, 41, 12); //--outer border
LCD.drawHLine(1, 41, 127); LCD.drawHLine(1, 52, 127);      LCD.drawVLine(1, 41, 12); LCD.drawVLine(126, 41, 12); //--inner border
*/

   if(dj_selekta == 4){
    String solenoid = "";
    solenoid = valve_is_ON?"Switch OFF":"Switch ON";
    
    LCD.drawVLine(126,14,11); LCD.drawVLine(127,14,11); //-- scroll bar
    LCD.drawHLine(3, 24, 5); LCD.drawHLine(3,25,5); // --underLine
    LCD.drawHLine(12, 12, 95); LCD.drawHLine(11, 25, 96);      LCD.drawVLine(10, 13, 12); LCD.drawVLine(107, 13, 12); //--outer border
    LCD.drawHLine(11, 13, 97); LCD.drawHLine(11, 24, 97);      LCD.drawVLine(11, 13, 12); LCD.drawVLine(106, 13, 12); //--inner border
LCD.setFont(u8g_font_lucasfont_alternate);  LCD.drawBox(0,55,56,9); 
LCD.setPrintPos(2, 63);  LCD.setColorIndex(0); LCD.print(solenoid);  LCD.setColorIndex(1);

    //LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(100, 23);   LCD.print("24C"); 
// --- highlighter...

  }
  else if(dj_selekta == 5){
    String submersible = "";
submersible = pump_is_ON?"Switch OFF":"Switch ON";

    LCD.drawVLine(126,28,11); LCD.drawVLine(127,28,11);
//highlighter...
LCD.drawHLine(3, 38, 5); LCD.drawHLine(3,39,5); // --underLine
LCD.drawHLine(12, 26, 95); LCD.drawHLine(11, 39, 96);      LCD.drawVLine(10, 27, 12); LCD.drawVLine(107, 27, 12); //--outer border
LCD.drawHLine(11, 27, 97); LCD.drawHLine(11, 38, 97);      LCD.drawVLine(11, 27, 12); LCD.drawVLine(106, 27, 12); //--inner border
LCD.setFont(u8g_font_lucasfont_alternate);  LCD.drawBox(0,55,56,9); 
LCD.setPrintPos(2, 63);  LCD.setColorIndex(0); 
LCD.print(submersible);  LCD.setColorIndex(1);

  }

  else if(dj_selekta == 6){
     LCD.drawVLine(126,42,11); LCD.drawVLine(127,42,11);
//highlighter...
LCD.drawHLine(3, 52, 5); LCD.drawHLine(3,53,5); // --underLine
  LCD.drawHLine(12, 40, 95); LCD.drawHLine(11, 53, 96);      LCD.drawVLine(10, 41, 12); LCD.drawVLine(107, 41, 12); //--outer border
  LCD.drawHLine(11, 41, 97); LCD.drawHLine(11, 52, 97);      LCD.drawVLine(11, 41, 12); LCD.drawVLine(106, 41, 12); //--inner border

LCD.setFont(u8g_font_lucasfont_alternate);  LCD.drawBox(0,55,40,9);
LCD.setPrintPos(2, 63);  LCD.setColorIndex(0); LCD.print("Select");  LCD.setColorIndex(1);
  
   }
 }

  
  
}while(LCD.nextPage());

   
 
 }
 

void Sensors(uint8_t * all_the_sensors){
  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_9x18B); // header --- u8g_font_6x10
  LCD.setPrintPos(10, 10);   LCD.print("Sensors : ");  LCD.print(*all_the_sensors);
  LCD.setPrintPos(122, 10); LCD.setFont(u8g_font_helvR08); LCD.print(selected_menu_option);
   // body
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("1. Soil Moisture Sensors"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 37);   LCD.print("2. Environmental Sensor");  
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 51);   LCD.print("3. Tank Level Sensor"); 
  //LCD.drawHLine(8, 10, 104); 
  //LCD.setFont(u8g_font_fub14); //LCD.print(171/0xa0);
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Select");   //LCD.setPrintPos(104, 63); LCD.print("Back");

  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
  LCD.drawFrame(0,53,36,11);  LCD.drawFrame(101,53, 27, 11);

if(selected_menu_option==1){
    LCD.drawTriangle(60,58, 70,58, 64,63); // --- scroll down

//highlighter...
LCD.drawHLine(1, 12, 126); LCD.drawHLine(1, 25, 126);      LCD.drawVLine(0, 13, 12); LCD.drawVLine(127, 13, 12); //--outer border
LCD.drawHLine(1, 13, 127); LCD.drawHLine(1, 24, 127);      LCD.drawVLine(1, 13, 12); LCD.drawVLine(126, 13, 12); //--inner border

  }
else if(selected_menu_option == 2){
  
//highlighter...
LCD.drawHLine(1, 26, 126); LCD.drawHLine(1, 39, 126);      LCD.drawVLine(0, 27, 12); LCD.drawVLine(127, 27, 12); //--outer border
LCD.drawHLine(1, 27, 127); LCD.drawHLine(1, 38, 127);      LCD.drawVLine(1, 27, 12); LCD.drawVLine(126, 27, 12); //--inner border

  }
 else {
  
//highlighter...
LCD.drawHLine(1, 40, 126); LCD.drawHLine(1, 53, 126);      LCD.drawVLine(0, 41, 12); LCD.drawVLine(127, 41, 12); //--outer border
LCD.drawHLine(1, 41, 127); LCD.drawHLine(1, 52, 127);      LCD.drawVLine(1, 41, 12); LCD.drawVLine(126, 41, 12); //--inner border
LCD.drawTriangle(65,58, 60,63, 69,63); 
  }
  }while(LCD.nextPage());
  // ... select and cancel should be exactly of the same font & in the same position at all times ... 
}


void Moisture_Sensors(uint8_t *howMany, String  readings[]) { // --- moisture sensor screen

  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_tpssb); // header u8g_font_8x13B u8g_font_courB10
  LCD.setPrintPos(12, 10);   LCD.print("Soil Moisture ("); LCD.print(*howMany); LCD.print(")"); 
  LCD.setFont(u8g_font_tpss);// body
  LCD.setFont(u8g_font_tpss); LCD.setPrintPos(2, 23);   LCD.print("Ridge 1: ");  LCD.setFont(u8g_font_tpssb);  LCD.print(readings[0]);   
  LCD.setFont(u8g_font_tpss); LCD.setPrintPos(2, 36);   LCD.print("Ridge 2: ");  LCD.setFont(u8g_font_tpssb);  LCD.print(readings[1]);   
  LCD.setFont(u8g_font_tpss); LCD.setPrintPos(2, 49);   LCD.print("Ridge 3: ");  LCD.setFont(u8g_font_tpssb);  LCD.print(readings[2]);   
  LCD.setFont(u8g_font_tpss); LCD.setPrintPos(2, 62);   LCD.print("Ridge 4: ");  LCD.setFont(u8g_font_tpssb);  LCD.print(readings[3]);   

  //LCD.drawHLine(106, 13, 5); LCD.drawHLine(106, 53, 5);  LCD.drawVLine(108, 13, 40);

  LCD.setFont(u8g_font_lucasfont_alternate);  LCD.setFont(u8g_font_tpss);LCD.setPrintPos(1, 63);   //LCD.print("Scan Sensor");  
  LCD.setPrintPos(120, 62); LCD.setFont(u8g_font_tpssb); LCD.print(">"); // draw triangle
  //LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
  }while(LCD.nextPage());
}


void Temp_n_Humidity(float * temperature, float *humidity) { // --- humidity sensors screen
  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_tpssb); // header
  LCD.setPrintPos(20, 10);   LCD.print("Field Conditions"); // LCD.print(*howMany); LCD.print(")");
  LCD.setFont(u8g_font_tpss); // body
  LCD.setFont(u8g_font_tpss);LCD.setPrintPos(10, 28);   LCD.print("Field Temp : ");    LCD.setFont(u8g_font_tpssb);LCD.print(*temperature); LCD.print("'C");
  LCD.setFont(u8g_font_tpss);LCD.setPrintPos(10, 49);   LCD.print("Field Humidity: "); LCD.setFont(u8g_font_tpssb);LCD.print(*humidity); LCD.print("%");
    LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
    LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
  }while(LCD.nextPage());

}


void Tank_Level(uint8_t * reading, uint8_t * irri_vol){ // --- humidity sensors screen
  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_7x13B); //header
  LCD.setPrintPos(7, 10);   LCD.print("Water Usage Stats");
  LCD.setFont(u8g_font_tpss);  //body
  LCD.setPrintPos(1, 25);   LCD.print("Tank Capacity: 1000l");
  LCD.setPrintPos(1, 38);   LCD.print("Past Irrigation Vol: ");    LCD.print(*irri_vol); 
  LCD.setPrintPos(1, 51);   LCD.print("Water Available Now: ");    LCD.print(*reading); LCD.print("l");

   LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
   LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
/*
if(reading < 100) {}
else if(reading >= 100 && reading <= 200){}
else if(reading >= 200 && reading <= 300){}
*/
  }while(LCD.nextPage());
}


void Farm(String nimilo[]) { // --- Kaawa com screen
  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_tpssb); //header
  LCD.setPrintPos(18, 10);   LCD.print("Farm Details"); // LCD.print(active_t_sensors); LCD.print(")");
  LCD.setFont(u8g_font_helvR08);  //body
  LCD.setPrintPos(1, 23);   LCD.print("Farm Location:"); LCD.setFont(u8g_font_4x6); LCD.print(nimilo[0]); LCD.setFont(u8g_font_helvR08);   LCD.print("0/hr");  
  LCD.setPrintPos(1, 36);   LCD.print("Farm Area: ");   LCD.print(nimilo[1]);  
  LCD.setPrintPos(1, 49);   LCD.print("Farm Owner: ");  LCD.print(nimilo[2]);  
  LCD.setPrintPos(1, 61);   LCD.print("Farm AEZ: ");    LCD.print(nimilo[3]);  

  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
  LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
  }while(LCD.nextPage());

}

uint8_t kong = 1;
void DataLog(void) { // --- READ direct from SD card and/or EEPROM
  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_tpssb); //header
  LCD.setPrintPos(10, 10);   LCD.print("Data Log"); LCD.setPrintPos(120, 10); LCD.print(kong); 
  LCD.setFont(u8g_font_helvR08);  //body
  
  //LCD.setPrintPos(1, 61);   LCD.print("Drying Time: 5hrs");    


if(kong == 1){
  // --- highlighter...
LCD.drawHLine(1, 12, 126); LCD.drawHLine(1, 25, 126);      LCD.drawVLine(0, 13, 12); LCD.drawVLine(127, 13, 12); //--outer border
LCD.drawHLine(1, 13, 127); LCD.drawHLine(1, 24, 127);      LCD.drawVLine(1, 13, 12); LCD.drawVLine(126, 13, 12); //--inner border
  
LCD.drawBox(2, 14, 125, 10);   LCD.setColorIndex(0); LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23); LCD.print("Crop Details"); LCD.setColorIndex(1); 

LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 36);   LCD.print("Farm Details ");
LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 47);   LCD.print("Pump Details"); 

}

else if(kong == 2){
LCD.drawHLine(1, 25, 126); LCD.drawHLine(1, 38, 126);      LCD.drawVLine(0, 26, 12); LCD.drawVLine(127, 26, 12); //--outer border
LCD.drawHLine(1, 26, 127); LCD.drawHLine(1, 37, 127);      LCD.drawVLine(1, 26, 12); LCD.drawVLine(126, 26, 12); //--inner border
    
LCD.drawBox(2, 27, 125, 10);    LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("Crop Details");

LCD.setFont(u8g_font_helvR08); LCD.setColorIndex(0);LCD.setPrintPos(3, 36);    LCD.print("Farm Details "); LCD.setColorIndex(1); 
LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 47);   LCD.print("Pump Details"); 
}

else {
LCD.drawHLine(1, 2536, 126); LCD.drawHLine(1, 49, 126);      LCD.drawVLine(0, 38, 12); LCD.drawVLine(127, 38, 12); //--outer border
LCD.drawHLine(1, 37, 127); LCD.drawHLine(1, 48, 127);      LCD.drawVLine(1, 38, 12); LCD.drawVLine(126, 38, 12); //--inner border
   
LCD.drawBox(2, 38, 125, 10);    LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(3, 23);   LCD.print("Crop Details");
LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(3, 36);    LCD.print("Farm Details ");
LCD.setFont(u8g_font_helvR08); LCD.setColorIndex(0); LCD.setPrintPos(3, 47);   LCD.print("Pump Details");  LCD.setColorIndex(1); 
 
}
   

  LCD.drawFrame(0,53,36,11);
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Select");   //LCD.setPrintPos(104, 63); LCD.print("Back");

  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
  LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
  }while(LCD.nextPage());
}

void Water_Tank(uint8_t * current_amount,  float vol, float capacity, float waterHeight, const float tankHeight, const float tankRad) {
 
   LCD.firstPage();
  do{
  LCD.setFont(u8g_font_7x13B); // header --- u8g_font_6x10
      LCD.setPrintPos(2, 11);   LCD.print("Overhead WaterTank");  
     // LCD.setFont(u8g_font_helvR08); LCD.print("["); LCD.print(included_crops);  LCD.print("]");
   // body
  
      //sending message to the universe ... 
     //LCD.setFont(u8g_font_helvR08);
String Amt = "";
if(*current_amount < 101) Amt = String(*current_amount) + "%";
else Amt = "0! ";

  LCD.setFont(u8g_font_timR08); 
  LCD.setPrintPos(53, 24); LCD.print("Capacity:"); LCD.print(capacity); LCD.print("l");
  LCD.setPrintPos(53, 36); LCD.print("Amount:"); LCD.print(vol); LCD.print("l");
  LCD.setPrintPos(53, 49); LCD.print("Percent: "); LCD.print(Amt); 
  
  /*
 
  LCD.setPrintPos(16, 20); LCD.print("Amount:"); LCD.print(vol); LCD.print("l");
  LCD.setPrintPos(56, 30); LCD.print("Percent: "); LCD.print(*current_amount); LCD.print("%");
  LCD.setPrintPos(56, 40); LCD.print("Water Hi: "); LCD.print(waterHeight); 
  LCD.setPrintPos(56, 50); LCD.print("Tank Hi: "); LCD.print(tankHeight);  
  LCD.setPrintPos(56, 60); LCD.print("Tank Rad: "); LCD.print(tankRad);  
*/
 
  //    +/- 5l
  
 
 //-- how much water can be calibrated to every 20l resolution || 20l is one line on the screen
 
  LCD.drawRFrame(1, 20, 50, 30,2); // --- full tank outline 
  LCD.drawFrame(5, 16, 42, 5);  // --- top lid
  //LCD.drawLine(1, 20, 10, 15); LCD.drawLine(50, 15, 54, 20); // lid njola'z
//LCD.drawVLine ... tank stand
  if(*current_amount == 0){}

 else if(*current_amount > 0 && *current_amount < 25)  LCD.drawBox(2, 47, 48, 2);

 else if(*current_amount >= 25 && *current_amount < 50) LCD.drawRBox(2, 40, 48, 10, 1);
 
 else if(*current_amount >= 50 && *current_amount < 75) LCD.drawRBox(2, 30, 48, 20, 1);

 else LCD.drawRBox(2, 22, 48, 28, 1);

  //LCD.drawHLine(8, 10, 104); 
  //LCD.setFont(u8g_font_fub14); //LCD.print(171/0xa0);
  
  LCD.setFont(u8g_font_lucasfont_alternate); LCD.setPrintPos(2, 62);   LCD.print("Measure Now");   //LCD.setPrintPos(104, 63); LCD.print("Back");

  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");
  
  LCD.drawFrame(0,53,72,11);
  LCD.drawFrame(101,53, 26, 11);
  }while(LCD.nextPage());

  // ... select and cancel should be exactly of the same font & in the same position at all times ... 
}

void Farmer(String Farmer[]) { // --- Also read directly from EEPROM
  LCD.firstPage();
  do{
  LCD.setFont(u8g_font_tpssb); //header
  LCD.setPrintPos(8, 10);   LCD.print("Farmer Details"); // LCD.print(active_t_sensors); LCD.print(")");
  LCD.setFont(u8g_font_helvR08);  //body
  LCD.setPrintPos(1, 23);   LCD.print("Name: "); LCD.setFont(u8g_font_4x6); LCD.print(Farmer[0]); LCD.setFont(u8g_font_helvR08);   LCD.print("0/hr");  
  LCD.setPrintPos(1, 36);   LCD.print("Location: ");  LCD.print(Farmer[1]);  
  LCD.setPrintPos(1, 49);   LCD.print("Experience: ");  LCD.print(Farmer[2]);  

  LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(105, 62); LCD.print("Back");
  LCD.drawHLine(103, 53, 25); LCD.drawHLine(103, 63, 25);      LCD.drawVLine(103, 53, 11); LCD.drawVLine(127, 53, 11);
  }while(LCD.nextPage());
}

void Irrigation_Curve(){
  
  LCD.firstPage();
  do{
     
     LCD.setPrintPos(3,10); LCD.setFont(u8g_font_tpssb); LCD.print("Bean Kc Curve");
     LCD.drawVLine(1,2,50);  // --- y-axis
     LCD.drawHLine(2,51,120);// --- x-axis
     LCD.drawLine(2,12,20,14); // --- the curves
     LCD.drawLine(20, 14, 60, 40);
     LCD.drawHLine(60,40,10);
     LCD.setFont(u8g_font_helvR08); LCD.setPrintPos(60,63); LCD.print("Time");
      
      LCD.drawBox(102,53, 25, 11); // NavBar Box
      LCD.setColorIndex(0); LCD.setFont(u8g_font_lucasfont_alternate);    LCD.setPrintPos(104, 62); LCD.print("Back");
  }while(LCD.nextPage());
}


void showPower(float * batule, float full_capacity, float empty, float quarter, float half) {
  uint8_t battako = int(*batule);
LCD.firstPage();
do{
//      LCD.drawTriangle(60,58, 70,58, 64,63);

    LCD.drawRFrame(19,30,90,25,4);
    LCD.drawBox(17,38, 3, 8); 

   if(battako >= full_capacity){ // --- full or nearly full battery
    LCD.drawRBox(19,30,90,25,4);
        }

    else if(battako > half && battako < full_capacity){ //three quarter full 12.21 - 12.65
        LCD.drawRBox(39,30,70,25,4);
    }

   else if(battako > quarter && battako <= half){ // half full or half empty
     LCD.drawRBox(60,30,50,25,4);
    }
  else if(battako >= empty && battako <= quarter){ // a quarter of full
     LCD.drawRBox(85,30,25,25,4);
    }
   else { // retain empty box
         // LCD.drawFrame(17,38, 3, 8); 
    }
   
    LCD.setFont(u8g_font_helvR08);
    LCD.setPrintPos(2,10); LCD.print(run_the.powerMessage[0]); //solar status
LCD.setPrintPos(40,25); LCD.print(String(battako)); LCD.print("V"); //batt status

int adjPos;  adjPos = battako>12.00?50:(battako<=9.00?30:88);
uint8_t adjCon = battako>=half?0:1;

LCD.setColorIndex(adjCon);LCD.setPrintPos(adjPos,46); LCD.print(run_the.powerMessage[3]); LCD.setColorIndex(1); 
LCD.setPrintPos(2,62); LCD.print(y); LCD.print(run_the.powerMessage[2]);  //batt status
}while(LCD.nextPage());

  
  //powerMessage
}


void message_sent(){
  LCD.firstPage();
    do{



LCD.setColorIndex(0);LCD.drawRBox(18, 8, 92, 48,4);LCD.setColorIndex(1);
LCD.drawRFrame(18, 8, 92, 48, 4); 

LCD.setFont(u8g_font_helvB08);   LCD.setPrintPos(27, 26);   LCD.print("Message Sent"); 

LCD.setFont(u8g_font_lucasfont_alternate);    

  
      
  if(move_sideways == 1){
   LCD.drawBox(24,40,34,12); LCD.setColorIndex(0); LCD.setPrintPos(28, 49); LCD.print("View"); LCD.setColorIndex(1);
   LCD.setPrintPos(78, 49); LCD.print("Quit"); LCD.drawFrame(70, 40, 34, 12); 
  }
  else{
      LCD.drawBox(70,40,34,12); LCD.setColorIndex(0); LCD.setPrintPos(76, 49); LCD.print("Quit"); LCD.setColorIndex(1);
      LCD.setPrintPos(26, 49); LCD.print("View"); LCD.drawFrame(22, 40, 34, 12);
    }

//}

}while(LCD.nextPage());
  
  }


//close the display class
};




disp Screen;



// --- json encode-able --- //
struct datamine{
private:

  
unsigned long updateRegistry(){
  unsigned long Datalogging_Round = 0;
 // --- first get the data registry intervals  --- //    
    uint8_t last_value = EEPROM.read(3006);
    
    uint8_t tens = EEPROM.read(3005);
    uint8_t hundreds = EEPROM.read(3004);
    uint8_t thousands = EEPROM.read(3003);
    uint8_t ten_thousands = EEPROM.read(3002);
    uint8_t hundred_thousands = EEPROM.read(3001);

    last_value++; // update number of writes
    EEPROM.update(3006, last_value); // do the placevalue thingie
  
 if(EEPROM.read(3006) > 9) {  EEPROM.update(3006, 0); EEPROM.update(3005, (tens+1)); }
 if(EEPROM.read(3005) > 9) {  EEPROM.update(3005, 0); EEPROM.update(3004, (hundreds+1)); }
 if(EEPROM.read(3004) > 9) {  EEPROM.update(3004, 0); EEPROM.update(3003, (thousands+1));  }
 if(EEPROM.read(3003) > 9) {  EEPROM.update(3003, 0); EEPROM.update(3002, (ten_thousands+1)); }
 if(EEPROM.read(3002) > 9) {  EEPROM.update(3002, 0); EEPROM.update(3001, (hundred_thousands+1)); }

  Datalogging_Round = (hundred_thousands*100000) + (ten_thousands*10000) + (thousands*1000) +  (hundreds*100) + (tens*10) + (last_value * 1); 

  return Datalogging_Round;
}


//  --- the variables to set
  public:const String settings_file = "settings.txt";
   void setParameters(byte date_time[], String farmer[], String farm[], String soil[], String crop[], uint16_t pump, uint16_t valve){

 String Area, Soil_type,  Soil_FC, Soil_WP, Slope;  // soil details
 String Crop_name, Planting_Date, Root_zone, cropStage; // crop details

 String pumpCapacity, valveCapacity;
          bool Sir_or_Madam = true;
          String title;

String setup_date, setup_time; // time and date of purchase
String the_Name, Location, Village, Parish, Sub_county, District, Country, Experience, phoneNumber; // farmer credentials
 // --- *date_array => date, month, year ---- //

  setup_time = String(date_time[0]) + ":" + String(date_time[1]) + ":" + String(date_time[2]);
  setup_date = String(date_time[3]) + "/" + String(date_time[4]) + "/" + String(date_time[6]);

  title = Sir_or_Madam?"Mr. ": "Madam ";
  the_Name = title + farmer[0] + " " + farmer[1]; 
  Location = farmer[2] + ", " + farmer[3];
  District = farmer[4];  phoneNumber = farmer[5]; Experience = farmer[6]; 
  
  Area = farm[0]; Slope = farm[1];   
  Soil_type = soil[0];  Soil_WP = soil[1]; Soil_FC = soil[2];
  Crop_name = crop[0];  Root_zone = crop[1];Planting_Date = crop[2];

  pumpCapacity  = String(pump)  + "m3/s";
  valveCapacity = String(valve) + "m3/s";
  
 
String settings[19][2] = { 
                     {"Kit Serial Number: ", Kit_ID},
                     {"Setup Time: ", setup_time},  // --- time...year
                     {"Setup Date: ", setup_date}, // --- time...year
                     
                     {"Farmer Name: ", the_Name},
                     {"Farmer Location: ", Location},
                     {"Farmer District: ", District},
                     {"Phone Contact: ", phoneNumber},
                     {"Farming Experience: ", Experience},
                     
                     {"Farm Size: ", Area},     //soil
                     {"Farm Slope: ", Slope},    
 
                     {"Soil-Wilting Point: ", Soil_WP}, 
                     {"Soil-Field Capacity: ", Soil_FC},

                     {"Crop-type: ", Crop_name},    //crop
                     {"Crop-Root: ", Root_zone},
                     {"Crop-planted on: ", Planting_Date},
                                         
                     {"Irrigation valve: ", valveCapacity},
                     {"Submersible Pump: ", pumpCapacity},
                     {"Battery size: ", "150AH"},
                     {"Solar Panel Wattage: ", "150Wp"}}; // the scheduling 

// ===  save to local storage's settings file when called upon

  if(!SD.exists(settings_file)) {Serial.print(settings_file); Serial.println(" doesn't exist"); //settings_file = "/" + settings_file; //create it  
                              if(SD.mkdir(settings_file)) Serial.println("Created!"); else {Serial.println("Failed to create File: "); Serial.print(settings_file);}}
  unsigned int Set_dataPoints = 0;
            Set_dataPoints = sizeof(settings)/sizeof(settings[0]);
   
  local_storage = SD.open(settings_file, FILE_WRITE);

  // if the file opened okay, write to it:
  if (local_storage) {
//    Serial.print("Writing to "); Serial.println(settings_file); 
        for(int i=0; i<Set_dataPoints; i++){ if(i==0 || i==(Set_dataPoints-1)){local_storage.println();} local_storage.println();
         for(int j=0; j<2; j++){
           local_storage.print(settings[i][j]);
        }
      }
    local_storage.close();
  }

 

}



//-- binded_4_first_datalog   const String settings_file = "settings.txt";

String send_data(){
    String all_of_it = "";
  char each_dataset; //char each_char[512];
  delay(500);
Serial.println("Send data Summoned!");

local_storage = SD.open(dataFile);  
if(local_storage){ Serial.println("Local Storage Opened Successfully!");

      while(local_storage.available()){each_dataset = local_storage.read(); all_of_it+=each_dataset; } //kopolola

Serial.println("Data Copied");
Serial.print(all_of_it);  delay(500);
                    SIM.println("AT"); //Initiate data transmission when a certain number of handwashs has been bundled up
                      updateSerial();
       
                    SIM.println("AT+CMGF=1\r"); // Configuring TEXT mode
                      updateSerial();
                      
              //String phoneNumber[2] = {"AT+CMGS=\"+256759991993\"", "AT+CMGS=\"+256757330484\""};
                  //    for(int i=0; i<2; i++){
            
                    SIM.println("AT+CMGS=\"+256759991993\"");   //  --- Recipient number (s) --- MasterLine 
                    updateSerial();
                    
                      SIM.print("Data for : "); SIM.print(Kit_ID); updateSerial();
              
              SIM.println(all_of_it); updateSerial();  
                    //  }
                /*
                 for(int i=0; i<512; i++){ if(i==0 || i==(511)) SIM.println();
                                     SIM.print(each_char[i]]); updateSerial();    
                      }*/
                     // }
                    SIM.write(26);

} else Serial.println("Local storage Failed-!");


}

public: String Notification_Prompt = "Sending...";

String send_settings(){
  
  Serial.println("Send Settings Answered");
  //read settings
  String all = "Nothing!";
  char each; //char each_char[512];

local_storage = SD.open(settings_file);
/*
  // if the file opened okay, write to it:
  SIM.println("AT"); delay(200);
  SIM.println("AT+CMGF=1\r"); delay(200);
  SIM.println("AT+CMGS=\"+256759991993\""); delay(200);
  SIM.println("Testing..."); delay(200);
  SIM.write(26);

  delay(1000);
  */
  if (local_storage) {
Serial.println("File Opened. Scanning...");
Serial.println("Each and All"); all = "";

    while(local_storage.available()){each = local_storage.read(); all+=each; } //kopolola 
  Serial.println("Sending...");
  Serial.println(all);
//send settings
//send to several numbers
  
                    SIM.println("AT"); //Initiate data transmission when a certain number of handwashs has been bundled up
                      updateSerial();
       
                    SIM.println("AT+CMGF=1\r"); // Configuring TEXT mode
                      updateSerial();
                      
              //String phoneNumber[2] = {"AT+CMGS=\"+256759991993\"", "AT+CMGS=\"+256757330484\""};
                 //     for(int i=0; i<2; i++){

            
                    SIM.println("AT+CMGS=\"+256759991993\"");   // 256757330484 --- Recipient number (s) --- MasterLine 
                      updateSerial();
                      SIM.println("System Settings for: "); SIM.println(Kit_ID); //updateSerial();
              
              SIM.println(all); updateSerial();  
                /*
                 for(int i=0; i<512; i++){ if(i==0 || i==(511)) SIM.println();
                                     SIM.print(each_char[i]]); updateSerial();    
                      }*/
                    // }
                    SIM.write(26);
                    
      sending = false;
      Notification_Prompt = "Message Sent!"; // circled --- tick

  }else Notification_Prompt  = "Card Missing!"; //Card_Prompt


return Notification_Prompt;
}

//periodical data-logging
//compile one JSON-like associative array of the data


private:  unsigned long D_Round; // --- should store 0 to 999,999
          String dataFile = "dataLog.txt";
public: bool already_saved = false;

String Save(uint8_t hh_mm_ss[], uint8_t dd_mm_yy[], String crop_details[], unsigned int irrigation_parameters[]){

D_Round = updateRegistry();
String Field_Humi, Field_Temp, Area;
String Soil_type,  Soil_FC, Soil_WP, Slope;  // soil details
String Crop_name, Planting_Date, Root_zone, cropStage; // crop details

String data_logging_date, data_logging_time; // time and date of purchase
String irrigation_time, irrigation_schedule, irrigation_interval, irrigation_duration, irrigation_volume, irrigation_rounds; //irrigation details
String Soil_Moi_b4_irri, Soil_Moisture_Now;
String irrigation_how_long;


data_logging_time = String(hh_mm_ss[0]) +":"+ String(hh_mm_ss[1]) +":"+ String(hh_mm_ss[2]);
data_logging_date = String(dd_mm_yy[0]) + "/" + String(dd_mm_yy[1]) +"/"+ String(dd_mm_yy[2]);

Crop_name  = crop_details[0];
cropStage  = crop_details[1];
Field_Temp = crop_details[2];
Field_Humi = crop_details[3];
Planting_Date = crop_details[4];

irrigation_schedule = String(irrigation_parameters[0]) + " + 3 GMT";
irrigation_time     = String(irrigation_parameters[1]) + " hours/O\'Clock";
irrigation_duration = String(irrigation_parameters[2]) + " minutes";
irrigation_interval = String(irrigation_parameters[3]) + " hours";
irrigation_volume   = String(irrigation_parameters[4]) + " litres"; 

Soil_Moi_b4_irri  = String(irrigation_parameters[5]) + "%";
Soil_Moisture_Now = String(irrigation_parameters[6]) + "%";
Soil_WP = String(irrigation_parameters[7]);
Soil_FC = String(irrigation_parameters[8]);


String binded_for_datalog[16][2] = {
                                    {"Kit Identity: ", Kit_ID},
                                    {"Data-logging Round: ", String(D_Round)},
                                    {"Time: ", data_logging_time},
                                    {"Date: ", data_logging_date},

                                    {"Crop: ", Crop_name},
                                    {"Crop Stage: ", cropStage},
                                    {"Crop-Root: ", Root_zone},
                                    {"Crop-planted on: ", Planting_Date},


                                    {"Field Temperature: ", Field_Temp},
                                    {"Field Humidity: ", Field_Humi},
                                    
                                    {"Irrigation Scheduled for: ", irrigation_schedule},
                                    {"Irrigated at: ", irrigation_time},
                                    {"Ittigation Lasted for: ", irrigation_how_long},
                                    {"Irrigated with volume (m^3): ", irrigation_volume},

                                    {"Soil Moisture before irrigation: ", Soil_Moi_b4_irri},
                                    {"Soil Moisture Now: ", Soil_Moisture_Now}};

unsigned int Save_dataPoints = 0;
             Save_dataPoints = sizeof(binded_for_datalog)/sizeof(binded_for_datalog[0]);

   // if(already_saved == true){
      String Message;
    
  if(!SD.exists(dataFile)) {Serial.print(dataFile); Serial.println(" doesn't exist"); //dataFile = "/" + dataFile; //create it  
                             if(SD.mkdir(dataFile)) Serial.println("Created!"); else {Serial.println("Failed to create File: "); Serial.print(dataFile); }}
  
  local_storage = SD.open(dataFile, FILE_WRITE);

  // if the file opened okay, write to it:
  if (local_storage) {
    Serial.print("Writing to "); Serial.println(dataFile); 

        local_storage.println();
    for(int i=0; i<Save_dataPoints; i++){
      for(int j=0; j<2; j++){
          local_storage.print(binded_for_datalog[i][j]);
        }
        local_storage.println();
      }

        local_storage.println();
        local_storage.close();
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Message = "Error Opening " + dataFile;
    Serial.println(Message);
    }
    already_saved = false;
//}
 String notification_;
notification_ = already_saved?"Saved!":"Failed!";
  return notification_;
} //close the save method / function


private:
unsigned int updateMessage_counts(){
unsigned int counter;

        // --- first get the data registry intervals  --- //    
    uint8_t last_value = EEPROM.read(2006);
    
    uint8_t tens = EEPROM.read(2005);
    uint8_t hundreds = EEPROM.read(2004);
    uint8_t thousands = EEPROM.read(2003);
    uint8_t ten_thousands = EEPROM.read(2002);

    last_value++; // update number of writes
    EEPROM.update(2006, last_value); // do the placevalue thingie
  
 if(EEPROM.read(2006) > 9) {  EEPROM.update(2006, 0); EEPROM.update(2005, (tens+1)); }
 if(EEPROM.read(2005) > 9) {  EEPROM.update(2005, 0); EEPROM.update(2004, (hundreds+1)); }
 if(EEPROM.read(2004) > 9) {  EEPROM.update(2004, 0); EEPROM.update(2003, (thousands+1));  }
 if(EEPROM.read(2003) > 9) {  EEPROM.update(2003, 0); EEPROM.update(2002, (ten_thousands+1)); }

  counter = (ten_thousands*10000) + (thousands*1000) +  (hundreds*100) + (tens*10) + (last_value * 1); 


      return counter;
}

public:

  // --- the functions to log or transmit the data  --- || 
  String Transmit(String irri_Parameter[]){

    D_Round = updateRegistry(); // data-logging round

    // messages sent
/*
am/pm....
moisture b4 irrigation 
max field capacity: ---FC ---:--- 30%
vary within 
never get to permanent wilting point: -:---WP---:-
 trigger if(between WP and lower than FC) excerpt
 */

message_counter = updateMessage_counts();
 
 
    //liters of water
    //using ... kW of power
    //

String moisture_before_irrigation = irri_Parameter[0] + "%";
String moisture_level_now = irri_Parameter[1] + "%";
String irrigation_start_time = irri_Parameter[2];
String irrigation_duration = irri_Parameter[3] + " minutes";
String irrigation_volume = irri_Parameter[4] + "litres\n";

String Field_temperature = irri_Parameter[5] + "C";
String field_humidity = irri_Parameter[6] + "%";
String DRound = String(message_counter) + "th time.\n";
String body = "";
/*
if(was_irrigating){
  body = "System irrigated today ";
  body += "\r";
  body += "Irrigation started at " + irrigation_start_time;
  body += " and lasted for " + irrigation_duration;
  body += "";
} else {}
*/
    
   String binded_4_transmission[8][2] = {{"Data received the the: ", DRound},
                                         {"Your Field was irrigated today starting at:  ", irrigation_start_time},
                                         
                                         {" for:  ", irrigation_duration},
                                         {" all using a volume of water amounting to: ", irrigation_volume},
                                         
                                         {"\nSoil Moisture before irrigation was : ", moisture_before_irrigation},
                                         
                                         {" and Soil Moisture just after irrigation:  ", moisture_level_now},
                                         

                                         {"\nThe field is now at temperature of: ", Field_temperature},
                                         {"and a humidity of: ", field_humidity}};

    unsigned int SIM_dataPoints = 0;
                 SIM_dataPoints = sizeof(binded_4_transmission)/sizeof(binded_4_transmission[0]);

    //GSM.///...
    //if(already_sent == false){
      Serial.println("Sending via GSM Network...");
                    SIM.println("AT"); //Initiate data transmission when a certain number of handwashs has been bundled up
                      updateSerial();
       
                    SIM.println("AT+CMGF=1\r"); // Configuring TEXT mode
                      updateSerial();

                      //for(int phonenumber=0;)
            
                    SIM.println("AT+CMGS=\"+256757330484\"");   //256759991993    --- Recipient number (s) --- MasterLine 
                      updateSerial();
                      SIM.print(Kit_ID); SIM.print("'s Transmission Today");  updateSerial();

                 for(int i=0; i<SIM_dataPoints; i++){ if(i==0 || i==(SIM_dataPoints-1)) SIM.println();
                     for(int j=0; j<2; j++){
                          SIM.print(binded_4_transmission[i][j]); updateSerial();    
                        } //SIM.println();
                      }
               
                    SIM.write(26);
//already_sent = true;  
     //}
String notification_;
notification_ = already_sent?"Sent!":"Failed!";
     
     return notification_;
 }
  
  void send_Message(String Irrigation_Message){
    SIM.println("AT"); //Initiate data transmission when a certain number of handwashs has been bundled up
                      updateSerial();
       
                    SIM.println("AT+CMGF=1\r"); // Configuring TEXT mode
                      updateSerial();

                    String sendTo = "";
                      for(int i=0; i<4; i++) {sendTo =  numberDB[i];
            
                    SIM.println(sendTo);   //  --- Recipient number (s) --- MasterLine 
                      updateSerial();
                  
                    SIM.print(Irrigation_Message); updateSerial();    
                        }
                  
                    SIM.write(26);
  }
};



datamine data;

uint8_t time_slot; 
bool set_up = false;
bool settings_saved = false; 


void setup() {  // === Kit_ID = 1; 

pinMode(back_light, OUTPUT);
LCD.begin();
  
int loader = 0; int pos = 0;
bool flip = true;

String bootPrompt = "";
//while(digitalRead(power)){delay(500); break;}

int humi_temp_signal = 6, humi_temp = 13;
//boot screen 
// -- IRRI-KIT
 
  Serial.begin(9600);
  String Status;
 String SD_Prompt = "Faulty"; bool SD_OK = false;

while(loader<100){ loader++; pos++; flip = !flip; //................................

    if(loader>=0 && loader<=40){ 
           if(loader == 1){Serial.begin(9600);}
      else if(loader == 2){SIM.begin(9600);}
    else if(loader == 3){sensor.begin(); pinMode(humi_temp_signal, OUTPUT); digitalWrite(humi_temp_signal, HIGH); }
      else if(loader == 4){pinMode(pumpPin, OUTPUT); digitalWrite(pumpPin, HIGH);}
      else if(loader == 5){pinMode(valvePin, OUTPUT); digitalWrite(valvePin, HIGH);}
      else if(loader == 6){for(int i=0; i<4; i++){ pinMode(soil_moisture_signal[i], OUTPUT); digitalWrite(soil_moisture_signal[i], HIGH);}}
      else if(loader == 7){pinMode(back_light, OUTPUT);  digitalWrite(back_light, 1);}
      else if(loader == 8){
               
               Serial.print("Initializing SD card...");  
  for(int check=0; check<2; check++){
  if (!SD.begin(CS)) {SD_OK = false; Serial.println("SD_Card Initialization Failed!"); 
                      delay(100);
                      //if(check>=3) //while (1);
  }}   Serial.println("initialization done."); SD_OK = true;

        
      } 

      
      else if(loader == 10){ water_percent = exec.check_tank(tank_level_sensorPin);  Status = "Tank";}
               
       if(loader == 15){pinMode(power_indica, OUTPUT); }
  else if(loader == 16){pinMode(right, INPUT);  }
  else if(loader == 17){pinMode(left, INPUT); pinMode(back, INPUT); pinMode(up, INPUT);}     
  else if(loader == 18){pinMode(set, INPUT);}
  else if(loader == 19){pinMode(back, INPUT); pinMode(up, INPUT); }
  else if(loader == 20){pinMode(Okay, INPUT);   pinMode(down, INPUT);}
  //pinMode fans and sensing nodes
  
  else if(loader > 26 && loader <= 28){digitalWrite(power_indica, flip);/*  tone(sound_fx,2100);     */}
//pinModes show connected devices ... 


LCD.firstPage();  
  do { 
  // LCD.setFont(u8g_font_fub42n); LCD.setPrintPos(1, 50);   LCD.print("Intellysis 2");
    LCD.setFont(u8g_font_fub20); LCD.setPrintPos(24, 24);   LCD.print("irri-kit");
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(20, 35);   LCD.print("Operating System"); 

    LCD.drawBox(15, 42, pos, 7); 
    //LCD.drawHLine(10, 40, 112); LCD.drawHLine(10, 49, 112); // --- exactly 100 spaces in between
   // LCD.drawVLine(10, 41, 8);   LCD.drawVLine(121, 41, 8);
    
    
    LCD.setFont(u8g_font_helvB08);  LCD.setPrintPos(60, 63);  LCD.print(loader); LCD.print("%");
 // LCD.setPrintPos(62, 1);  LCD.print(F("Initializing Sensors!"));
  } while( LCD.nextPage());

}

else if(loader>40 && loader<=68){ digitalWrite(power_indica, flip); 
     if(loader == 40){digitalWrite(soil_moisture_signal[0], LOW); digitalWrite(back_light,HIGH);}
     else if(loader == 50){digitalWrite(soil_moisture_signal[1], LOW);}
     else if(loader == 60) {digitalWrite(soil_moisture_signal[2], LOW);}


     
LCD.firstPage();  
  do { 
     LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(20, 10);   LCD.print("Irri-Kit OS");
     LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(10, 25);   LCD.print("Makerere University");
     LCD.setPrintPos(10, 35);   LCD.print("Badaye Technologies");
       

    LCD.drawBox(15, 55, pos, 7); LCD.drawFrame(15,55,100,7);
   
    
    LCD.setFont(u8g_font_helvB08);  LCD.setPrintPos(80, 50);  LCD.print("2021");
  } while( LCD.nextPage());

}



else if(loader>68 && loader<=82){  digitalWrite(power_indica, flip);
    if(loader == 80) digitalWrite(soil_moisture_signal[3], LOW);

LCD.firstPage();  
  do { 
    LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(20, 10);   LCD.print("Lead Team");
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(2, 25);   LCD.print("1. Dr. J. Wanyama"); 
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(2, 35);   LCD.print("2. Dr. P. Nakawuka"); 
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(2, 45);   LCD.print("3. Mr. A. Komakech"); 
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(2, 55);   LCD.print("3. Mr. P. Tumutegyereize"); 



  //  LCD.drawBox(15, 55, pos, 7); LCD.drawFrame(15,55,100,7);
   
    
    
  } while( LCD.nextPage());
 
  }

else if(loader > 82 && loader <= 100){ // 82 - 94 --- 19
  
  SD_Prompt = SD_OK?"OK":"Faulty";

LCD.firstPage();  
  do { 
    LCD.setFont(u8g_font_7x13B); LCD.setPrintPos(12, 10);      LCD.print("irri-kit OS POST"); 
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(5, 25);   LCD.print("Power : "); LCD.print("OK");
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(5, 35);   LCD.print("SD : ");  LCD.print(SD_Prompt); 
    LCD.setFont(u8g_font_helvR08);  LCD.setPrintPos(5, 45);   LCD.print("Sensors: ");  LCD.print("OK");


    LCD.drawBox(15, 55, pos, 7); LCD.drawFrame(15,55,100,7);
   
    
  } while( LCD.nextPage());

if(loader == 85){digitalWrite(power_indica, OFF); }
        
      /*      
else { //95, 96, 97, 98, 99, 100
String phone[2] = {"\"+256759991993\"", "\"+256757330484\""};
}    
*/
        else if(loader == 99){ bootPrompt = "Connecting to Network...";
/*
  SIM.println("AT");delay(500);
  SIM.println("AT+CCLK?");delay(500); // +CCLK: "04/01/01,00:14:12+22"
  SIM.println("AT+CLTS=1"); delay(500);// Enable auto network time sync : 
  SIM.println("AT+CLTS?");  delay(500);// Check if value is set : 
  SIM.println("AT&W"); delay(500); // Save the setting to permanent memory so that module enables sync on restart also  :  
  SIM.println("AT+CCLK?"); delay(500);// Read current Time
    
     Serial.write(SIM.read());
     delay(2000);        
  */      
delay(1000);   
SIM.println("AT+CCLK?"); extract();  //Read current Time

SIM.println("AT"); 
                      updateSerial();
       
                    SIM.println("AT+CMGF=1\r"); // Configuring TEXT mode
                      updateSerial();
                      //String phones[5] = {};c
                      
                    SIM.println("AT+CMGS=\"+256757330484\"");   //  --- Recipient number (s) --- MasterLine 
                      updateSerial();
                      SIM.print(Kit_ID); SIM.print("has been turned ON  Now");  updateSerial();
                      
                    SIM.write(26);
      delay(500);

      
                    SIM.println("AT"); delay(500);
                    SIM.println("AT+CMGF=1\r"); delay(500);// Dr. Wanyama
                    SIM.println("AT+CMGS=\"+256755279145\"");   delay(500);
   
                    SIM.println(Kit_ID); SIM.println(" has just been switched ON Now"); 
                      
                    SIM.write(26); 
                    
/*                    delay(500);

                    SIM.println("AT"); delay(500);
                    SIM.println("AT+CMGF=1\r"); delay(500);// Dr. Prossie
                    SIM.println("AT+CMGS=\"+2567018533322\"");   delay(500);
   
                    SIM.println(Kit_ID); SIM/print(" has just been switched ON Now"); 
                      
                    SIM.write(26); 
  */                  
                    delay(500);
        
                    SIM.println("AT"); delay(500);
                    SIM.println("AT+CMGF=1\r"); delay(500);// Dr. William
                    SIM.println("AT+CMGS=\"+256758643906\"");   delay(500);
   
                    SIM.println(Kit_ID); SIM.print(" has just been switched ON Now"); updateSerial();
                      
                    SIM.write(26); 
                    
                    delay(500);
                    
              //for(int i=0; i<2; i++){ cycle through various phone numbers and delay(500) each round
                SIM.println("AT"); delay(500);
                    SIM.println("AT+CMGF=1\r"); delay(500);// Configuring TEXT mode
                    SIM.println("AT+CMGS=\"+256759991993\"");   delay(500); // MasterLine
   
                    SIM.println(Kit_ID); SIM.print(" has just been switched ON"); 
                    SIM.println("System Time=>"); SIM.println("AT+CCLK?"); updateSerial(); delay(500);
                      
                    SIM.write(26); delay(500);
                   
              }


 
  }
  
  
// 3 procedures to load a progress bar
   
} // while

for(int i=0; i<4; i++){ digitalWrite(soil_moisture_signal[i], LOW);}

digitalWrite(power_indica, HIGH);
Serial.println("SD Card OK!");



  set_up = true;
    
   
    current_Screen = 9;
         
} 
// setup()

//byte countr = 0; //uint8_t changing_second = second;

 float temp = 0.00, humi = 0.00;


unsigned long current_millis;
unsigned int irrigation_duration, actual_duration;

// --- TIME STORAGE ---- //

bool locked = false, first_pass_on = false, first_pass_off = false; // tik tok lockers
String Time_Prompt = "";

uint8_t sub_routine = 0;
//unsigned long seconds = 0; //  
bool toggle = false; unsigned int indicate = 0;


uint16_t trigger_moisture_read = 1;

String in_progress = "null";


 bool ManagePower(uint8_t * battery_reader);

String irrigate_icon = "unset";
String data_icon = "unset";
String sent_icon = "unset";

float h = 0.00, t = 0.00;
unsigned int ct_Dwn = 0;
byte led_blink = 0;

unsigned long count_down_in_secs = 0;

    float tank_b4_irri = 0.00, tank_after_irri = 0.00;
    float moi_b4_irri = 0.00;
    float moi_after_irri = 0.00;
    float Soil_WP = 20.00, Soil_FC = 30.70;
    unsigned int trackMoisture = 0; unsigned int waiting_time = 0; 
    uint8_t starting_hour = 0, starting_min = 0, starting_sec = 0;
    uint8_t starting_date = 1, starting_month = 1, starting_year = 21;


    uint16_t valve_discharge = 2; // m^3/s
    uint16_t pump_discharge = 500; //m^3/s

 // scheduled tasks
    bool tank_unlocked = false;
    bool save_unlocked = false;
    bool irri_unlocked= false;

    bool send_unlocked = false;

    String MessagePrompt = "";
    
void system_setup(){   
  current_Screen = 1; 
/*
       while(current_Screen<9){ 
               if(current_Screen==1) Welcome to irrikit;
          else if(current_Screen==2) Name, Location, district, phoneNumber, Experience;
          else if(current_Screen==3) Farm Size, Farm Slope;
          else if(current_Screen==4) Soil WP, Soil FC;
          else if(current_Screen==5) Crop type, Rootdepth, Planting_Date;
          else if(current_Screen==6) Irrigation Valve, Pump;
          else if(current_Screen==7) Batery, Panel;
          else if(current_Screen==8) Date, Time;
  current_Screen++; }
  if(settings_saved) set_up = true; //break;

  
          byte date_n_time[7] = {7, 49, 55, 17, 8, 20, 21};
          String farmer_details[7] = {"Moses", "Kiganga", "Bunamwamba Village", "Bukyabo Sub County", "Sironko", "0772001002", "10 years"};
          String farm[10] = {"100ha", "2deg", "25degC"};
          String soil[5] = {"loamy clay", "15", "75"};
          String crop[5] = {"beans", "20cm", "1st April 2021"};

          valve_discharge = 2; // m^3/s
          pump_discharge = 500; 
          unsigned long irrigationParam[10] = {820, 24, 15, 200, 100, 30, 40};

if (settings_saved) {
          String _name; //char _name[20]
// --- save the time 
EEPROM.update(time_slot, Screen.temp_hr); EEPROM.update(time_slot+1, Screen.temp_min); 

// --- save the date
Screen.temp_year%=100;
EEPROM.update(time_slot+3, Screen.temp_date); EEPROM.update(time_slot+4, Screen.temp_month); EEPROM.update(time_slot+5, 20); EEPROM.update(time_slot+6, Screen.temp_year);

// --- save the crop data
EEPROM.update(7, Screen.selekta); EEPROM.update(8, Screen.move_box);
      
        hour = EEPROM.read(0); minute = EEPROM.read(1); second = EEPROM.read(2);    // --- time
        day = EEPROM.read(3); month = EEPROM.read(4); year = (100*(EEPROM.read(5)) + EEPROM.read(6)); // --- date
        drying_crop = EEPROM.read(7); drying_regime = EEPROM.read(8);        // --- crop/speed



   // --- do the actual work  --- // 
      data.setParameters(date_n_time, farmer_details, farm, soil, crop, pump_discharge, valve_discharge);
      

// --- save the phone number of the intended receipient

// --- save the name of the dryer owner
_name = ""; //EEPROM.update(10, DEC(_name));
// --- send personalized notification SMS to the phone

String first = "Congratulations," + _name + ", Your irrikit has been successful set up for your farm in Kabanyoro.";

String second = "In this kit there is, an AI entrusted with the task of monitoring your farm";
String third = "I'll be periodically updating you on all the affairs of your maize farm.";
String fourth = "Wherever neccessary, I'll rebalance soil moisture by pumping water.";


String messages_2_send[4] = {first, second, third, fourth};

String receiver[2] = {"AT+CMGS=\"+256759991993\"", "AT+CMGS=\"+256757564289\""};



  SIM.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  SIM.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  for(int jk=0; jk<2; jk++){
   for(int i=0; i<=4; i++){
  
      SIM.println(receiver[jk]); // their_number ---  //receiver of message
      updateSerial();
  
      SIM.print(messages_2_send[i]); // message to send //Phone.print(":"); Phone.print(minute); //text content
      updateSerial();
  }}
  SIM.write(26);

    
tone(sound_fx, 817); delay(400); noTone(sound_fx);  delay(20); // --- message sent notification
tone(sound_fx, 817); delay(400); noTone(sound_fx);    
 //delay(1000); // delay between several messages
  




}
*/

scan_buttons();
updateScreen(&current_Screen, &second);
}

//track time from a fixed reference point  to make ^ stop watch 
      unsigned long starting_time_in_seconds=0;  //|\\ e.g.  22800; // --- 6:20:25 
      unsigned long stopping_time_in_seconds=0; // | \\ e.g. 24300; //  --- 6:45:32

float Soil_FieldCap = 30.00;
float Soil_WiltinP = 13.00;
const float tank_radius = 0.53, tank_height = 1.20;
float water_height = 0.00, water_volume = 0.00;                           

bool flip = false;
//start

String all_data = "=> ";
char read_byte; 

char acquired[70];
char stolen[17];

 /*char Month[8] = "Null";*/ String Month;


void extract(){  delay(50);
  int i=0;
  while(SIM.available())   {  read_byte = SIM.read(); acquired[i] = read_byte;  all_data += read_byte; i++;}
}   


String SystemClock = "HH:MM dd/mm/yyyy";
String NetworkTime = "", NetworkDate = "";
//end

void loop() { // --- since no reaction is instantaneous here, we will use some non-overlapping scheduling method
//---heavily event-driven program
    a++;
  while(!set_up){  system_setup();  }
     
SIM.println("AT+CCLK?"); extract();

Excavator();
;;
//Serial.print("Extracted time :: "); Serial.print(hour); Serial.print(":"); Serial.print(minute); Serial.print(":"); Serial.print(second);
;;    //schedulable schedulable tasks first
        tank_unlocked = false;
    if(second%30 == 0) tank_unlocked=true;

    if(hour %3 == 0) {send_unlocked = true; }
       else {send_unlocked = false; already_sent=false;}     
                   
    if(hour %5 == 0) {save_unlocked = true; send_unlocked = true;}
    else {save_unlocked = false; send_unlocked = true;}
            

     if(sub_routine == 1) { power_status = run_the.ManagePower(&battery_pin); // every two seconds, &power_signals) the reason is to cut off power supply
                            Serial.print("Power Status: "); Serial.println(power_status); }

      //  --- it is unwise to measure tank level multiple times per second
else if(sub_routine == 2) {   if(tank_unlocked){  // else just keep the PUMPING... notification on the screen 
                                 tank_status  = exec.check_tank(tank_level_sensorPin); // after two other seconds using sonar -- if low, pump in more
                                 water_percent = exec.tank_levo;
                                 water_height = float(water_percent)/100.00 * tank_height;
                                 water_volume = water_height * 3.14 * (tank_radius*tank_radius); //in cubic meters
                                 water_volume *= 1000.00; // in litres
                                 }

                              
                              Pumping = run_refill_pump(&pumpPin, tank_status, power_status, water_percent);  

                //always have water just enough to complete an irrigation event
                            //datalog after a succesful pumping
                            tank_unlocked=false;}   

else if(sub_routine == 3) moisture_now = moischa(connected_soil_sensors, soil_moisture_signal);    // , &soil_sginals) in it first detect plugged in status

else if(sub_routine == 4) { 

// the time constraint thing --- how do we know it would be irrigating when time is not right???

         if(moisture_now > 22.50) {/* Do Nothing, if ON, power OFF */ irrigationPrompt = "Soil Moisture OK";
              if(valve_is_ON) {digitalWrite(valvePin, HIGH); valve_is_ON=false;} }
    else if(moisture_now <= 21.50 && moisture_now > 0.00){
           // if(water_percent < 30) { irrigationPrompt = "No Water!";/* Kwatila wano, compute Vol Deficit, refill tank to 70%, then return HERE */}
       // else{
          //    if(!power_status){ irrigationPrompt = "Low Power"; /* Kwatila wano, recharge batule first to 12.6V, then return HERE */}
           //   else{
                  //  --- implement the irrigation LOGIC
                  //computing the moisture deficit between RAM and instantaneous moisture
                  //return countdown counter
                  //e.g.
                      
                      //frst get vol
                      float wetted_area = 0.00; float application_rate = 0.00;
                            wetted_area = bosco.emitter_spacing * bosco.drip_line_spacing;
                            application_rate = bosco.emitter_flow_rate / wetted_area;
                            application_rate *= 0.95; // because of the efficiency
                      float duration_in_minz = 0.00; float duration_in_hr = 0.00;      
                     duration_in_hr = (bosco.FieldCapacity - moisture_now)*10 * bosco.root_depth_at_stage_x / application_rate;
                     duration_in_minz = duration_in_hr * 60;
                     
                     if(was_irrigating) waiting_time = (5*60);  // 5 minutes for water seepage

                    if(!valve_is_ON){ //teekako valuvu
      irrigationPrompt = "Irrigating...";
      digitalWrite(valvePin, LOW);  valve_is_ON = true; 
        was_irrigating = true;
        starting_time_in_seconds = waiting_time + current_millis; 

        starting_hour = hour; starting_min = minute; starting_sec = second;
        stopping_time_in_seconds = starting_time_in_seconds + (duration_in_minz*60);

        moi_b4_irri = moisture_now;
        tank_b4_irri = water_volume; 


        count_down_in_secs = stopping_time_in_seconds - current_millis;        
 }
 else if(valve_is_ON) { 
  digitalWrite(soil_moisture_signal[0], flip);
  digitalWrite(soil_moisture_signal[1], flip);
  digitalWrite(soil_moisture_signal[2], flip);
  digitalWrite(soil_moisture_signal[3], flip);
  
  
   if(count_down_in_secs == 0) {irrigationPrompt = "Not Irrigating!";}
   else{
      if(count_down_in_secs == current_millis) {count_down_in_secs = 0; digitalWrite(valvePin, HIGH); valve_is_ON = false; 
      moi_after_irri = moisture_now;
      tank_after_irri = water_volume;
      float irri_vol = tank_after_irri - tank_b4_irri;
      //temp = humi_temp(humi_temp_signal);  humi = h; 

String stat_time = String(starting_hour) + ":" + String(starting_min);

String irri_param[7] = {String(moi_b4_irri), 
                       String(moi_after_irri), 
                       stat_time, 
                       String(duration_in_minz), 
                       String(irri_vol),
                       String(temp),
                       String(h)};

     // data.Transmit(irri_param); // ---  doesn't make sense since may send 100 times a day


//       data.Save(); 
      irrigationPrompt = "Stopped Irrigating!"; }
      else   count_down_in_secs = stopping_time_in_seconds - current_millis;
        if(was_irrigating) irrigationPrompt = "Finished!";
        else irrigationPrompt = "Not Irrigating!";
      }
    }
    
  }

                     //actual_duration = (Soil_FieldCap - int(moisture_now+0.50)) / emmiter_flow_rate;  // --- time to fill the remainder
             // }
         // }
}

    /*
else if(sub_routine == 5){
      if(save_unlocked) {
       if(data.already_saved == false) {
        uint8_t hh_mm_ss[3] = {hour, minute, second};
         uint8_t dd_mm_yy[3] = {day, month, Y2};
         // --- get crop, pump, soil details from SD card --- //
          String crop_details[5] = {"Beans", "Flowering", String(temp), String(humi), "1/4/21"};
           //temp = humi_temp(humi_temp_signal);  humi = h; 
         // unsigned int irrigation_parameters[9] = {ct_Dwn, irri_hour, actual_duration, waiting_time, (int)water_volume, (int)moi_b4_irri, (int)moisture_now, (int)Soil_FC, (int)Soil_WP};

        //datalogPrompt = data.Save(hh_mm_ss, dd_mm_yy, crop_details, irrigation_parameters); //if time is due for data-logging
        data.already_saved = true;
      } else datalogPrompt = "Already Saved!";
    } else datalogPrompt = "Can't Save Now!";
     
      if(send_unlocked == true){ //prevent sends in ODD hours
         if(already_sent==false) { //prevent from multiple transmissions in 1 instant
              uint8_t hh_mm_ss[3] = {hour, minute, second};
              String time_now = String(hour) + ":" + String(minute);
              //temp = humi_temp(humi_temp_signal);  humi = h; // Sensor readings up to 2 seconds 'old' (its a very slow sensor)
               String irrigation_arguments[8] = {String(temp), String(humi), String(ct_Dwn), time_now, String(water_volume), String(moi_b4_irri), String(moi_after_irri), "100"};

              MessagePrompt = data.Transmit(irrigation_arguments); // if the time due for data transmission
              //already_sent = true;
           } else MessagePrompt = "Already Sent!"; 
       } else MessagePrompt = "Can't send Now!";
 // } else MessagePrompt = "Power Low!"; datalogPrompt = "Power Low!";
}

*/

else sub_routine = 0;

//Serial.print("Message Send Notification: "); Serial.println(MessagePrompt);
//Serial.print("Message Save Notification: "); Serial.println(datalogPrompt);

//if power_status == HIGH ... 
         if(dim_screen >= 500 && dim_screen < 700) digitalWrite(full_beam, LOW); //dim=true
    else if(dim_screen >= 700) { digitalWrite(dim, LOW);
                 if(dim_screen >= 2e9) dim_screen = 100; }
    else if(dim_screen >= 2000){
      current_Screen = 9; //return_home;
      Screen.dj_selekta = 1;
      Screen.scroll = 1; //home
      selected_menu_option = 1; //menu
      Screen.set_option = 1;//settings
      Screen.kong = 1; //crop select
  
    }
    else {digitalWrite(full_beam, 1);  digitalWrite(dim, 1);}

  
  
          if(led_blink<=1) digitalWrite(power_indica, HIGH); 
    else if(led_blink >= 2 && led_blink < 8) {digitalWrite(power_indica, LOW); flip != flip;}
    else {led_blink=0;}

    led_blink++;
    
   

   scan_buttons();

   updateScreen(&current_Screen, &second);
   dim_screen++;  //sleepScreen+;
   
sub_routine++;
} // --- loop()



void Excavator(){
for(int i=0; i<70; i++){
  //Serial.print(acquired[i]);
  if(acquired[i] == '"' && (acquired[i+1] == '2' || acquired[i+1] == '0')){//first occurence
    for(int j=0; j<17; j++){
      stolen[j] = acquired[i+j+1];
    }
   }
  }
//delay(500);

char concatenate[2];

for(int i=0; i<17; i++){

 if(stolen[i] == ','){ Serial.println();
    concatenate[0] = stolen[i+1]; //Serial.print("Char 1: "); Serial.println(concatenate[0]);
    concatenate[1] = stolen[i+2]; //Serial.print("Char 2: "); Serial.println(concatenate[1]);
    hour = 10 * (int(concatenate[0]) - 48)  + (int(concatenate[1]) - 48);

    concatenate[0] = stolen[i+4];
    concatenate[1] = stolen[i+5];
    minute = 10 * (int(concatenate[0]) - 48)  + (int(concatenate[1]) - 48);

    concatenate[0] = stolen[i+7];
    concatenate[1] = stolen[i+8];
    second = 10 * (int(concatenate[0]) - 48)  + (int(concatenate[1]) - 48);

    //backward to date... up to i-7 && i-8
    concatenate[0] = stolen[i-2];
    concatenate[1] = stolen[i-1];
    day = 10 * (int(concatenate[0]) - 48)  + (int(concatenate[1]) - 48);

    concatenate[0] = stolen[i-5];
    concatenate[1] = stolen[i-4];
    month = 10 * (int(concatenate[0]) - 48)  + (int(concatenate[1]) - 48);

    concatenate[0] = stolen[i-8];
    concatenate[1] = stolen[i-7];
    year = 10 * (int(concatenate[0]) - 48)  + (int(concatenate[1]) - 48);

  break;    
    }
  }

}


void calendar(){
year += 2000;
    // shift to the display class{};
      if(month==1) Month = "January";    else if(month==2) Month = "February"; else if(month==3) Month = "March";
 else if(month==4) Month = "April";      else if(month==5) Month = "May";      else if(month==6) Month = "June";
 else if(month==7) Month = "July";       else if(month==8) Month = "August";   else if(month==9) Month = "September";
 else if(month==10)Month = "October";    else if(month==11) Month = "November";     else if(month==12) Month = "December"; 
}


String run_refill_pump(const uint8_t * pin, bool power_is_Available, bool tank_is_beyond_quarter, unsigned int levo_when_pumping){
  
  
  // Serial.println("Refiller answered!");

   if(power_is_Available == false){Pump_Prompt = "Power Low";}//charge until 12.6V //blink in red and put dangreous icon --- njagala okupimpinga naye power
   else{
        if(water_percent >= 1 && water_percent <=100){Pump_Prompt = "Tank tank at: "+String(levo_when_pumping); //already some water
           
         if(water_percent<40){ //if(!pump_is_ON)Pump_Prompt = "Tank Empty";   //if quarter or below
              if(water_percent == 0){Pump_Prompt = "Sensor Out!";}
              else {if(pump_is_ON == false) {digitalWrite(*pin, LOW); pump_is_ON = true; levo_when_pumping = water_percent;} // blink in green
                    if(pump_is_ON) Pump_Prompt = "Pumping.. ";
              }
        }
        else if(water_percent >= 80){
              if(pump_is_ON) {digitalWrite(*pin, HIGH); pump_is_ON = false; Pump_Prompt = "Now Full"; /*data.Save()*/} } //now full, record current volume
           //in the 30 seconds of sensing, the tank could have pumped about 
        
   }//if no water...
        else Pump_Prompt = "No Water!!!"; //if beyond 100 ... not in or very very empty
  }
     //Pump_Prompt = (pump_is_ON)?"Pumping":"Not pumping";
     //else {digitalWrite(pump, HIGH); pump_is_ON = false;}
    //return true;
    return Pump_Prompt;
}


float m;
bool plugged_1 = false; bool last_state1 = false;
bool plugged_2 = false; bool last_state2 = false;
bool plugged_3 = false; bool last_state3 = false;
bool plugged_4 = false; bool last_state4 = false;


uint8_t active_moisture_sensors, active_tank_sensors = 0, active_temp_sensors = 0;
uint8_t all_sensors = 0;
      

String M_Readings[4]; 
bool secret_moischa_key = false;

float moischa(const uint8_t  connecteddy[],  const uint8_t lights[]){
  float m1, m2, m3, m4;
   //if(secret_moischa_key ==true )secret_moischa_key = false;
  m=0;  active_moisture_sensors = 0;
    // --- The following is based on the assumption that we know them...
     //  m1 = analogRead(A4);  m2 = analogRead(A7);  m3 = analogRead(A6);  m4 = analogRead(A5);

       m1 = analogRead(connecteddy[0]);  m2 = analogRead(connecteddy[1]); m3 = analogRead(connecteddy[2]); m4 = analogRead(connecteddy[3]);
       
      
    if(m1 == m2 == m3 == m4 >= 5){ M_Readings[0] = M_Readings[1] = M_Readings[2] = M_Readings[3] = "Power!!";   }
else{
     
  if(m1 >= 1020){ M_Readings[0] = "Sensor Out"; plugged_1 = false; last_state1 = false;}
  else { plugged_1 = true; 
    if(plugged_1 && !last_state1){digitalWrite(lights[3], 1); delay(50); /*tone(sound_fx, 2000,50);*/   digitalWrite(lights[3], 0); last_state1 = true; }
      m1 = float(map(m1, 1023,0, 1023,0)); 
      
    //y = 0.002*(x^3) - 0.1137*(x^2) + 2.1434*(x) - 9.3544
    //m1 = 0.0020*(m1*m1*m1) - 0.1137*(m1*m1) + 2.1434*(m1) - 9.3544;
    
    //y = 0.1391x + 0.743; 
    //m1 = 0.1391m1 + 0.743;

    //y = 0.0211x + 3.7722;
    m1 = 0.0211*m1 + 3.7722;
  
      /*m1 = constrain(m1,0,5);*/ 
      M_Readings[0] = String(int(m1)+0.50);// + "V";
      m+=m1; active_moisture_sensors++; } 
  
  if(m2 >= 1020){ M_Readings[1] = "Sensor Out";  plugged_2 = false; last_state2 = false;}
  else { plugged_2 = true; 
    if(plugged_2 && !last_state2){digitalWrite(lights[2], 1); delay(50); /*tone(sound_fx, 2000,50);*/   digitalWrite(lights[2], 0); last_state2 = true;}
        m2 = float(map(m2, 1023,0, 1023,0)); //m2 = constrain(m2,0,5); 
        m2 = 0.0211*m2 + 3.7722;
        M_Readings[1] = String(int(m2)+0.50);// + "V"; 
        m+=m2; active_moisture_sensors++; }

    
  if(m3 >= 1020){ M_Readings[2] = "Sensor Out";  plugged_3 = false; last_state3 = false;}
    else {plugged_3 = true; 
    if(plugged_3 && !last_state3){digitalWrite(lights[1], 1);delay(50); /*tone(sound_fx, 2000,50);*/    digitalWrite(lights[1], 0); last_state3 = true;}
        m3 = float(map(m3, 1023,0, 0,1023)); //m3 = constrain(m3,0,5); 
        m3 = 0.0211*m3 + 3.7722;
        M_Readings[2] = String(int(m3)+0.50);// + "V";
        m+=m3; active_moisture_sensors++;   }

    
  if(m4 >= 1020){ M_Readings[3] = "Sensor Out";  plugged_4 = false; last_state4 = false;}
    else { plugged_4 = true;  
    if(plugged_4 && !last_state4){digitalWrite(lights[0], 1); delay(50); /*tone(sound_fx, 2000,50);*/   digitalWrite(lights[0], 0); last_state4 = true;}
        m4 = float(map(m4, 1023,0, 0,1023)); //m4 = constrain(m4,0,5); 
        m4 = 0.0211*m4 + 3.7722;
        M_Readings[3] = String((m4)+0.50);// + "V"; 
       m+=m4; active_moisture_sensors++;   }
}
    
    if(m1 - m4 > 7.50) {// ignore m2
          m=m-m4; active_moisture_sensors--; }
    else if(m4 - m1 > 7.50){//ignore m1
          m=m-m1; active_moisture_sensors--;
    }
    //if(m2 - m3 > 7.50){m-=m3; active_moisture_sensors--;}
    

if(active_moisture_sensors == 0)  m = 0.00; //avoid dividing by zero
else m = m/active_moisture_sensors;

 return m;

}


unsigned long current_time_in_secs = 0;


unsigned long track_time(){
   pulse =  digitalRead(pulsePin);

// --- 212 loop cycles in just one second --- //

     if(pulse == HIGH && locked == false){  locked = true; first_pass_off = true; }
else if(pulse == LOW) {                     locked = false; first_pass_on = true; }

     if( locked && first_pass_on == true){ first_pass_on = false; second++; trigger_moisture_read++;
     EEPROM.update(time_slot, hour); EEPROM.update((time_slot+1), minute); EEPROM.update((time_slot+2), second); // -- update EEPROM internal system time every time a second changes
     EEPROM.update((time_slot+3), day);  EEPROM.update((time_slot+4), month);  EEPROM.update((time_slot+6), 21); // update EEPROM internal system calendar every time a minute changes

     }
else if(!locked && first_pass_off == true) {first_pass_off = false; second++;
     EEPROM.update(time_slot, hour); EEPROM.update((time_slot+1), minute); EEPROM.update((time_slot+2), second); 
     EEPROM.update((time_slot+3), day);  EEPROM.update((time_slot+4), month);  EEPROM.update((time_slot+6), 21);
     } 
  //Serial.print("Day: ");  Serial.print(day);  Serial.print(" Month: ");  Serial.print(Month);  Serial.print(" Year: "); Serial.println(year);
  //Serial.print("Hour: "); Serial.print(hour); Serial.print(" Minute: "); Serial.print(minute);  Serial.print(" Second: ");  Serial.println(second);


// --- clock
  if(second == 60){ minute++; second = 0; }
  if(minute == 60){ hour++; minute = 0; }
  if(hour == 24)  { day++; hour = 0; time_slot++; EEPROM.update(1000, time_slot); } 
      //since seconds in a day change 86,400 times ... and EEPROM dies at 100,000 writes ... 
      // ||  SHIFT ONE CELL RIGHT || \\



// --- calendar --- //
    if(month == 2){ 
        if(year%4 != 0){if(day == 29){day=0; month++;}} // lunar
        else {if(day == 30){ day=1; month++; }} // leap year
      }
  else  if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12){
        if(day == 31){month++; day = 1;}
    }
  else { //month == April || month == June || month == September || month == November
    if(day == 32){month++; day=1;}
    }

  if(month == 13){year++; month = 1; }

    // shift to the display class{};
      if(month==1) Month = "January";    else if(month==2) Month = "February"; else if(month==3) Month = "March";
 else if(month==4) Month = "April";      else if(month==5) Month = "May";      else if(month==6) Month = "June";
 else if(month==7) Month = "July";       else if(month==8) Month = "August";   else if(month==9) Month = "September";
 else if(month==10)Month = "October";    else if(month==11) Month = "November";     else if(month==12) Month = "Deceomber"; 
//if day has changed 


  current_time_in_secs = (long(3600) * hour) + (long(60) * minute) + (second);
//  current_time_in_secs = (minute * 60) + (second);
//  current_time_in_secs = second;

return current_time_in_secs;

  }



void scan_buttons(){
//is there any high?? 
//by whom? 
//ACTIVE LOW
byte _btn; 

     if(digitalRead(set)==0)  _btn = set; else if(digitalRead(up)==0) _btn = up;
else if(digitalRead(back)==0) _btn = back; else if(digitalRead(Okay)==0) _btn = Okay;
else if(digitalRead(left)==0) _btn = left; else if(digitalRead(down)==0) _btn = down;
else if(digitalRead(right)==0) _btn = right;/*else if(digitalRead(power)) {delay(500); _btn = power;}*/

else _btn = 0;

  

if(_btn != 0) {dim_screen = 0; delay(150);}

//Serial.println(_btn);

eventHandler(_btn);

}



bool plugged1 = false; bool laststate1 = false;
bool plugged2 = false; bool laststate2 = false;
bool plugged3 = false; bool laststate3 = false;
bool plugged4 = false; bool laststate4 = false;



  float humi_temp(const uint8_t indicator){ // the humidity temperature function

active_temp_sensors = 0;

bool isOn = false; 
String ScreenMessage[2] = {"Out", "Out"};
bool in = false, state = false;

if(isOn == false){
    h = sensor.readHumidity(); t = sensor.readTemperature();
  }

else Serial.println("Postponing Reading of DHT sensors");
  
    if(isnan(h)) { h=0.00; t=0.00; ScreenMessage[0] = ScreenMessage[1] =  "Out!";      //
        in = false; state = false;
                 }

  else {  in = true;  active_temp_sensors=1; Serial.print("h = "); Serial.println(h); Serial.print("t = "); Serial.println(t); Serial.println();
     if(in && !state){digitalWrite(indicator, 1); delay(50); digitalWrite(indicator, 0);}
    ScreenMessage[0] = String(h) + "%";   ScreenMessage[1] = String(int(t+0.5)) + "`C"; // 
    }

 
    return t;
    }


void updateScreen(byte * screen, byte * sec){
  
// before updating current screen
  


byte hour_min_sec[3] = {hour, minute, *sec};

  // --- setup mode --- //
       if(*screen == 0){Screen.settings();}
  else if(*screen == 1){Screen.welcome();}
  else if(*screen == 2){Screen.set_time();}
  else if(*screen == 3){Screen.set_date();}
  else if(*screen == 4){Screen.set_crop();}
  else if(*screen == 5){Screen.Irrigation_Curve();}
  else if(*screen == 6){Screen.confirm_changes();}
  else if(*screen == 7){Screen.save_changes();}
  else if(*screen == 32){Screen.configuring();}

 // --- normal mode --- //   
  else if(*screen == 8){
                    message_counter = (EEPROM.read(2005)*10) + (EEPROM.read(2006) * 1);
                    byte date_month_year[3] = {day, month, Y2};
                    Screen.sleep(hour_min_sec, date_month_year, &message_counter);  } // sleep

  else if(*screen == 9){
   /* 1. */        
   /* 2. */       byte average_soil_moisture = int(moisture_now+0.50);
   /* 3. */      // temp = humi_temp(humi_temp_signal);  humi = h; 
   /* 4. */       all_sensors = active_temp_sensors + active_moisture_sensors + active_tank_sensors;
 
byte soil_tank_field[4] = {average_soil_moisture, (byte)water_percent, (byte)temp, (byte)humi};


                    float PowerParam[3] = {run_the.loaded_battery, run_the.panel_OC_voltage, float(run_the.Battery_Percentage)};
                        message_counter = (EEPROM.read(2005)*10) + (EEPROM.read(2006) * 1);
           Screen.Home(hour_min_sec, soil_tank_field, PowerParam[0], PowerParam[1], PowerParam[2], Pumping, irrigationPrompt, &message_counter, &ct_Dwn);} // home

  else if(*screen == 10){Screen.menu();} // menu
  

  else if(*screen == 11){//all_sensors = active_temp_sensors + active_moisture_sensors + active_tank_sensors; i believe you cant reach here minus passing home
                        Screen.Sensors(&all_sensors);}
  else if(*screen == 12){Screen.Moisture_Sensors(&active_moisture_sensors, M_Readings);}
  else if(*screen == 13){Screen.Temp_n_Humidity(&temp, &humi);}
  else if(*screen == 14){Screen.Tank_Level(&water_percent, &latest_irri_vol);}

  else if(*screen == 15){ // --- tank_capacity = SD.read("tank_details.txt"); --- log the SD card tank details
               const float tank_radius = 0.53, tank_height = 1.20;
                              float water_height = 0.00, water_volume = 0.00, Capacity = 0.00;
                              Capacity = 3.14  * tank_radius * tank_radius * tank_height;
                              Capacity *= 1000;
                          // water_height = current_height - top_dead_center//
                              water_height = float(water_percent)/100.00 * tank_height;
                              water_volume = water_height * 3.14 * (tank_radius*tank_radius); //in cubic meters
                              water_volume *= 1000; // in litres

                            Screen.Water_Tank(&water_percent, water_volume, Capacity, water_height, tank_height, tank_radius);} // water_volume
  else if(*screen == 16){ 
  //  String Farm_Details[4];
  //gulawo = SD.open("farm.txt");
  //Farm_Details[0] = Serial.write(myFile.read())
  //   = {String(EEPROM.read(2000)), String(EEPROM.read(2500)), String(EEPROM.read(3000))};
      String Farm_Details[]   = {"Nansana", "50ha", "Beans", "20/3/21"};
      
//cast_card_details();


                     /*    
                     Irrigation_Curve
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
                     */
  // if the file isn't open, pop up an error:
  
    // --- tank_capacity = SD.read("farm_details.txt"); --- log the SD card farm details
    
//
  //data.setRunTime_parameters();
 //  -------------- data.save(); //if time is due for data-logging
  // -------------- data.(); // if the time due for data transmission


//normal mode
// Sensors(uint8_t * moisture_sensors, uint8_t *temp_sensor, uint8_t *tank_level)
                            Screen.Farm(Farm_Details);}

  else if(*screen == 18){ // --- tank_capacity = SD.read("details.txt"); --- log the SD card farmer details
                          String Farmer_Details[] = {"Name", "Location", "Crop", "Irrigation"}; //{String(EEPROM.read(1000)), String(EEPROM.read(1500)), String(EEPROM.read(2000))};
                          Screen.Farmer(Farmer_Details);}

  else if(*screen == 19){ // --- //current_time[] = EEPROM.read("details.txt"); --- log the SD card tank details
                            Screen.Setting();} // --- current time param ---- //// 

  //else if(*screen == 20){Screen.showPower();}
  else if(*screen == 21){Screen.message_sent();}
   //  else if(*screen == 22){Screen.Coffee();}
  //   else if(*screen == 23){Screen.Pineapple();}
 //    else if(*screen == 24){Screen.Jackfruit();}
  else if(*screen == 25){Screen.reset_time();}
  else if(*screen == 26){Screen.time_is_set();}
  else if(*screen == 29){Screen.DataLog();}
  else    *screen = 8;

/*
  
   
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("1. Soil Sensors"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 37);   LCD.print("2. About this Farm"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 51);   LCD.print("3. The Water Tank"); 

  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 23);   LCD.print("4. Field Temperature"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 37);   LCD.print("5. Data Log"); 
  LCD.setFont(u8g_font_helvR08);LCD.setPrintPos(3, 51);   LCD.print("6. Settings"); 

 *   if(scroller == 1) scroller = 11;
    else if(scroller == 2) scroller = 16;
    else if(scroller == 3) scroller = 19;
    else if(scroller == 4) scroller = 10;
    else if(scroller == 5) scroller = 29;
    else if(scroller == 6) scroller = 19;
 */
  
}
/*
if(valve_is_ON) digitalWrite(valvePin, LOW);
else digitalWrite(valvePin, HIGH);
*/
  int menu_select(int selected){
    int jump_to = 10; //default to current screen
         if(selected == 1) jump_to = 11; //to sensors
    else if(selected == 2) jump_to = 15; //to tank
    else if(selected == 3) jump_to = 16; //to 
    else if(selected == 4) {if(!valve_is_ON) {digitalWrite(valvePin, LOW); valve_is_ON=true;} else {digitalWrite(valvePin, HIGH); valve_is_ON=false;} jump_to = 10;}
    else if(selected == 5) {if(!pump_is_ON)  {digitalWrite(pumpPin, LOW); pump_is_ON=true;} else {digitalWrite(pumpPin, HIGH); pump_is_ON=false;}  jump_to = 10;}
    else if(selected == 6) jump_to = 19;
    return jump_to;
    }

int eventHandler(byte _btn){


// isset = true;
   // EEPROM.write(setting_key_address, isset);
if(_btn != 0){

if(current_Screen == 0){  // --- settings screen
  switch(_btn){
            case set:  tone(sound_fx, 880, 50); current_Screen = 2;   break;
            case back: tone(sound_fx, 123, 30); current_Screen = 8;  break;
            case up: case down: case left: case right: tone(sound_fx, 250, 80); break;
            
    }
  }
                   
else if(current_Screen == 2){ // --- setting the time
        switch(_btn){
            case set: current_Screen = 3; tone(sound_fx, 880, 30);   break;
            case back: current_Screen = 0; tone(sound_fx, 123, 30); break;
            case up:    if(Screen.shift == 1){if(Screen.temp_hr<23){Screen.temp_hr++;} else{Screen.temp_hr=0;}} 
                        else { if(Screen.temp_min<59){Screen.temp_min++;} else{Screen.temp_min=0;}} Screen.changed = true; break;
            case down:  if(Screen.shift == 1){if(Screen.temp_hr>0){Screen.temp_hr--;}else {Screen.temp_hr=23;}} 
                        else { if(Screen.temp_min>0){Screen.temp_min--;} else{Screen.temp_min=59;}}  Screen.changed = true;   break;
            case left: Screen.shift = 1; break;
            case right: Screen.shift = 2;  break;
            //case power : current_Screen = SleepNow;  break;
     }
   }


 else if(current_Screen == 3){ // ---- setting the date
        switch(_btn){
            case set: current_Screen = 4; tone(sound_fx, 880, 30);  break;
            case back: current_Screen = 2; tone(sound_fx, 123, 30); break;
            case up:         if(Screen.shifta == 1){ //date shifter
                                     if(Screen.temp_month == 2 && (Screen.temp_year%4!=0)) { if(Screen.temp_date<28){Screen.temp_date++;} else{Screen.temp_date=1;}}
                                else if(Screen.temp_month == 2 && (Screen.temp_year%4==0)) { if(Screen.temp_date<29){Screen.temp_date++;} else{Screen.temp_date=1;}}     
                                else if(Screen.temp_month == 1 || Screen.temp_month == 3 || Screen.temp_month == 5 || Screen.temp_month == 7 || Screen.temp_month == 8 || Screen.temp_month == 10 || Screen.temp_month == 12){
                                        if(Screen.temp_date<31){Screen.temp_date++;} else{Screen.temp_date=1;} }
                                else if(Screen.temp_month ==  4 || Screen.temp_month == 6 || Screen.temp_month == 9 || Screen.temp_month == 11){
                                        if(Screen.temp_date<30){Screen.temp_date++;} else{Screen.temp_date=1;} }
                                } 
                                
                        else if(Screen.shifta == 2) { //month shifter 
                          if(Screen.temp_month<12){Screen.temp_month++;} else{Screen.temp_month=1;}}  
                          
                        else { // year shifter
                          if(Screen.temp_year<2030){Screen.temp_year++;} else{Screen.temp_year=2010;}} Screen.changed = true; break;
                          
            case down:         if(Screen.shifta == 1){ //date shifter
                                     if(Screen.temp_month == 2 && (Screen.temp_year%4!=0)) { if(Screen.temp_date>1){Screen.temp_date--;} else{Screen.temp_date=28;}}
                                else if(Screen.temp_month == 2 && (Screen.temp_year%4==0)) { if(Screen.temp_date>1){Screen.temp_date--;} else{Screen.temp_date=29;}}     
                                else if(Screen.temp_month == 1 || Screen.temp_month == 3 || Screen.temp_month == 5 || Screen.temp_month == 7 || Screen.temp_month == 8 || Screen.temp_month == 10 || Screen.temp_month == 12){
                                        if(Screen.temp_date>1){Screen.temp_date--;} else{Screen.temp_date=31;} }
                                else if(Screen.temp_month ==  4 || Screen.temp_month == 6 || Screen.temp_month == 9 || Screen.temp_month == 11){
                                        if(Screen.temp_date>1){Screen.temp_date--;} else{Screen.temp_date=30;} }
                                } 
                                
                        else if(Screen.shifta == 2) { //month shifter 
                          if(Screen.temp_month>1){Screen.temp_month--;} else{Screen.temp_month=12;}}  
                          
                        else { // year shifter
                          if(Screen.temp_year>2010){Screen.temp_year--;} else{Screen.temp_year=2030;}}Screen.changed = true; break;

                          
            case left:  if(Screen.shifta == 1){Screen.shifta = 3;} else Screen.shifta--; break;
            case right: if(Screen.shifta == 3) {Screen.shifta = 1;} else Screen.shifta++;  break;
            //case power : current_Screen = SleepNow; break;
  }
}

                
else if(current_Screen == 4){ // --- set crop
        switch(_btn){
            case set:  current_Screen = 5; tone(sound_fx, 880, 30);   break;
            case back: current_Screen = 3; tone(sound_fx, 123, 30); break;
            case up:    if(Screen.selekta == 1) Screen.selekta = 3; else Screen.selekta--;  break;
            case down:  if(Screen.selekta == 3) Screen.selekta = 1; else Screen.selekta++;      break;
            case left:  case right :break;
            case Okay:  break;
           // case power : current_Screen = SleepNow;  break;
  }
}

             
else if(current_Screen == 5){ // --- set speed
        switch(_btn){
            case set:  current_Screen = 6; tone(sound_fx, 880, 30);    break;
            case back: current_Screen = 4; tone(sound_fx, 123, 30); break;
            case up:   case down:  break;
            case left:  if(Screen.move_box == 1) Screen.move_box = 3; else Screen.move_box--; break;
            case right: if(Screen.move_box == 3) Screen.move_box = 1; else Screen.move_box++; break;
            case Okay:  break;
          //  case power : current_Screen = SleepNow;  break;
  }
}

          
else if(current_Screen == 6){ // --- confirm
        switch(_btn){
            case set:  current_Screen = 7; tone(sound_fx, 880, 30);    break;
            case back: current_Screen = 5; tone(sound_fx, 123, 30); break;
            case up:   case down:  break; case left : case right : tone(sound_fx, 100, 50); break;
            case Okay:  break;
          //  case power : current_Screen = SleepNow;  break;
  }
}

          
else if(current_Screen == 7){ // --- save changes
        switch(_btn){
            case set: case Okay: current_Screen = 32;   tone(sound_fx, 880, 30);   break;
            case back: current_Screen = 6; tone(sound_fx, 123, 30); break;
            case up:   case down:  break;
            case left:  if(Screen.move_boxa == 1) Screen.move_boxa = 2; else Screen.move_boxa--; break;
            case right: if(Screen.move_boxa == 2) Screen.move_boxa = 1; else Screen.move_boxa++; break;
        //    case power : current_Screen = SleepNow;  break;
  }
}


else if(current_Screen == 32){
  //do nothing or maybe just go back
  save = true;
}

      
else if(current_Screen == 8){ // --- sleep screen
        switch(_btn){
            case set:  case Okay: current_Screen = 9;  dim_screen = 0;  tone(sound_fx, 600, 50); break;
            case back: current_Screen = SleepNow; tone(sound_fx, 123, 30); break;
            case up:   case down:  case left: case right : tone(sound_fx, 100, 50); break;
  }
}
        
else if(current_Screen == 9){  // --- home screen
        switch(_btn){
            case set:  current_Screen = 10;   dim_screen = 0; tone(sound_fx, 880, 30); break;
            case back: if(started == false) current_Screen = SleepNow; else started = false;   break;//if(valve_is_ON || pump_is_ON)
            case up:   if(Screen.scroll == 1) Screen.scroll=1; else Screen.scroll--; tone(sound_fx, 880, 10);break;
            case down: if(Screen.scroll == 3) Screen.scroll=3; else Screen.scroll++; tone(sound_fx, 880, 10); break;  
            case Okay: current_Screen = 10;  break; // temporarily start the burner fan speed regulation
            case left: case right : tone(sound_fx, 330, 30); break;
         //   case power: current_Screen = 50; break;
  }
}

       
else if(current_Screen == 10){ // --- menu screen
        switch(_btn){
            case set:  case Okay: 
                current_Screen = menu_select(Screen.dj_selekta);  /*if(Startd flip back to stop)*/  break;
            case back:  current_Screen = 9; /*if(cancelled == false)*/ break;
            
            case up:   if(Screen.dj_selekta == 1) Screen.dj_selekta = 6; else Screen.dj_selekta--;  break;
            case down: if(Screen.dj_selekta == 6) Screen.dj_selekta = 1; else Screen.dj_selekta++; break; 
          //  case power :  current_Screen = SleepNow; break;
            //case left: case right : break;
            
  }
}

else if(current_Screen == 11){ // --- sensors screen
        switch(_btn){
            case set:  case Okay: current_Screen = (selected_menu_option==1?12:(selected_menu_option==2)?13:14); tone(sound_fx, 740, 50);   break;
            case back:  current_Screen = 10;  tone(sound_fx, 123, 30); break;
            
            case up:   if(selected_menu_option == 1) selected_menu_option++; else selected_menu_option--; break;
            case down: if(selected_menu_option == 3) selected_menu_option = 1; else selected_menu_option++; break; 
          //  case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}


    
else if(current_Screen == 12){ // --- moisture screen
        switch(_btn){
            case set:  case Okay:  break; // 
            case back:  current_Screen = 11;  tone(sound_fx, 123, 30); break;
            
            case up:    break;
            case down:  break; 
         //   case power :  current_Screen = SleepNow; break;
            
  }
}
 

     
else if(current_Screen == 13){ // --- temp screen
        switch(_btn){
            case set:  case Okay:     break;
            case back:  current_Screen = 11;  tone(sound_fx, 123, 30); break;
            
            case up:    break;
            case down:  break; 
           // case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}

    
else if(current_Screen == 14){ // --- humidity screen
        switch(_btn){
            case set:  case Okay:    break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 11;  tone(sound_fx, 123, 30); break;
            
            case up:    break;
            case down:   break; 
          //  case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}


else if(current_Screen == 15){ // --- Power Status screen
        switch(_btn){
            case set:  case Okay: exec.check_tank(tank_level_sensorPin);   break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 10;  tone(sound_fx, 123, 30); break;
           
         //   case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }}

else if(current_Screen == 16){ // --- Power Status screen
        switch(_btn){
            case set:  case Okay:    break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 10;  tone(sound_fx, 123, 30); break;
           
        //    case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }}

  
      
else if(current_Screen == 19){ // ---setting() screen
        switch(_btn){
            case set:  
            case Okay:  
                        if(Screen.set_option==1){Screen.temp_hr = hour;  Screen.temp_min = minute; current_Screen=25;} 
                        else if(Screen.set_option==2) {sending = true; data.send_settings();}
                        else {/*sending = true;*/ data.send_data();} break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 10;  tone(sound_fx, 123, 30); break;
            
            case up:   if(Screen.set_option == 1) Screen.set_option=3; else Screen.set_option--;  break;
            case down: if(Screen.set_option == 3) Screen.set_option=1; else Screen.set_option++; break; 
        //    case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}


else if(current_Screen == 20){ // --- Power Status screen
        switch(_btn){
            case set:  case Okay:    break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 10;  tone(sound_fx, 123, 30); break;
           
         //   case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}


else if(current_Screen == 21){ // --- Send Message screen
        switch(_btn){
            case set:  case Okay: break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 19;  tone(sound_fx, 123, 30); break;
           
         //   case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}


else if(current_Screen == 22){ // --- Send Message screen
        switch(_btn){
            case set:  case Okay:    break; 
            case back:  current_Screen = 15;  tone(sound_fx, 123, 30); break;
           
        //    case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}


else if(current_Screen == 23){ // --- Send Message screen
        switch(_btn){
            case set:  case Okay:  break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 15;  tone(sound_fx, 123, 30); break;
           
         //   case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}

else if(current_Screen == 24){ // --- Send Message screen
        switch(_btn){
            case set:  case Okay:    break; 
            case back:  current_Screen = 15;  tone(sound_fx, 123, 30); break;
           
          //  case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}

                
else if(current_Screen == 25){ // --- setting the time ... either update EEPROM or modify the superglobal time variables
        switch(_btn){ // UPDATE the latest time_slot
            case set: EEPROM.update(time_slot, Screen.temp_hr); EEPROM.update(time_slot+1, Screen.temp_min); EEPROM.update(time_slot+2, 0); current_Screen = 26; tone(sound_fx, 880, 30);  break;
            case back: current_Screen = 19; tone(sound_fx, 123, 30); break;
            case up:    if(Screen.shift_right == 1){if(Screen.temp_hr<23){Screen.temp_hr++;} else{Screen.temp_hr=0;}} 
                        else { if(Screen.temp_min<59){Screen.temp_min++;} else{Screen.temp_min=0;}} Screen.changed = true; break;
            case down:  if(Screen.shift_right == 1){if(Screen.temp_hr>0){Screen.temp_hr--;}else {Screen.temp_hr=23;}} 
                        else { if(Screen.temp_min>0){Screen.temp_min--;} else{Screen.temp_min=59;}}  Screen.changed = true;   break;
            case left: Screen.shift_right = 1; break;
            case right: Screen.shift_right = 2;  break;
         //   case power : current_Screen = SleepNow;  break;
  }
}



else if(current_Screen == 26){ // --- Time has been set screen --- SAVE IT TO EEPROM
        switch(_btn){
            case set:  case Okay: hour = Screen.temp_hr; minute = Screen.temp_min; second = 0; current_Screen=9;  break; //goHome if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen=25;  tone(sound_fx, 123, 30); break;
           
         //   case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
  }
}

 
else if(current_Screen == 29){ // --- Crops screen
        switch(_btn){
            case set:  case Okay: current_Screen = (Screen.kong==1?22:(Screen.kong==2?23:24));  break; // if backlit_state == false{digitalWrite(back_light, HIGH);}
            case back:  current_Screen = 10;  tone(sound_fx, 123, 30); break;
            
            case up:   if(Screen.kong == 1) Screen.kong=3; else Screen.kong--;  break;
            case down: if(Screen.kong == 3) Screen.kong=1; else Screen.kong++;  break; 
         //   case power :  current_Screen = SleepNow; break;
            //case left: case right :  break;
            
    }
  }

else if(current_Screen == 50){ // shut down screen
  switch(_btn){
    
    }
  }
}

return current_Screen;

}
