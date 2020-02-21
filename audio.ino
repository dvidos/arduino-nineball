/*
	https://create.arduino.cc/projecthub/BobB/pinball-sounds-and-music-bac92b
	
	https://github.com/robertsonics/WAV-Trigger-Arduino-Serial-Library/blob/master/examples/WTrigBasic/WTrigBasic.ino#L28
	
	It seems I can use one of the internal I/O ports of the Mega, like 18/19.
	
	
*/

#define SOUND_FX_1             1
#define SOUND_FX_2             2
#define SOUND_FX_3             3
#define SOUND_FX_4             4
#define SOUND_FX_5             5
#define SOUND_FX_6             6
#define SOUND_FX_7             7
#define SOUND_SPINNER          8
#define SOUND_FEED_BALL        9
#define SOUND_SLINGSHOT       10
#define SOUND_INLANE          11
#define SOUND_OUTHOLE_DRAIN   12
#define SOUND_TILT            13
#define SOUND_COIN            15
#define SOUND_START           16
#define SOUND_BOOT            17
#define SOUND_PHASERS         18
#define SOUND_FOUR_FX_IN_ONE  19
#define SOUND_MACHINE_GUNS    20
#define SOUND_FAST_PHASERS    21
#define SOUND_ANTICIPATION    22
#define SOUND_EXPLOSION       23

#define SOUND_EFFECT_FIRST     1
#define SOUND_EFFECT_LAST     23
#define SOUND_SONG_FIRST     101
#define SOUND_SONG_LAST      110


void setup_sounds() {
}

void play_sound(int sound) {
}

void stop_all_sounds() {
}
