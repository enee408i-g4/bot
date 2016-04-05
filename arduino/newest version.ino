#include <Pixy.h>

#define RMOTORDIR 8
#define RMOTORPWM 3
#define LMOTORDIR 6
#define LMOTORPWM 5
#define PING      13
#define LMOTORRATIO 1.0f
#define RMOTORRATIO 1.0f
#define SLOW    50

#define DEBUG     (12)
#define PIXY_X_CENTER (((PIXY_MAX_X)-(PIXY_MIN_X))/2)
#define PIXY_Y_CENTER (((PIXY_MAX_Y)-(PIXY_MIN_Y))/2)

Pixy pixy;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(LMOTORDIR, OUTPUT);
  pinMode(LMOTORPWM, OUTPUT);
  pinMode(RMOTORDIR, OUTPUT);
  pinMode(RMOTORPWM, OUTPUT);
  pinMode(DEBUG, OUTPUT);
  digitalWrite(DEBUG, LOW);

  Serial.begin(9600);
  Serial.println("start");
  pixy.init();
  //delay(5000L);
}

int lmotord = HIGH, rmotord = HIGH;
int lmotors = (int) ((float) SLOW * LMOTORRATIO);
int rmotors = (int) ((float) SLOW * RMOTORRATIO);

// the loop function runs over and over again forever
void loop() {


  static int m = 0;
  int n;
  uint16_t blocks;
  char buff[32]; 
  
  // grab blocks!
  blocks = pixy.getBlocks();
  
  // If there are detect blocks, print them!
  if (blocks)
  {
    m++;
    
    // do this (print) every 50 frames because printing every
    // frame would bog down the Arduino
    if (m%15==0)
    {
      sprintf(buff, "Detected %d:\n", blocks);
      Serial.print(buff);
      for (n=0; n<blocks; n++)
      {
        sprintf(buff, "  block %d: ", n);
        Serial.print(buff); 
        pixy.blocks[n].print();
        command(pixy.blocks[0]); 
        analogWrite(LMOTORPWM, lmotors);
        analogWrite(RMOTORPWM, rmotors);

        delay(250);

        analogWrite(LMOTORPWM, 0);
        analogWrite(RMOTORPWM, 0);
        
      }
    }
  }


  /*StaticJsonBuffer<200> jsBuf;
   JsonObject& root = jsBuf.createObject();
   root["ping"] = pingCm();
   root["lmotor"] = lmotor;
   root["rmotor"] = rmotor;
   root.printTo(Serial);
   Serial.println();
   */

    /*lmotors = (int) ((float) SLOW * LMOTORRATIO);
    rmotors = (int) ((float) SLOW * RMOTORRATIO);*/

  delay(100L);
}

void command(Block pixy_obj){
    int fwdRange = 30;
    int pan_error = PIXY_X_CENTER - pixy_obj.x;
    int leftmotordir=0;
    int rightmotordir=0;
    if ((pan_error < fwdRange) && (pan_error > -fwdRange)){

    Serial.print("forward");
    leftmotordir=1;
    rightmotordir=1;
    
    }
    else if(pan_error > fwdRange){

    Serial.print("left");
    leftmotordir=0;
    rightmotordir=1;
    
    }
    
    else if(pan_error < -fwdRange){

    Serial.print("right");
    leftmotordir=1;
    rightmotordir=0;
    
    }


    Serial.println();
    Serial.println("left :");
    Serial.println(leftmotordir);
    Serial.println(lmotors);
    Serial.println("right :");
    Serial.println(rightmotordir);
    Serial.println(rmotors);
    Serial.println();


    digitalWrite(LMOTORDIR, leftmotordir);
    digitalWrite(RMOTORDIR, rightmotordir);

  
}

long pingCm() {
  long duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(PING, OUTPUT);
  digitalWrite(PING, LOW);
  delayMicroseconds(2);
  digitalWrite(PING, HIGH);
  delayMicroseconds(5);
  digitalWrite(PING, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(PING, INPUT);
  duration = pulseIn(PING, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  return cm;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
