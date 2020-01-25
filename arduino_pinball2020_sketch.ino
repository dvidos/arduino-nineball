/*
  
  
  menu selection
  ------------------
  pushing the key under the pinbal, we shuold cycle the various modes, pushing start would start a mode.
  0 = normal play (e.g. what start will do if no black button is pressed)
  1 = happy mode, infinite balls
  2 = practice mode, specific targets.
  ...
  8 = settings menu
  9 = diagnostics menu
  
  
  watchdog
  -----------
  if the pin is free, have the led being lit every two seconds or so, 
  to understand whether we fell in an infinite loop or something.
  
  
  
  variables
  --------------
  keep everything in 3-4 large structures with bit fields 
  to preserve space and allow for easy reporting of sizeof()
  
  
  timing
  -----------
  should keep time in 10 or 100 msecs increments. 
  A char would allow up to 2.5 seconds or 25 seconds duration which is good for coils, animations, lamps etc.
  
  Should have a way of keeping some stats, such as max lamps blinking at the time and max switches queue length.
  
  Timers
  ---------
  Timers 0 and 2, 8 bits
  Timer 1, 16 bits
  On mega, there are also timers 3, 4, 5, 16 bits
  Timer 0 is for delay(), millis() and micro()
  Timer 1 is for servo library (we don't care)
  Timer 2 is for tone (we don't care)
  
  noInterrupts() will disable interrupts (inside the IRSR)
  interrupts() enables them
  well, quite a few pseudo-variables to set. TCCR3A, TCCR3B, OCR3A etc
  quite a lot to digest, here: https://oscarliang.com/arduino-timer-and-interrupt-tutorial/
  but I think we can make an interrupt timer for milliseconds (lamp & matrix switch)
  and another timer for timeouts (coils and game timeouts)
  
  // here's how to define the ISR for a timer:
  ISR(TIMER1_COMPA_vect) {
    Led1.Refresh();
  }
  
  
*/


void setup() {
  Serial.begin(9600);
  Serial.println("Hello!");
}

void loop() {
  // Serial.println("Hello!");
  // lampOn(2);
  // lampOff(1);
}



