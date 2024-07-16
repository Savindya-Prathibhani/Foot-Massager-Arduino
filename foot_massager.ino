#include <Wire.h>
#include <LiquidCrystal_I2C_Hangul.h>
#include <Keypad.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// LCD DisplY
LiquidCrystal_I2C_Hangul lcd(0x3F,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

#define WaterLevelSensorPin A0
#define irSensorPin 29 // IR Sensor pin 29
#define ONE_WIRE_BUS 27  // temperature sensor pin 27
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

#define spray 37                   //relay 1
#define fan1 35                  //relay2
#define fan2 33                //relay 3
#define motor 31             //relay4
#define WaterLevelLedpin 6
#define buzzerPin 7


int const PULSE_SENSOR_PIN = A1;

int Threshold = 700;
int Signal;
int bpm; 

int r_en = 10;
int l_en = 9;
//Use PWM pins
int r_pwm = 11;
int l_pwm = 12;
int pwm=180;

// Variables
boolean legDetected = false;
boolean fanOn = false;
boolean motorOn = false;
boolean sprayOn = false;
boolean buzOn = false;

int Wlevel = 0;

String inputString;
int inputInt;
int timepoint = 14;


unsigned long fanStartTime = 0;
unsigned long sprayStartTime = 0;
unsigned long massageStartTime = 0;
unsigned long legRemovedTime=0;


int fanDuration =2;
int massageDuration;
int sprayDuration=1;


const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte colPins[COLS] = {47,49,51,53 }; // Pins connected to C1, C2, C3, C4
byte rowPins[ROWS] = {39,41,43,45}; // Pins connected to R1, R2, R3, R4

Keypad userKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  
  pinMode(irSensorPin, INPUT);
 // pinMode(PULSE_SENSOR_PIN,INPUT);
  pinMode(WaterLevelLedpin,OUTPUT);
  
  
  pinMode(fan1,OUTPUT);
  pinMode(fan2,OUTPUT);
  pinMode(motor,OUTPUT);
  pinMode(spray,OUTPUT);


  pinMode(r_en, OUTPUT);
  pinMode(l_en, OUTPUT);
  pinMode(r_pwm, OUTPUT);
  pinMode(l_pwm, OUTPUT);

  

  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  lcd.setCursor(2,0);   //Set cursor to character 3 on line 0
  lcd.print("Foot Massager");   // Print initial messages on the LCD
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Leg on");
  lcd.setCursor(0,1);
  lcd.print("the machine");

  sensors.begin(); // Start up the DallasTemperature library

}

void loop() {
  //Check if leg is detected by IR sensor
  //irSensorValue = digitalRead(irSensorPin);
  if((!legDetected && fanOn) || (!legDetected && motorOn) || (!legDetected && sprayOn)){
  if(millis()-legRemovedTime>3000){
    digitalWrite(fan1,LOW);
    digitalWrite(fan2,LOW);
    digitalWrite(motor,LOW);
    digitalWrite(r_en, LOW);
    digitalWrite(l_en, LOW);
    digitalWrite(spray,LOW);
    fanOn=false;
    motorOn=false;
    sprayOn=false;
  }
  }
  
  if (digitalRead(irSensorPin) == LOW) {
    if (!legDetected) {
      legDetected = true;
      lcd.clear();
      lcd.print("Leg Detected");
      delay(1000);
      lcd.clear();

    }else{

    }
    char key = userKeypad.getKey();
      
      if(key=='A'){
        if(motorOn){
          digitalWrite(motor,LOW);
          stopMassage();
          lcd.clear();
          buzAlert();
        }else{
          massageStartTime=functionalKeyA(); 
        }
        
      }
      if(key=='B'){
        if(fanOn){
          digitalWrite(fan1, LOW); 
          digitalWrite(fan2,LOW);
          fanOn=false;
          buzAlert();
        }else{
          fanStartTime=functionalKeyB();
          Serial.print("fan start");
        }
        
      }
      if(key=='C'){
        if(sprayOn){
          digitalWrite(spray,LOW);
          sprayOn=false;
          buzAlert();
        }else{
          if(buzOn){
            buzOn=false;
          }else{ 
            sprayStartTime=functionalKeyC();
          }
        }
        
      }
      
      if(key=='D'){
        lcd.clear();
        lcd.print("Reading Pulse....");
        delay(1000);
        readPulse();
        delay(1000);
        lcd.clear();
      }
    
      //check whether the liquid is filled
       Wlevel = readWaterLevelSensor();

        if(Wlevel<500){
          digitalWrite(WaterLevelLedpin,HIGH);
      
        }else{
          digitalWrite(WaterLevelLedpin,LOW);
          buzOn=false;
        }

      int temperature = getTemperature();
      displayTemperature(temperature);
    
        if(temperature<=20){
        //heating fan on
          digitalWrite(fan1,HIGH);
          digitalWrite(fan2,HIGH);
          buzAlert();

        }else if(temperature>40){
          digitalWrite(fan1,LOW);
          digitalWrite(fan2,LOW);
          buzAlert();
        }
        
      // Measure and display pulse rate
   //   readPulse();
      

	delay(20);
      
   
      
  if(fanOn){
    if(millis()-fanStartTime>=(fanDuration*60000)){
      digitalWrite(fan1,LOW);
      digitalWrite(fan2,LOW);
      fanOn=false;
      buzAlert();
    }
  
  } 

  if(sprayOn){
    if(millis()-sprayStartTime>=(sprayDuration*60000)){
      digitalWrite(spray,LOW);
      sprayOn=false;
      buzAlert();
    }
    
  }   
  if(motorOn){
    if(millis()-massageStartTime>=(massageDuration*60000)){
      digitalWrite(motor,LOW);
      stopMassage();
      lcd.clear();
    }
    
    int leftTime = massageDuration- ((millis()-massageStartTime)/60000);
    lcd.setCursor(0,1);
    lcd.print("Time left : ");
    lcd.setCursor(12,1 );
    lcd.print(leftTime);
    lcd.setCursor(13,1);
    lcd.print("mins");
    
  }
   if(buzOn){
    // digitalWrite(buzzerPin,HIGH);
     lcd.clear();
     lcd.print("Add liquid !");
     analogWrite(buzzerPin,  20);
     delay(400);
     digitalWrite(buzzerPin,LOW);
   }   
    
  }else {
    if (legDetected) {
      legRemovedTime = millis();
      legDetected = false;
      lcd.clear();
      lcd.print("Place Leg on");
      lcd.setCursor(0, 1);
      lcd.print("the Machine");
     
    }
  }
  
}
int getTemperature(){
  sensors.requestTemperatures();       // Send the command to get temperatures
  int temperatureC = sensors.getTempCByIndex(0);  // Read the temperature in Celsius
  delay(500);
  return temperatureC;
}

//display temperature
void displayTemperature(int temperature) {
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C");
}

//display pulse
void readPulse()
{ 
  delay(1000);
  while(Signal<Threshold){
    Signal = analogRead(PULSE_SENSOR_PIN); 
  }
  
  if(Signal>Threshold){
    bpm = 60000/Signal;
      lcd.clear();
      lcd.print("Heart Rate:");
      lcd.setCursor(0,1);
      lcd.print(bpm);
      lcd.setCursor(4,1);
      lcd.print("BPM");
      //lcd.display(bpm);
      delay(1000);
  }else{
    lcd.clear();
    lcd.print("Finger not detected");
    delay(1000);
  }
  Signal=0;
}


//check liquid level
int readWaterLevelSensor() {
	int wlevel = analogRead(WaterLevelSensorPin);		// Read the analog value form sensor
	return wlevel;							// send current reading
}



void startMassage(){
  digitalWrite(r_en, HIGH);
  digitalWrite(l_en, HIGH);
  //RPM in forward and backward
  analogWrite(r_pwm, pwm);
  analogWrite(l_pwm, 0);
  
  motorOn=true;
  buzAlert();
}

void stopMassage(){
  digitalWrite(r_en, LOW);
  digitalWrite(l_en, LOW);
  motorOn=false;
  lcd.clear();
  buzAlert();
}

void buzAlert(){
    analogWrite(buzzerPin,  20);
    delay(400);
    digitalWrite(buzzerPin,LOW);
}

unsigned long functionalKeyA(){      
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Massage Time: ");
        char key;
      
        do{
            key = userKeypad.getKey();
          
          lcd.setCursor(14,0);
          if(key=='*'){
            inputString="";
             lcd.clear();  
             lcd.print("Massage Time: ");
             lcd.setCursor(14,0);
          }
          if (key >= '0' && key <= '9' && inputString.length()<2) {     // only act on numeric keys
            inputString += key; 
            lcd.print(inputString);  
                        // append new character to input string
          }    
        }while (key!='#');  // while key!='#' i put this when the # is not working
        
        lcd.setCursor(14,0);
        lcd.print(inputString);
        delay(1000);
        lcd.clear();

        if (inputString.length() > 0) {
        inputInt = inputString.toInt();// YOU GOT AN INTEGER NUMBER
        inputString = "";               // clear input
        }else{
          inputInt=0;
        }
        
        if(inputInt<=15 ){
        lcd.clear();
        do{
          lcd.setCursor(0, 0);
          lcd.print("Select the Speed");
          key = userKeypad.getKey();  
        }while ((key!='1') && (key!='2') && (key!='3'));  
         
         if(key=='1'){
             pwm=180;
         }else if(key=='2'){
             pwm=200;
         }else if(key=='3'){
             pwm=220;
         }
         massageDuration=inputInt; 
         digitalWrite(motor,HIGH);
         startMassage();
         lcd.clear();
         lcd.print("massaging");
         delay(1000);
        }else{
          functionalKeyA();
        }      
return millis();      
}

unsigned long functionalKeyB(){
            digitalWrite(fan1,HIGH);
            digitalWrite(fan2,HIGH);
            buzAlert();
            fanOn=true;
            return millis();
}

unsigned long functionalKeyC(){
        
        if(Wlevel>500){
          //spray on
          digitalWrite(spray,HIGH);
          buzAlert();
          sprayOn=true;
        }else{
           buzOn=true;
           lcd.clear();
           lcd.print("Add liquid !");
           delay(1000);
           lcd.clear();
        }
        
        Serial.print("spray on/off");
     
return millis();
}
