/*
   pins   0,1  are also connected to the usb-to-ttl chip, cannot be used with the "Serial" class at the same time.
   pins  2-13  are available (pin 13 also has the yellow led)
   pins 14-19  used to receive and transmit ttl data (huh?) - can be used digitally.
   pins 20,21  used for comms, SDA, SDC for I2C, using the Wire Library.
   the two top corner ones do not seem to be power.
   the two bottom corner ones seem to be ground.
   pins 22-53  available, confirmed (50-53 MISO/MOSI/SCK/SS for SPI with the SPI Library?)
   so, if no USB or COMMs are used, we have 54 output pins, all tested!
   
   there seem to be 16 analog inputs. 
   - plenty for 8 returns from switch matrix, 
   - 3 from pinball front user.
   - it takes about 100 nsec (0.1 msec) to read an analog input...
   if they are slow, we could use some of the 54 digital pins, using "pinMode(IN)"
   
   it seems we can also use the Analog pins for general digital I/O: https://www.arduino.cc/en/Tutorial/AnalogInputPins
   tested, it works. In setup():
      pinMode(13, OUTPUT);
      pinMode(A0, INPUT_PULLUP);
   in loop()
      digitalWrite(13, digitalRead(A0));
   the led is constantly lit (A0 is pulled up) and, if a pushbutton brings it to ground, the led goes out!
   
   
   
   digitalWrite() reaches from a little bit less than 4µs (UNO, non PWM pin) to a little bit less than 6µs (MEGA, PWM pin).

   There's also direct port manipulation. Here's a method I use a lot.
   Say you were changing PortB, bit 0 (= D8 on an Uno:
	
   PORTB = PORTB & 0b11111110; // clear bit 0,  leave rest alone
   PORTB = PORTB | 0b00000001; // set bit 0, leave rest alone
	
   Use pinMode in setup as you normally would.
   If you just want to flip an output bit (because you already know what state it is in) you can write to the input port:
	
   PINB = 0b00000001; // toggle bit 0
  
   But definitely see: http://www.billporter.info/2010/08/18/ready-set-oscillate-the-fastest-way-to-change-arduino-pins/
   using direct port, it takes 2 cpu cycles, while digitalWrite takes about 50.
  	
   digitalWrite(pin, LOW);       digitalWrite(pin, HIGH);    7000 nsec
   CLR(PORTB, 0) ;     				SET(PORTB, 0);               250 nsec
   PORTB |= _BV(0);              PORTB &= ~(_BV(0));          250 nsec
	
   The macros used:

   #define CLR(x,y) (x&=(~(1<<y)))
   #define SET(x,y) (x|=(1<<y))
   #define _BV(bit) (1 << (bit))
	
   we could create preprocessor macros for each pin, as in:
   #define SET_PIN_STROBE()  SET(PORTC, 3)  	
   #define CLR_PIN_STROBE()  CLR(PORTC, 3)
	
*/

/*
   See also https://lynx2015.files.wordpress.com/2015/08/arduino-mega-pinout-diagram.png
   It's good to put the lamps rows and switch returns to a full port each.
	
   Columns:
   - Arduino Pin No
   - CPU Port:Bit
   - Arduino usage
   - CPU Usage
   - My Usage
   
   Pin  P:B  Ard.Usage     CPU Usage      My usage
   ----------------------------------------------------------------
     0  PE0  RX0+USB       U              Software loading + logging
     1  PE1  TX0+USB       U              Software loading + logging
     2  PE4  PWM           U              U
     3  PE5  PWM           U              U
     4  PG5  PWM           U              U
     5  PE3  PWM           U              U
     6  PH3  PWM           U              U
     7  PH4  PWM           U              U
     8  PH5  PWM           U              U
     9  PH6  PWM           U              U
    10  PB4  PWM           U              U
    11  PB5  PWM           U              U
    12  PB6  PWM           U              U
    13  PB7  PWM+LED       U              U
    14  PJ1  TX3           U              Wav Trigger comm
    15  PJ0  RX3           U              Wav Trigger comm
    16  PH1  TX2           U              U
    17  PH0  RX2           U              U
    18  PD3  TX1           U              U
    19  PD2  RX1           U              U
    20  PD1  SDA           U              U
    21  PD0  SCL           U              U
    22  PA0  I/O           U              U
    23  PA1  I/O           U              U
    24  PA2  I/O           U              U
    25  PA3  I/O           U              U
    26  PA4  I/O           U              U
    27  PA5  I/O           U              U
    28  PA6  I/O           U              U
    29  PA7  I/O           U              U
    30  PC7  I/O           U              U
    31  PC6  I/O           U              U
    32  PC5  I/O           U              U
    33  PC4  I/O           U              U
    34  PC3  I/O           U              U
    35  PC2  I/O           U              U
    36  PC1  I/O           U              U
    37  PC0  I/O           U              U
    38  PD7  I/O           U              U
    39  PG2  I/O           U              U
    40  PG1  I/O           U              U
    41  PG0  I/O           U              U
    42  PL7  I/O           U              U
    43  PL6  I/O           U              U
    44  PL5  I/O           U              U
    45  PL4  I/O           U              U
    46  PL3  I/O           U              U
    47  PL2  I/O           U              U
    48  PL1  I/O           U              U
    49  PL0  I/O           U              U
    50  PB3  I/O           U              U
    51  PB2  I/O           U              U
    52  PB1  I/O           U              U
    53  PB0  I/O           U              U
    A0  PF0  ADC0          U              U
    A1  PF1  ADC1          U              U
    A2  PF2  ADC2          U              U
    A3  PF3  ADC3          U              U
    A4  PF4  ADC4          U              U
    A5  PF5  ADC5          U              U
    A6  PF6  ADC6          U              U
    A7  PF7  ADC7          U              U
    A8  PK0  ADC8          U              U
    A9  PK1  ADC9          U              U
   A10  PK2  ADC10         U              U
   A11  PK3  ADC11         U              U
   A12  PK4  ADC12         U              U
   A13  PK5  ADC13         U              U
   A14  PK6  ADC14         U              U
   A15  PK7  ADC15         U              U

*/
int current_pin = 0;
int pins[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
               18,19,20,21,22,23, 24,25,26,27,28,29,30,31,32,33,34,
               35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};



void setup_pins() {
   //int pins_len = sizeof(pins)/sizeof(pins[0]);
   //for (int i = 0; i < pins_len; i++)
   //	pinMode(pins[i], OUTPUT);
	
   // set the A0 input to PULLUP_RESISTOR
   pinMode(13, OUTPUT);
   pinMode(A0, INPUT_PULLUP);
}

void loop_pins() {
   int pins_len = sizeof(pins)/sizeof(pins[0]);
   for (int i = 0; i < pins_len; i++)
      digitalWrite(pins[i], 0);

   current_pin++;
   if (current_pin >= pins_len)
      current_pin = 0;
		
   digitalWrite(pins[current_pin], 1);

   digitalWrite(13, digitalRead(A0));
}
