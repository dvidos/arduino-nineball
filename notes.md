# Nineball Pinball Project

## Notes

### menu selection

pushing the key under the pinbal, we shuold cycle the various modes, pushing start would start a mode.
the second button will cycle the item in that menu

0 = normal play (e.g. what start will do if no black button is pressed)
1 = happy mode, infinite balls
2 = practice mode, specific targets.
...
8 = settings menu
9 = diagnostics menu


### watchdog

if the pin is free, have the led being lit every two seconds or so, 
to understand whether we fell in an infinite loop or something.



### variables

keep everything in 3-4 large structures with bit fields 
to preserve space and allow for easy reporting of sizeof()


### timing

should keep time in 10 or 100 msecs increments. 
A char would allow up to 2.5 seconds or 25 seconds duration which is good for coils, animations, lamps etc.

Should have a way of keeping some stats, such as max lamps blinking at the time and max switches queue length.


I want to find a way to automate lamp animations, even at the application level,
without having to deal with them in the state machine. We should be able to 
"launchAnimation(x)" and it should work to completion, upon which completion, 
we could be notified (similar to a timeout) and we would update the lamps as should be.

Sameway, we might be able to be notified when a sound has finished.
Maybe we will do this for long songs, set a long timer, having hardcoded the number of seconds
the song is long. 


### Sound

https://create.arduino.cc/projecthub/BobB/pinball-sounds-and-music-bac92b

https://github.com/robertsonics/WAV-Trigger-Arduino-Serial-Library/blob/master/examples/WTrigBasic/WTrigBasic.ino#L28

It seems I can use one of the internal I/O ports of the Mega, like 18/19.
https://github.com/robertsonics/WAV-Trigger-Arduino-Serial-Library/blob/master/wavTrigger.h#L38
Uncomment the desired one. One cable sound! Woot!




