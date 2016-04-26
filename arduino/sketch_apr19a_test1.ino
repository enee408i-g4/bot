#include <Pixy.h>

// pin definitions
#define RMOTORDIR   (8)
#define RMOTORPWM   (3)
#define LMOTORDIR   (6)
#define LMOTORPWM   (5)
#define STATE_0     (11)
#define STATE_1     (12)
#define STATE_2     (13)
#define PING        (7)

// drive constants
#define LMOTORRATIO (1.0f)
#define RMOTORRATIO (1.0f)
#define SLOW        (50)
int lmotord = HIGH, rmotord = HIGH;
int lmotors = (int) ((float) SLOW * LMOTORRATIO);
int rmotors = (int) ((float) SLOW * RMOTORRATIO);

// state information
#define S_GONE      (1)
#define S_LEFT      (2)
#define S_RIGHT     (3)
#define S_MIDDLE    (4)
#define S_CAPTURED  (5)
int state;

// pixy info
#define PIXY_X_CENTER (((PIXY_MAX_X)-(PIXY_MIN_X))/2)
#define PIXY_Y_CENTER (((PIXY_MAX_Y)-(PIXY_MIN_Y))/2)
Pixy pixy;
int last_pan_error = 0;
int adjusted_lmotors = 0;
int adjusted_rmotors = 0;
const int fwdRange = 30;


void setup() {
  pinMode(LMOTORDIR, OUTPUT);
  pinMode(LMOTORPWM, OUTPUT);
  pinMode(RMOTORDIR, OUTPUT);
  pinMode(RMOTORPWM, OUTPUT);
  pinMode(STATE_0, OUTPUT);
  pinMode(STATE_1, OUTPUT);
  pinMode(STATE_2, OUTPUT);

  state = S_GONE;

  Serial.begin(9600);
  Serial.println("started");
  pixy.init();

}


void loop() {

  static int m = 0;
  uint16_t blocks;
  char buff[32];
  int i = 0;
  int detected = 0;
  
  blocks = pixy.getBlocks();
  if (blocks){
    sprintf(buff, "Detected %d:\n", blocks);
    Serial.print(buff);
   
    for(i=0;i<blocks;i++){
      if(pixy.blocks[i].signature == 2){
        Serial.println("acquired orange cone");
        capture_cone(pixy.blocks[i]);

        if(state == S_CAPTURED){
           Serial.println("tracking dest");
            for(i=0;i<blocks;i++){
                if(pixy.blocks[i].signature == 1){
                  return_dest(pixy.blocks[i]);
                }
                
            }//break;
        }
      }
     }
    }
  else {
  Serial.println("no blocks");
    state = S_GONE;
  }

    for(i=0;i<blocks;i++){
      pixy.blocks[i].print();
    }


  
  digitalWrite(STATE_0, (state >> 0) & 1);
  digitalWrite(STATE_1, (state >> 1) & 1);
  digitalWrite(STATE_2, (state >> 2) & 1);
  Serial.println("state: ");
  Serial.println(state);

  drive();
  
}


void drive() {
  
  switch(state) {
    case S_GONE:
      lmotord = HIGH;
      rmotord = LOW;
      lmotors = (int) ((float) SLOW * (LMOTORRATIO - 0.2));
      rmotors = (int) ((float) SLOW * (RMOTORRATIO - 0.4));
      break;
      
    case S_LEFT:
      lmotord = LOW;
      rmotord = HIGH;
      lmotors = adjusted_lmotors;
      rmotors = adjusted_rmotors;
      break;

    case S_RIGHT:
      lmotord = HIGH;
      rmotord = LOW;
      lmotors = adjusted_lmotors;
      rmotors = adjusted_rmotors;
      break;

    case S_MIDDLE:
      lmotord = HIGH;
      rmotord = HIGH;
      lmotors = (int) ((float) SLOW * LMOTORRATIO);
      rmotors = (int) ((float) SLOW * RMOTORRATIO);
      break;
      
    case S_CAPTURED:
      lmotord = HIGH;
      rmotord = LOW;
      lmotors = 40;
      rmotors = 20 ;
  }
  
  analogWrite(LMOTORPWM, lmotors);
  analogWrite(RMOTORPWM, rmotors);
  digitalWrite(LMOTORDIR, lmotord);
  digitalWrite(RMOTORDIR, rmotord);
  
}


void capture_cone(Block pixy_obj){
      int area = pixy_obj.width * pixy_obj.height;
      if(area > 400) {
        pixy_obj.print();
        process_cone(pixy_obj);
      }  
  
}

void return_dest(Block pixy_obj){
       int dest_area = pixy_obj.width * pixy_obj.height;
       if(dest_area > 400){
          pixy_obj.print();
          Serial.println("destination found: ");
          process_dest(pixy_obj);
       }
}


void process_cone(Block pixy_obj){
    last_pan_error = PIXY_X_CENTER - pixy_obj.x;
    float PWM_COEF;
    long dist;

    dist = pingCm();

    Serial.println(dist);

    if( dist < 14){
      Serial.println("captured");
      state = S_CAPTURED;
    }
      else if ((last_pan_error < fwdRange) && (last_pan_error > -fwdRange)){
      state = S_MIDDLE;
    } else if(last_pan_error > fwdRange){
      state = S_LEFT;  
    } else if(last_pan_error < -fwdRange){
      state = S_RIGHT;   
    }

    if(last_pan_error > fwdRange || last_pan_error < -fwdRange){    
      if(last_pan_error > fwdRange){
        PWM_COEF = 0.012308*last_pan_error + 0.030769;
      }
      else{
        PWM_COEF = -0.012308*last_pan_error + 0.030769;  
      }


      
      adjusted_lmotors = (int) ((float) SLOW * LMOTORRATIO * PWM_COEF * 0.5);
      adjusted_rmotors = (int) ((float) SLOW * RMOTORRATIO * PWM_COEF * 0.5);   
    }

    
}

void process_dest(Block pixy_obj){
    last_pan_error = PIXY_X_CENTER - pixy_obj.x;
    float PWM_COEF;

    if ((last_pan_error < fwdRange) && (last_pan_error > -fwdRange)){
      state = S_MIDDLE;
    } else if(last_pan_error > fwdRange){
      state = S_LEFT;  
    } else if(last_pan_error < -fwdRange){
      state = S_RIGHT;   
    }

    if(last_pan_error > fwdRange || last_pan_error < -fwdRange){    
      if(last_pan_error > fwdRange){
        PWM_COEF = 0.012308*last_pan_error + 0.030769;
      }
      else{
        PWM_COEF = -0.012308*last_pan_error + 0.030769;  
      }


      
      adjusted_lmotors = (int) ((float) SLOW * LMOTORRATIO * PWM_COEF * 0.5);
      adjusted_rmotors = (int) ((float) SLOW * RMOTORRATIO * PWM_COEF * 0.5);   
    }

    Serial.println("state_dest");
    Serial.println(state);
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
