/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 16 has an LED connected on most Arduino boards.
// give it a name:
#define LMOTORDIR 8
#define LMOTORPWM 3
#define RMOTORDIR 6
#define RMOTORPWM 5

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
    pinMode(LMOTORDIR, OUTPUT);     
    pinMode(RMOTORDIR, OUTPUT);
    pinMode(RMOTORPWM, OUTPUT);
    pinMode(LMOTORPWM, OUTPUT);
    analogWrite(RMOTORPWM, 50);
    analogWrite(LMOTORPWM, 50);
}  

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(LMOTORDIR, HIGH);
  digitalWrite(RMOTORDIR, HIGH);
  delay(1000);
  digitalWrite(LMOTORDIR, LOW);
  delay(1000);
  digitalWrite(RMOTORDIR, LOW);
  delay(1000); 
  digitalWrite(LMOTORDIR, HIGH);
  delay(1000);
}
