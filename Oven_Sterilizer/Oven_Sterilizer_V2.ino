#include <LiquidCrystal_I2C.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>

const float high_temp = 77;
const float low_temp = 76;
const int buttonPin = 5;     // the number of the pushbutton pin
const int gun_a = 2;
const int gun_b = 4;
float tempC = 0;
int buttonEndState;
int buttonStartState;         // variable for reading the pushbutton status 
int state =1;
int timeCount;
String convertedTemp;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Weather sensor;
Weather sensor_upper_chamber;

void setup() 
{
  pinMode(gun_a, OUTPUT);
  pinMode(gun_b, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, 0);
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  //Initialize the I2C sensor and ping it
  sensor.begin();   
}

void loop() 
{
  if (state == 1)
    waiting();
  else if (state == 2)
    preheat();
  else if (state == 3)
    sterilizing_gun_on();
  else if (state == 4)
    sterilizing_gun_off();
  delay(1000);
}

//State 1
void waiting()
{
 //Set LCD to Push Button to Start
  updateLCD("PUSH THE BUTTON", "TO START");
  buttonStartState = digitalRead(buttonPin);
  delay(1000);
  buttonEndState = digitalRead(buttonPin);
  if(buttonStartState != buttonEndState){
    digitalWrite(buttonPin, 0);
    state =2;
  } else
      waiting();  
    
}

//State 2
void preheat()
{
   float checkedTemp = (getTemperature(),0);
   convertedTemp =  String(checkedTemp);
   updateLCD("Preheating..", convertedTemp);
   //Checking temp from sensor vs set low temperature
   if(checkedTemp > low_temp ){
     //Switching State
     state =3;
     timeCount = 1800;   
   } else
       turnOnGuns();
     
}

//state 3
void sterilizing_gun_on()
{
  //Checking temp once per second from the main loop
  if(timeCount > 0){
    float checkedTemp = (getTemperature(),0);
    convertedTemp =  String(checkedTemp);
    updateLCD("Inactivating...", convertedTemp + " " + F("\xDF""C.") + " " + "Timer:" + convertTime(timeCount) );
    timeCount = timeCount - 1;
    if(checkedTemp > high_temp){
      turnOffGuns(); 
      state = 4;  //Transition to state 4
    }
  } else {
       turnOffGuns();
       updateLCD("Finished"," ");
       state = 1; //Back to State 1
    }
}

//state 4
void sterilizing_gun_off()
{
  //Checking temp once per second from the main loop
  if(timeCount > 0){
    float checkedTemp = (getTemperature(),0);
    convertedTemp =  String(checkedTemp);
    updateLCD("Inactivating...", convertedTemp + " " + F("\xDF""C.") + " " + "Timer:" + convertTime(timeCount) );
    timeCount = timeCount - 1;
    if(checkedTemp <low_temp){  
      turnOnGuns(); 
      //Transition to state 3
      state = 3;
    }
  } else {
       turnOffGuns(); 
       updateLCD("Finished"," ");
       state = 1; //Back to State 1
    }
}

//Function converts time into Minutes and Seconds
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

//Turn Off Guns
void turnOffGuns()
{
  digitalWrite(gun_a, LOW);
  digitalWrite(gun_b, LOW);
}

//Turn On Guns
void turnOnGuns()
{
  digitalWrite(gun_a, HIGH);
  digitalWrite(gun_b, HIGH);
}

//Extracts temperature from the Si7021 Sensor
float getTemperature()
{
  tempC = sensor.getTemp();
  Serial.println(tempC,0);
  return tempC;
}

String updateLCD(String line1, String line2)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
