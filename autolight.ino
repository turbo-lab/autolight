#include <FreqCounter.h>

#define CPS          1  // Do loop 1 times per second
#define LOOPDELAY    1000 / CPS

#define TACHOPIN     2  // Tachometer
#define BREAKPIN     4  // Pin for break detection
#define HEADPIN      5  // Head light relay pin
#define TAILPIN      6  // Tail relay pin
#define LDRPIN       A0 // LDR pin

#define TAILONLEVEL  500 // Tail on threshold
#define TAILOFFLEVEL 200 // Tail off threshold
#define TAILOFFDELAY 5 * CPS  // Delay for switch off tail

#define TACHOLEVEL   600 / 60 / CPS  // Running engine threshold is equal 600 RPM

int roundsValue = 0;
int lastRoundsValue;
int ldrValue = 0;
int tailTimer = 0;

void tachoInterrupt()
{
    roundsValue++;
}

boolean isEngineRunning()
{
    lastRoundsValue = roundsValue;
    roundsValue = 0; 
  
    Serial.print("Tachometer: ");
    Serial.print(lastRoundsValue, DEC);
    Serial.println(" rpm"); 
  
    return lastRoundsValue > TACHOLEVEL ? true : false;
}


void setup() {
    attachInterrupt(TACHOPIN - 2, tachoInterrupt, FALLING);
    pinMode(TACHOPIN, INPUT);
    pinMode(BREAKPIN, INPUT);
    pinMode(HEADPIN, OUTPUT);
    pinMode(TAILPIN, OUTPUT);  
    digitalWrite(HEADPIN, LOW);
    digitalWrite(TAILPIN, LOW);
}

void loop() {
  
  if(isEngineRunning) { // The lights is only allowed when engine is running
      if(digitalRead(BREAKPIN) == 0) {  // Break is pressed
          digitalWrite(HEADPIN, HIGH);  // We should switch on head lights
      }

      ldrValue = analogRead(LDRPIN);

      if (tailTimer) { // Tail lights are switched on 
          if (ldrValue > TAILONLEVEL) {  // Too dark again
              tailTimer = TAILOFFDELAY;  // restart timer
          }
          else if (ldrValue < TAILOFFLEVEL) { // Too light
              tailTimer--;                    // decrement timer value
          }
          if (tailTimer == 0) {            // Too light for a long time
              digitalWrite(TAILPIN, LOW);  // We should switch off tail lights
          }
      }
      else { // Tail lights are switched off
          if(ldrValue > TAILONLEVEL) {     // Too dark
              digitalWrite(TAILPIN, HIGH); // We should switch on tail lights immediately
              tailTimer = TAILOFFDELAY;
          }
      }
  }
  else { // otherwise save battery
      digitalWrite(HEADPIN, LOW);  // We should switch off both lights
      digitalWrite(TAILPIN, LOW);
  }
  
  delay(LOOPDELAY);       
}
