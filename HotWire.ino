// Hotwire Foam Cutter Controller
// Arduino control of a MOSFET for cutting Foam

#include <Arduino.h>
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include"SPI.h"
#include"Wire.h"
#include"boot.c"

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUM_SAMPLES 100

const int maxPWM= 70; // sets the max PWM selectable to prevent over current.
const int pot   = A0;    // input potentiometer
const int vSENSE =A1; // input from voltage divider.
const int PWMOUT =3;  // output pin for MOSFET gate

int cells =0;         //battery cell count
int lowbatt = 0;
int pwmPercent = 0;
int pwmGAUGE =0;
int sum =0;
int i=0;

unsigned char sample_count =0;

float voltage =0.0;
float cvoltage=0;
float AcsValueF =0.0;
float vCalibration =11;

void setup(){
    //Serial.begin(9600);  //debug

     pinMode(PWMOUT, OUTPUT);
     pinMode(pot, INPUT);
     pinMode(vSENSE,INPUT);
     pinMode(13,OUTPUT);
     pinMode(5,OUTPUT);

     rVOLTS();            //measure pack voltage
     rVOLTS();            //Once more for clarity


     display.begin(SSD1306_SWITCHCAPVCC, 0x3c); // generate hi power
     display.clearDisplay();

     digitalWrite(5,HIGH);
     delay(200);
     digitalWrite(5,LOW);

     display.drawBitmap(0, 0, splash, 120, 60, WHITE);  //draw logo
     display.display();
     delay(500);

     display.clearDisplay();
     display.drawBitmap(0, 0, splash3, 120, 60, WHITE);  //draw logo
     display.display();
     delay(1100);

/*
///Testing only.
     display.clearDisplay();
     display.drawBitmap(0, 0, battery4s, 128, 60, WHITE);  //draw logo
     display.display();
     delay(1100);

     display.clearDisplay();
     display.drawBitmap(0, 0, battery3s, 128, 60, WHITE);  //draw logo
     display.display();
     delay(1100);

     display.clearDisplay();
     display.drawBitmap(0, 0, battery2s, 128, 60, WHITE);  //draw logo
     display.display();
     delay(1100);

     display.clearDisplay();
     display.drawBitmap(0, 0, battery1s, 128, 60, WHITE);  //draw logo
     display.display();
     delay(1100);
*/
     display.clearDisplay();
     display.setTextSize(1);
     display.setTextColor(WHITE);
     display.drawRect(0,0,128,63,WHITE); // draw borders and top bar
     display.drawRect(1,1,127,62,WHITE);
     display.fillRect(0,0,128, 15, WHITE);
     display.setCursor(56,20);
     display.print("HOT");
     display.setCursor(34, 32);
     display.print("WIRE CUTTER");
     display.setCursor(45,44);
     display.print("FW 1.2");
     display.setCursor(40, 54);
     display.print("M Set ");
     display.print(maxPWM);
     display.display();
     delay(2000);
        cDetect();
        bSCREEN();
        delay(2000);

}



void loop () {
    int pwm = analogRead(pot);
    rVOLTS();
    voltDetect();

    pwm = map(pwm, 1, 1023, 0, maxPWM);  // map PWM values to provide a lower max duty cycle
    if (pwm<=3) {
        pwm = 0;    //ensure PWM is zero if pot is in low position
            }
    analogWrite (PWMOUT, pwm); //write PWM value to the output
    pwmPercent = map(pwm, 0, maxPWM -1, 0 , 100); //-1 to ensure it displays as 100%
   
    if(pwmPercent >=101)
    {pwmPercent =100;
      }
    pwmGAUGE = map(pwm, 0, maxPWM, 0, 128); // maps the Gauge to the PWM so it fills the screen

pSCREEN(); //display stuff on screen
    //Serial.println(pwm); //debug
}


// reads and calculates voltage
void rVOLTS (){

         while (sample_count < NUM_SAMPLES){
             sum += analogRead(vSENSE);
             sample_count++;
             //delay(1);
         }
     //voltage calculation
    voltage = ((float)sum / (float) NUM_SAMPLES *4.95) /1023.0;
     cvoltage=(voltage*vCalibration);
    //Serial.println(voltage   *11.123);
    //Serial.print("v");
    sample_count =0;
    sum =0;

}


// detects low voltage
void voltDetect(){
 if ( (cells == 3) && (cvoltage <=11.0))
  {lowbatt = 1;}
  else if ((cells == 2) && (cvoltage <= 7.6))
  {lowbatt=1;}
  else if ((cells == 1) && (cvoltage <= 3.8))
  {lowbatt=1;}
    else if ((cells == 4) && (cvoltage <= 14.8))
  {lowbatt=1;}
  else
  {lowbatt = 0;}


  if (lowbatt ==1)
  {
  digitalWrite(13,HIGH);
  digitalWrite(5,HIGH); ///buzzer
    }
  else{
    digitalWrite(13,LOW);
    digitalWrite(5,LOW);
    }
}



void pSCREEN(){
 // display stuff

 display.clearDisplay();
 display.setTextColor(WHITE, BLACK);
 display.setTextSize(1);
 display.setCursor(5,20);
 display.print("Vin");
 display.setCursor(48,20);
 display.print("Power");
 display.setCursor(5, 30);

 //display.print(voltage*11.123 ,1);// one decimal place.
 display.print(cvoltage,1);
 display.print("v");
display.setCursor(115,20);
display.print(cells);
display.print("s");

    //Displays low battery warning
    if (lowbatt ==1)
    {
        display.setTextSize(1);
        display.setCursor(79,45);
        display.print("Low batt");
        display.drawBitmap(0, 0, battery1, 120, 60, WHITE);  //draw logo
        }
            else{}


 display.setCursor(42,35);
 display.setTextSize(2);
 display.print(pwmPercent,1);
 display.print( "%" );
 //display.fillRect(pwm,1,128 - pwm, 10, BLACK);
 display.fillRect(0,0,pwmGAUGE, 15, WHITE); // gauge
 display.drawRect(0,16,128,40,WHITE); //border
 for (i = 1; i <18; i++){
     display.fillRect(i*5,0,2,15,BLACK);
        } /// black bars in gauge

display.display(); //display everything.

}


/// boot screen and cell detect
void bSCREEN(){
display.clearDisplay();

switch (cells) {
    case 1:
     display.drawBitmap(0, 0, battery1s, 128, 60, WHITE);
      break;
    case 2:
    display.drawBitmap(0, 0, battery2s, 128, 60, WHITE);
      break;
    case 3:
    display.drawBitmap(0, 0, battery3s, 128, 60, WHITE);
      break;
          case 4:
    display.drawBitmap(0, 0, battery4s, 128, 60, WHITE);
      break;
    
    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }

 display.setTextColor(WHITE, BLACK);
 display.setTextSize(1);
 display.setCursor(0,0);
 display.print("Vin");
 display.setCursor(20, 0);
 //display.print(voltage*11.123 ,1); // one decimal place.
 display.print(cvoltage ,1);
 display.print("v");
 //display.setCursor(5, 45);
 //display.print(cells);
 //display.print(" Cells Detected");
 display.display(); //display everything. 
}

void cDetect(){
     cvoltage=voltage*vCalibration;
     if (cvoltage >= 1 && cvoltage <=4.8)
      {cells =1;
      //cellScreen = "battery1s";
      }

     if (cvoltage >= 5 && cvoltage <=8.4)
      {cells =2;}

      if(cvoltage >= 10 && cvoltage<= 12.6)
      {cells=3;}

      if(cvoltage >= 13 && cvoltage<= 17)
      {cells=4;}



}
