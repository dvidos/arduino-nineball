

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CSettings Settings;





void setup() {
#ifdef RUN_TESTS
    run_tests();
#else
	noInterrupts();
	setup_timers();
	setup_pins();
	setup_sounds();
	interrupts();
#endif
}

void loop() {
#ifdef RUN_TESTS
#else
#endif
}




