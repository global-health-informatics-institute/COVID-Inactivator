#include "SparkFun_Si7021_Breakout_Library.h"
#include <SPIFFS.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int current_gun;
int buttonState;         // variable for reading the pushbutton status
int state =1;
float low_temp = 63;
float high_temp = 65;
float tempC = 0;
const int gun_a = 2;
const int gun_b = 4;
const int buttonPin = 14;     // the number of the pushbutton pin
unsigned long period = 1800000; //the amount of time to complete single sterilizing , 30 minutes
unsigned long start_time;
unsigned long timer;
unsigned long tseconds;
unsigned long tminutes;
unsigned long seconds;


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

  if (state == 2){
    
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
  state = 1;
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    digitalWrite(buttonState, HIGH);
    Serial.println(buttonState);
    delay(500);
    state =2;
    //preheat();
  } else{
    waiting();
  }
  
}

//State 2
void preheat(){
   state =2;
   digitalWrite(buttonState, HIGH);
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Preheating");
   lcd.setCursor(1,1);
   lcd.print("In Process...");
   
   while(getTemperature() < low_temp ){
   //Turning on Both Guns
   digitalWrite(gun_a, HIGH);
   digitalWrite(gun_b, HIGH);
   }
   sterilizing();
}

void sterilizing(){
   state =3;
   
        // Initialize SPIFFS
  #ifdef ESP32
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Sterilizing ...");
  
  for( start_time = millis();  (millis()-start_time) < (period - 1000);  ){

          Serial.println((period - start_time));
          timer = period - millis();
          tseconds = timer / 1000;
          tminutes = tseconds / 60;
          seconds = tseconds % 60;
          lcd.setCursor(1,1);
          lcd.print("Timer : ");
          lcd.print(tminutes);
          lcd.print(":");
          lcd.print(seconds);
          int current_gun = readFile(SPIFFS, "/gun.txt").toInt();
          if(current_gun == NULL){
                Serial.println((period - start_time));
                timer = period - millis();
                tseconds = timer / 1000;
                tminutes = tseconds / 60;
                seconds = tseconds % 60;
                
                lcd.setCursor(1,1);
                lcd.print("Timer : ");
                lcd.print(tminutes);
                lcd.print(":");
                lcd.print(seconds);
                current_gun = gun_a;
                String gun_now = String(current_gun);
                writeFile(SPIFFS, "/gun.txt", gun_now.c_str());
            }
           
          else if(current_gun == 2){
            //turn off first gun
            Serial.println((period - start_time));
                timer = period - millis();
                tseconds = timer / 1000;
                tminutes = tseconds / 60;
                seconds = tseconds % 60;
                 
                lcd.setCursor(1,1);
                lcd.print("Timer : ");
                lcd.print(tminutes);
                lcd.print(":");
                lcd.print(seconds);
                digitalWrite(current_gun, LOW);
          
            while(getTemperature() >= low_temp && getTemperature() <= high_temp){
                //turn off second gun
                digitalWrite(4, LOW);
                Serial.println((period - start_time));
                timer = period - millis();
                tseconds = timer / 1000;
                tminutes = tseconds / 60;
                seconds = tseconds % 60;
                 
                lcd.setCursor(1,1);
                lcd.print("Timer : ");
                lcd.print(tminutes);
                lcd.print(":");
                lcd.print(seconds);
             }
              
              if(getTemperature() <= low_temp )
              {
                  while(getTemperature() <= high_temp){
                   digitalWrite(current_gun, HIGH); 
                   Serial.println((millis()-start_time));
                   timer = period - millis();
                   tseconds = timer / 1000;
                   tminutes = tseconds / 60;
                   seconds = tseconds % 60;
                   lcd.setCursor(1,1);
                   lcd.print("Timer : ");
                   lcd.print(tminutes);
                   lcd.print(":");
                   lcd.print(seconds);
                  }
                  digitalWrite(current_gun, LOW);
                  //changing current gun with idle gun 
                  current_gun = 4;
                  String gun_now = String(current_gun);
                  writeFile(SPIFFS, "/gun.txt", gun_now.c_str());
                  current_gun = readFile(SPIFFS, "/gun.txt").toInt();
              }
            }
          
           else if(current_gun == 4){
            Serial.println((period - start_time));
            timer = period - millis();
            tseconds = timer / 1000;
            tminutes = tseconds / 60;
            seconds = tseconds % 60;
            lcd.setCursor(1,1);
            lcd.print("Timer : ");
            lcd.print(tminutes);
            lcd.print(":");
            lcd.print(seconds);
            //turn off first gun
            digitalWrite(current_gun, LOW);
          
             while(getTemperature() >= low_temp && getTemperature() <= high_temp){
                //turn off second gun
               Serial.println((period - start_time));
               timer = period - millis();
               tseconds = timer / 1000;
               tminutes = tseconds / 60;
               seconds = tseconds % 60;
               lcd.setCursor(1,1);
               lcd.print("Timer : ");
               lcd.print(tminutes);
               lcd.print(":");
               lcd.print(seconds);
               digitalWrite(2, LOW);
             }
              
              if(getTemperature() <= low_temp )
              {
                  while(getTemperature() <= high_temp){
                   Serial.println((period - start_time));
                   timer = period - millis();
                   tseconds = timer / 1000;
                   tminutes = tseconds / 60;
                   seconds = tseconds % 60;
                   lcd.setCursor(1,1);
                   lcd.print("Timer : ");
                   lcd.print(tminutes);
                   lcd.print(":");
                   lcd.print(seconds);
                   digitalWrite(current_gun, HIGH); 
                  }
                  Serial.println((period - start_time));
                  timer = period - millis();
                  tseconds = timer / 1000;
                  tminutes = tseconds / 60;
                  seconds = tseconds % 60;
                  lcd.setCursor(1,1);
                  lcd.print("Timer : ");
                  lcd.print(tminutes);
                  lcd.print(":");
                  lcd.print(seconds);
                  digitalWrite(current_gun, LOW);
                  //changing current gun with idle gun 
                  current_gun = 2;
                  String gun_now = String(current_gun);
                  writeFile(SPIFFS, "/gun.txt", gun_now.c_str());
                  current_gun = readFile(SPIFFS, "/gun.txt").toInt();
              }
            }
          }
          
  //Shutdown Operation
  digitalWrite(gun_a, LOW);
  digitalWrite(gun_b, LOW);
  lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("FINISHED");
   lcd.setCursor(1,1);
   lcd.print("Sterilizing :)");
  //Back to state 1
  state = 1;
}

float getTemperature(){
  //Get readings from Sensor
  //Measure Temperature from the HTU21D or Si7021
  tempC = sensor.getTemp();
  Serial.println(tempC);
  return tempC;
  
}

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
