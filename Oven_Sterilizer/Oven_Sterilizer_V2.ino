#include "SparkFun_Si7021_Breakout_Library.h"
#include "Countimer.h"
#include <SPIFFS.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


int timeCount;
int current_gun;
int buttonState;         // variable for reading the pushbutton status
int buttonStartState;         // variable for reading the pushbutton status
int buttonEndState;         // variable for reading the pushbutton status
int state =1;
const float low_temp = 63;
const float high_temp = 65;
float tempC = 0;
const int gun_a = 2;
const int gun_b = 4;
const int buttonPin = 5;     // the number of the pushbutton pin

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Weather sensor;


void setup() {
  pinMode(gun_a, OUTPUT);
  pinMode(gun_b, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, 0);
  Serial.begin(9600);
  //Initialize the I2C sensor and ping it
  sensor.begin();
  
}

void loop() {
  if (state == 1){
    waiting();
  }

  else if (state == 2){
    preheat();
  }

  else if (state == 3){
    sterilizing();
  }
  delay(1000);
}

//State 1
void waiting(){
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
  if (buttonStartState != buttonEndState){
    Serial.println("Button was pressed. Toggle Oven state");
    state =2;
  }
  else{
    waiting();  
    }
}

//State 2
void preheat(){
   
   digitalWrite(buttonState, HIGH);
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Preheating");
   lcd.setCursor(1,1);
   lcd.print("In Process...");
   
   if(getTemperature() > low_temp ){
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
void sterilizing(){
  
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Sterilizing ...");

  if(timeCount > 0){
    timeCount = timeCount - 1;

    //Displaying the time remaining
    Serial.println(convertMins(timeCount));
    Serial.println(convertSecs(timeCount));
    lcd.setCursor(1,1);
    lcd.print("Timer : ");
    lcd.print(convertMins(timeCount));
    lcd.print(":");
    lcd.print(convertSecs(timeCount));
  }

  else {
    state = 1;
   //Turning on Both Guns
   digitalWrite(gun_a, LOW);
   digitalWrite(gun_b, LOW);
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Finished");
   lcd.setCursor(1,1);
   lcd.print("Sterilizing :)");
  }
}

float getTemperature(){
  //Get readings from Sensor
  //Measure Temperature from Si7021
  tempC = sensor.getTemp();
  Serial.println(tempC);
  return tempC;
  
}

//Reading from the file function

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
  file.close();
}

//Writing in the file function
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

int convertMins(int current_time){ 
  int mins = current_time/60;
  return (mins);
}

int convertSecs(int current_time){
  int secs = current_time%60;
  return (secs);
}


//  Ignore this section for now
//  
        // Initialize SPIFFS
//  #ifdef ESP32
//    if(!SPIFFS.begin(true)){
//      Serial.println("An Error has occurred while mounting SPIFFS");
//      return;
//    }
//  #else
//    if(!SPIFFS.begin()){
//      Serial.println("An Error has occurred while mounting SPIFFS");
//      return;
//    }
//  #endif
//  //Reading in memory last gun that was saved
//  int current_gun = readFile(SPIFFS, "/gun.txt").toInt();
//
//  //checking if it is empty
//  if(current_gun == NULL){
//    current_gun = gun_a;
//    String gun_now = String(current_gun);
//    writeFile(SPIFFS, "/gun.txt", gun_now.c_str());
//    }
//
//         //If it was the second gun then turn it off
//        else if(current_gun == 4){
//
//            //Turn off gun second off
//            digitalWrite(current_gun, LOW);
//          
//             while(getTemperature() >= low_temp && getTemperature() <= high_temp ){
//              //Turn off first gun 
//               digitalWrite(2, LOW);
//             }
//              //Checking If temperature is below range
//              if(getTemperature() <= low_temp)
//              {
//                  //Then Turn on second gun until range temperature
//                  while(getTemperature() <= high_temp ){
//                   digitalWrite(current_gun, HIGH); 
//                  }
//
//                  // turn off second gun if the reaches range temperatue
//                  digitalWrite(current_gun, LOW);
//                  
//                  //changing current gun with idle gun 
//                  current_gun = 2;
//                  String gun_now = String(current_gun);
//                  writeFile(SPIFFS, "/gun.txt", gun_now.c_str());
//                  current_gun = readFile(SPIFFS, "/gun.txt").toInt();
//              }
//            }
//
//              //End of while loop
//              //Shutdown Operation
//              digitalWrite(gun_a, LOW);
//              digitalWrite(gun_b, LOW);
//                lcd.clear();
//               lcd.setCursor(1,0);
//               lcd.print("FINISHED");
//               lcd.setCursor(1,1);
//               lcd.print("Sterilizing :)");
//               delay(5000);
//              //Back to state 1
//              state = 1;
//              waiting();  
            
