#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

const int stepsPerRevolution = 2048; 

Stepper pan(stepsPerRevolution, 2, 4, 3, 5);
Stepper tilt(stepsPerRevolution, 6, 8, 7, 12);
Servo shutter;

LiquidCrystal_I2C lcd(0x27,16,2); 

int images = 0;
int pans = 0;
int tilts = 0;

int currentPanAngle = 0;
int currentTiltAngle = 0;

const int shutterwait = 2000; //in ms (also this is the thing for the camera to capture a image, i will use 1S so its gonna be a little bit longer, also it needs to return to the menu so its that)
const int shutterstart = 0; // at 0 degrees
const int shutterend = 30; // at 30 degrees
const int presstime = 300; // again some shitty 300 ms (this is gonna be painful)

const int x = 4; // how much would the servo move in every capture
const int y = 3; // again same shit as usual

void setup() {
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("ok this will work");
  delay(1000);
  lcd.clear();
  
  pan.setSpeed(10);
  tilt.setSpeed(10);
  
  shutter.attach(11);
  
  // write() yerine yazdığımız servoMove fonksiyonunu kullanarak 90'a gidiyoruz
  pans = 90;
  tilts = 90;
  servoMove();
  
  Serial.begin(9600);
  Serial.println("calibrate now it should point directly up");
  
  // Tekrar 0'a dönüyoruz
  pans = 0;
  tilts = 0;
  servoMove();
  
  Serial.println("ok its now at 0 0 it should start in 20 seconds");
  delay(20000);
}

void pressShutter(){
  //so this shit is complex for me so there is a function
  shutter.write(shutterstart);
  shutter.write(shutterend);
  delay(presstime);
  shutter.write(shutterstart);
  delay(shutterwait);
  // holy fuck a image takes nearly 3 secs this is gonna take hours aaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhhhh
  images = images+1;
  lcd.clear();
  lcd.print("taken images: " + (String(images)));
}

void servoMove(){
  int targetPanSteps = (pans * stepsPerRevolution) / 360;
  int currentPanSteps = (currentPanAngle * stepsPerRevolution) / 360;
  int panStepDiff = targetPanSteps - currentPanSteps;
  
  int targetTiltSteps = (tilts * stepsPerRevolution) / 360;
  int currentTiltSteps = (currentTiltAngle * stepsPerRevolution) / 360;
  int tiltStepDiff = targetTiltSteps - currentTiltSteps;
  
  if (panStepDiff != 0) {
    pan.step(panStepDiff);
    currentPanAngle = pans;
  }
  if (tiltStepDiff != 0) {
    tilt.step(tiltStepDiff);
    currentTiltAngle = tilts;
  }
}

void loop() {
  for (int a = 0; a < (180/y); a++) {
    for (int i = 0; i < (180/x); i++) {
      pans = (pans + x);
      servoMove();
      pressShutter();
    }
    pans=0;
    tilts = (tilts + y);
    servoMove();
    pressShutter();
  }
  lcd.print("ok done!");
  while(1); // İşlem bitince durması için ekledim
}