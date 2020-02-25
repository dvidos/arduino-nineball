# Nineball Pinball Project

This is a project where an Arduino mega is used to recreate the [Stern Nineball][1] pinball machine with custom hardware.

The story started around 1990, where a Stern Nineball [playfield][2] was found in a trashcan and salvaged. After being hung on my wall
for years, on 2007 I tried to rebuild the rest of the machine with custom hardware and sofrware.

The project stalled for lack of funds and time. On 2020, it got restarted. A new microcontroller was chosen (an Arduino Mega)
and this repository holds the code for this project.

## Hardware

The hardware connected to this Arduino mega is:

* A 8 x 8 lamp matrix of #44 lamps, powered through power transistors
* A 5 x 8 switch matrix for all the switches on the playfield
* A collection of 10 to 15 high powered coils (+45 VDC) controlled through power transistors
* Two seven digits 7-segments displays for displaying score
* The SparkFun WavTrigger board for sound effects and music
* A small "T" relay for energizing the flippers and pop bumpers when gameplay begins

## Software

Essentially the software tries to emulate the original game, as described in the [game manual][3].
Since this is a machine for fun and not for profit, a lot of coin related options are left out.


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




[1]: https://www.ipdb.org/machine.cgi?id=1678
[2]: https://www.ipdb.org/images/1678/image-4.jpg
[3]: https://www.ipdb.org/files/1678/Stern_1980_Nine_Ball_Manual.pdf
  
