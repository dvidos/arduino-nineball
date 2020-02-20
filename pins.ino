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
