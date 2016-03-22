#include <ArduinoJson.h>

#define RMOTORDIR 8
#define RMOTORPWM 3
#define LMOTORDIR 6
#define LMOTORPWM 5
#define PING      13
#define LMOTORRATIO 1.0f
#define RMOTORRATIO 1.0f
#define SLOW    50

#define DEBUG     (12)

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(LMOTORDIR, OUTPUT);
  pinMode(LMOTORPWM, OUTPUT);
  pinMode(RMOTORDIR, OUTPUT);
  pinMode(RMOTORPWM, OUTPUT);
  pinMode(DEBUG, OUTPUT);
  digitalWrite(DEBUG, LOW);

  Serial.begin(19200);
  Serial.println("start");
  //delay(5000L);
}

int lmotord = HIGH, rmotord = HIGH;
int lmotors = (int) ((float) SLOW * LMOTORRATIO);
int rmotors = (int) ((float) SLOW * RMOTORRATIO);

// the loop function runs over and over again forever
void loop() {


  //analogWrite(LMOTORPWM, lmotor);
  //analogWrite(RMOTORPWM, rmotor);

  /*StaticJsonBuffer<200> jsBuf;
   JsonObject& root = jsBuf.createObject();
   root["ping"] = pingCm();
   root["lmotor"] = lmotor;
   root["rmotor"] = rmotor;
   root.printTo(Serial);
   Serial.println();
   */

  if(Serial.available() > 0) {
    digitalWrite(DEBUG, HIGH);
    StaticJsonBuffer<200> buf;
    String msg;
    for(int i = 0; i < 200; i++) {
      char c = (char) Serial.read();
      if(c  == '\n') break;
      else msg += c; 
    }
    Serial.flush();
    JsonObject& root = buf.parseObject(msg);
    Serial.println(msg);
    String command = root["command"];

    lmotors = (int) ((float) SLOW * LMOTORRATIO);
    rmotors = (int) ((float) SLOW * RMOTORRATIO);

    if(command == "forward") {
      lmotord = 1;
      rmotord = 1;
    } 
    else if (command == "backward") {
      lmotord = 0;
      rmotord = 0;
    } 
    else if (command == "left") {
      lmotord = 0;
      rmotord = 1;
    } 
    else if (command == "right") {
      lmotord = 1;
      rmotord = 0;
    } 
    else if (command == "stop") {
      lmotors = 0;
      rmotors = 0;
    }

    Serial.println(lmotord);
    Serial.println(rmotord);
    Serial.println(lmotors); 
    Serial.println(rmotors);
    Serial.println();


    analogWrite(LMOTORPWM, lmotors);
    analogWrite(RMOTORPWM, rmotors);
    digitalWrite(LMOTORDIR, lmotord);
    digitalWrite(RMOTORDIR, rmotord);

  }

  delay(100L);
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

