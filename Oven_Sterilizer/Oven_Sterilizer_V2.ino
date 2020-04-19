#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


int timeCount;
int current_gun;
int buttonStartState;         // variable for reading the pushbutton status
int buttonEndState;         // variable for reading the pushbutton status
int state =1;
int activeGun;
float tempC = 0;
const float low_temp = 75;
const float high_temp = 77;
const int gun_a = 2;
const int gun_b = 4;
const int buttonPin = 5;     // the number of the pushbutton pin

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Weather sensor;


void setup() 
{
  pinMode(gun_a, OUTPUT);
  pinMode(gun_b, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, 0);
  activeGun = gun_a;
  Serial.begin(9600);
  //Initialize the I2C sensor and ping it
  sensor.begin();
  
}

void loop() 
{
  if (state == 1){
    waiting();
  }

  else if (state == 2){
    preheat();
  }

  else if (state == 3){
   sterilizing_gun_on();
  }

    else if (state == 4){
   sterilizing_gun_off();
  }
  delay(1000);
}

//State 1
void waiting()
{
 //Set LCD to Push Button to Start
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("PUSH THE BUTTON");
  lcd.setCursor(1,1);
  lcd.print("TO START");
  buttonStartState = digitalRead(buttonPin);
  delay(1000);
  buttonEndState = digitalRead(buttonPin);
  if(buttonStartState != buttonEndState){
    //Serial.println("Button was pressed. Toggle Oven state");
    digitalWrite(buttonPin, 0);
    state =2;
  } else{
      waiting();  
    }
}

//State 2
void preheat()
{
   float checkedTemp = getTemperature();
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Preheating...");
   lcd.setCursor(4,1);
   lcd.print(checkedTemp,0);
   lcd.print(F("/77\xDF""C"));
   
   //Checking temp from sensor vs set low temperature
   if(checkedTemp > low_temp ){
   //Switching State
   state =3;
   timeCount = 1800;   
   }
   else{
   //Turning on Both Guns
   digitalWrite(gun_a, HIGH);
   digitalWrite(gun_b, HIGH);
   }
}

//state 3
void sterilizing_gun_on(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inactivating.... ");
  lcd.setCursor(0,1);
 
  //Checking temp once per second from the main loop
  if(timeCount > 0){
    //get temperature from the sensor
    float checkTemp = getTemperature();
    //Display Temperature
    lcd.print(checkTemp,0);
    lcd.print(F("\xDF""C."));
    timeCount = timeCount - 1;
    
    //Displaying the time remaining
    lcd.print("Timer:");

    //Adding zero to minutes to appear as two digits
    if(convertMins(timeCount) < 10)
    {
      lcd.print("0");
      lcd.print(convertMins(timeCount));
    } else{
    lcd.print(convertMins(timeCount));
    }

    //Adding zero to Seconds to appear as two digits
    lcd.print(":");
    if(convertSecs(timeCount) < 10)
    {
      lcd.print("0");
      lcd.print(convertSecs(timeCount));
    } else{
    lcd.print(convertSecs(timeCount));
    }

    //
    if(checkTemp > high_temp){
      digitalWrite(gun_a, LOW);
      digitalWrite(gun_b, LOW); 
      
      //Transition to state 4
      state = 4;
    }
    
  } else {
       //Turning on Both Guns
       digitalWrite(gun_a, LOW);
       digitalWrite(gun_b, LOW);
       lcd.clear();
       lcd.setCursor(1,0);
       lcd.print("Finished");
       lcd.setCursor(1,1);
       lcd.print("Sterilizing :)");
       
       //Back to State 1
       state = 1;
    }
}

//state 4
void sterilizing_gun_off(){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inactivating.... ");
  lcd.setCursor(0,1);
  //Checking temp once per second from the main loop

  if(timeCount > 0){

    //get temperature from the sensor
    float checkTemp = getTemperature();
    //Display Temperature
    lcd.print(checkTemp,0);
    lcd.print(F("\xDF""C."));
    timeCount = timeCount - 1;
    
    //Displaying the time remaining
    lcd.print("Timer:");
    
    //Adding zero to minutes to appear as two digits
    if(convertMins(timeCount) < 10)
    {
      lcd.print("0");
      lcd.print(convertMins(timeCount));
    } else{
    lcd.print(convertMins(timeCount));
    }

    //Adding zero to Seconds to appear as two digits
    lcd.print(":");
    if(convertSecs(timeCount) < 10)
    {
      lcd.print("0");
      lcd.print(convertSecs(timeCount));
    } else{
    lcd.print(convertSecs(timeCount));
    }
    
    if(checkTemp <low_temp){  
      digitalWrite(gun_a, HIGH);  
      digitalWrite(gun_b, HIGH); 
      //Transition to state 3
      state = 3;
    }
  }
  //end timeCount condition
  
  else {
   //Turning on Both Guns
   digitalWrite(gun_a, LOW);
   digitalWrite(gun_b, LOW);
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Finished");
   lcd.setCursor(1,1);
   lcd.print("Sterilizing :)");

   //Back to State 1
    state = 1;
  }
}

//Function converts time into Minutes
int convertMins(int current_time)
{ 
  int mins = current_time/60;
  return (mins);
}

//Converts time into Seconds
int convertSecs(int current_time)
{
  int secs = current_time%60;
  return (secs);
}

//Extracts temperature from the Sensor
float getTemperature()
{
  //Measure Temperature from Si7021
  tempC = sensor.getTemp();
  Serial.println(tempC,0);
  return tempC;
  
}
