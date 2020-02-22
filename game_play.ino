
/*
	the initial idea was to have a state machine,
	switching actions based on context and events that happened.
	the various timed actions (timeouts, switches detection) are performed in interrupts, so they have to be fast.
	therefore we will set flags from the interrupts and handle the events from the main loop.
	
	the main logic is to respond to events and kick off coils, sounds, lamps, scores, etc. 
	
	adding score is done with an unsigned int, in BCD, in tens. That means that 3,000 is encoded in 0x0300.
	also, adding score is done on a temporary buffer and a background process adds it to the real score, in thousands
	this is to simulate slowly giving the user points and make it more enjoyable.
	
*/

void game_play_tick() {
	// act on switches
	// act on timeouts
	// act on animations finished
	// ?
}

void start_animation(int animation_no);
void start_timer(int timer_no);
void start_sound(int sound_no);
void start_coil(int coil_no);
void queue_score(uint bcd_amount_in_tens);


void handle_timeout(char timeout_no) {
	switch (timeout_no) {
		case TIMEOUT_BALL_NINE_MOVING_TARGET:
			// move to the next moving target
		case TIMEOUT_WOW_MOVING_TARGET:
			// move to the next moving target, skip n
	}
}

void handle_animation_finished(char animation_no) {
	switch (animation_no) {
		case ANIM_TOP_LOOP_SCORE:
			// set the lamps to the appropriate top loop score
		case ANIM_BONUS_MULTIPLIER:
			// set the lamps to the appropriate bonus multiplier
		case ANIM_SPINNER_VALUE:
			// set the lamps to the approrpiate spinner value
		case ANIM_BALLS_DIAMOND:
			// set the lamps to the appropriate objects made
	}
}

void handle_switch_closed(char switch_no) {
	switch (switch_no) {
		case SW_LEFT_OUTLANE:
		case SW_RIGHT_OUTLANE:
		case SW_LEFT_INLANE:
		case SW_RIGHT_INLANE:
		
		case SW_MAIN_POP_BUMPER:
		case SW_TOP_POP_BUMPER:
		
		case SW_TOP_BANK_LEFT_TARGET:
		case SW_TOP_BANK_CENTER_TARGET:
		case SW_TOP_BANK_RIGHT_TARGET:
		
		case SW_RIGHT_BANK_LEFT_TARGET:
		case SW_RIGHT_BANK_CENTER_TARGET:
		case SW_RIGHT_BANK_RIGHT_TARGET:
		
		case SW_LEFT_BANK_TARGET_1:
		case SW_LEFT_BANK_TARGET_2:
		case SW_LEFT_BANK_TARGET_3:
		case SW_LEFT_BANK_TARGET_4:
		case SW_LEFT_BANK_TARGET_5:
		case SW_LEFT_BANK_TARGET_6:
		case SW_LEFT_BANK_TARGET_7:
		case SW_LEFT_BANK_TARGET_8:
			// I think we must raise the 8 drop targets whenever target 8 is hit,
			// to avoid having a ball stuck on the gap that 8 leaves.
		
		case SW_LEFT_LANE_CAPTURE_FIRST_BALL:
		case SW_LEFT_LANE_CAPTURE_SECOND_BALL:
		case SW_LEFT_LANE_CAPTURE_THIRD_BALL:
		case SW_LEFT_LANE_EXIT:
			add_score(0x0300);
			start_audio(AUDIO_FX_1);
			loop_target_value = ((loop_target_value + 1) % 5)
			start_animation(ANIM_TOP_LOOP_SCORE);
			break;
	
		case SW_SKILL_SHOT_TARGET:
		
		case SW_SPINNER:
			
		
		case SW_TOP_LOOP_PASS:
		case SW_TOP_LOOP_TARGET:
		
		case SW_DRAIN_HOLE:
	}
}

void handle_switch_opened(char switch_no) {
	switch (switch_no) {
		case SW_EJECT_LANE:
			// game on, no more players, start audio etc.
			break;
	}
	
}