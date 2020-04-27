#include <LiquidCrystal_I2C.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h> 

//Inputs and outputs
int firing_pin = 32;
int zero_cross = 33;

LiquidCrystal_I2C lcd(0x27,20,4);  //sometimes the adress is not 0x27. Change to 0x3f if it dosn't work.
Weather sensor;

//Variables
int state =1;
int last_CH1_state = 0;
int timeCount;
String convertedTemp;
bool zero_cross_detected = false;
const int maximum_firing_delay = 9000;
int firing_delay = maximum_firing_delay;
// Max firing delay set to 9ms based on AC frequency of 50Hz
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;
int temp_read_Delay = 500;
float real_temperature = 0;
int setpoint = 45;
//PID variables
float PID_error = 0;
float previous_error = 0;
float elapsedTime, Time, timePrev;
int PID_value = 0;
//PID constants
int kp = 203;   int ki= 7.2;   int kd = 1.04;
int PID_p = 0;    int PID_i = 0;    int PID_d = 0;

//Zero Crossing Interrupt Function
void IRAM_ATTR zero_crossing()
{
  //If the last state was 0, then we have a state change...
  if(last_CH1_state == 0) 
    zero_cross_detected = true; //We have detected a state change! We need both falling and rising edges
  //If pin 8 is LOW and the last state was HIGH then we have a state change      
  else if(last_CH1_state == 1){ 
    zero_cross_detected = true;    //We have detected a state change!  We need both falling and rising edges.
    last_CH1_state = 0;            //Store the current state into the last state for the next loop
  }   
}

void setup() {
  //Define the pinsmaximum_firing_delay - PID_value
  pinMode (firing_pin,OUTPUT); 
  pinMode (zero_cross,INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(zero_cross), zero_crossing, CHANGE);
  lcd.begin(); //Begin the LCD communication through I2C
  lcd.backlight();  //Turn on backlight for LCD
  //inititalize the I2C the sensor and bing it
  sensor.begin();
  Serial.begin(9600);
}


void loop() 
{   
  if (state == 1)
    waiting();
  else if (state == 2)
    preheat();
  else if (state == 3)
    inactivating();
}
//End of void loop

//State 1
void waiting()
{
  lcd.begin();
  lcd.backlight();
 //Set LCD to Push Button to Start
  updateLCD("PUSH THE BUTTON", "TO START");    
}

void preheat()
{
   int checkedTemp = (getTemperature(),0);
   convertedTemp =  String(checkedTemp);
   updateLCD("Preheating...", convertedTemp + "" +"/"+""+ setpoint +"" +F("\xDF""C"));
   //Checking temp from sensor vs set low temperature
   if(checkedTemp >= setpoint ){
     //Switching State
     state =3;
     timeCount = 1800;   
   } else
       temperature_control();    
}

void inactivating()
{
    //Checking temp once per second from the main loop
  if(timeCount > 0){
    int checkedTemp = (getTemperature(),0);
    convertedTemp =  String(checkedTemp);
    updateLCD("Inactivating...", convertedTemp + "" + F("\xDF""C.") + "" + "Timer:" + convertTime(timeCount) );
    timeCount = timeCount - 1;
    temperature_control();
  } else {
       updateLCD("Finished"," ");
       state = 1; //Back to State 1
    }
}

void temperature_control()
{
    currentMillis = millis();           //Save the value of time before the loop
  // We create this if so we will read the temperature and change values each "temp_read_Delay"
  if(currentMillis - previousMillis >= temp_read_Delay){
    previousMillis += temp_read_Delay;              //Increase the previous time for next loop
    real_temperature = (getTemperature());  //get the real temperature in Celsius degrees
    Serial.println(getTemperature());
    PID_error = setpoint - real_temperature;        //Calculate the pid ERROR
    if(PID_error > 30)                              //integral constant will only affect errors below 30ºC             
      PID_i = 0;
    PID_p = kp * PID_error;                         //Calculate the P value
    PID_i = PID_i + (ki * PID_error);               //Calculate the I value
    timePrev = Time;                    // the previous time is stored before the actual time read
    Time = millis();                    // actual time read
    elapsedTime = (Time - timePrev) / 1000;   
    PID_d = kd*((PID_error - previous_error)/elapsedTime); //Calculate the D value
    PID_value = PID_p + PID_i + PID_d; //Calculate total PID value
    //We define firing delay range between 0 and 7400. Read above why 7400!!!!!!!
    if(PID_value < 0)      
      PID_value = 0;       
    if(PID_value > maximum_firing_delay)      
      PID_value = maximum_firing_delay;    
    previous_error = PID_error; //Remember to store the previous error.
  }
  //If the zero cross interruption was detected we create the 100us firing pulse  
  if (zero_cross_detected){
      delayMicroseconds(maximum_firing_delay - PID_value); //This delay controls the power
      digitalWrite(firing_pin,HIGH);
      delayMicroseconds(100);
      digitalWrite(firing_pin,LOW);
      zero_cross_detected = false;
  } 
}

//Extracts temperature from the Sensor
float getTemperature()
{
  //Measure Temperature from Si7021
  int tempC = sensor.getTemp();
  Serial.println(tempC,0);
  return tempC;
}

String convertTime(int currentTime)
{
  int mins = currentTime/60;
  int secs = currentTime%60;
  String TimerSec;
  String TimerMin;
  String s_min = String(mins);
  String s_sec = String(secs);
//Adding Zero to Minutes that are less than 10
  if(mins < 10)
    TimerMin = "0"+s_min;
  else
    TimerMin = s_min;   
//Adding Zero to Seconds that are less than 10
  if(secs < 10)
    TimerSec = "0"+s_sec;
  else
    TimerSec = s_sec;     
  return(TimerMin + ":" + TimerSec);
}

String updateLCD(String line1, String line2)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
