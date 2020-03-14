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


## Run Modes

The program can run on three modes:

* `RUN_SOFTWARE_TESTS` - runs unit tests, presenting output on the serial monitor
* `RUN_HARDWARE_TESTS` - runs a small applet, allowing verification of various functionalities
* no defines - runs the normal pinball sofrware, as supposed to.

## Maintenability

Unfortunately, this is prey to the shallow entry curve of Arduino. Yes, it worked well since the first time I plugged it in.
But now I cannot add the WavLibrary to the source control. I cannot modify the makefile.
I cannot build using custom or open source tools.
So, I guess, this is the price to pay for so easy to get-started platform.

I may try to find a way to build from makefile.


## To Do

* Initialize Pins (I/O)
* Construct and test switch matrix
* Connect and test audio
* Implement 4-player game.
* Implement Eva Happy Hour mode
* Implement score display and updates in increments every 100 msec.
* Write documentation for hardware, as we go.
* Create connectors for the various pieces.
*

# The menu

When the machine is booted up, we can start a new game using the Start button.
But we can also enter the menu system via the P1 button (left button under the
machine body). The selected option is displayed on the score display. Pressing
Start takes us inside the menu. Inside a menu, P2 (the right button under the
machine body) cycles through the options

The menu options are:

| No | Title | Comment |
| -- | ----- | ------- |
| 1 | Regular game | can also press Start directly |
| 2 | Eva's Happy Hour | infinite balls game |
| 3 | Radio / Jukebox | play random songs |
| 4 | Adjust machine settings | |
| 5 | Diagnostics | |

The available settings are:

| No | Description | Options |
| -- | ----------- | ------- |
| 01 | Highscore to date | pressing P2 three times resets to zero |
| 02 | Profile #1 highscore to date | pressing P2 three times resets to zero |
| 03 | Profile #2 highscore to date | pressing P2 three times resets to zero |
| 04 | Profile #3 highscore to date | pressing P2 three times resets to zero |
| 05 | Balls served to date | pressing P2 three times resets to zero |
| 06 | Games played to date | pressing P2 three times resets to zero |
| 07 | Highscore 1 threshold | P2 increases score by 100K |
| 08 | Highscore 2 threshold | P2 increases score by 100K |
| 09 | Highscore 3 threshold | P2 increases score by 100K |
| 10 | Background sounds | 0/1 for off/on |
| 11 | Background music | 0/1 for off/on |
| 12 | Balls per game | 3 or 5 |
| 13 | Spot light strategy | 0 for conservative, 1 for liberal |
| 14 | Multiplier step up | 0 for both 3-banks, 1 for any 3-bank |
| 15 | Advance spinner value | 0 for center target, 1 for any outside target |
| 16 | Eight bank WOW starts | 0 on making No 9, 1 on making number 8 |
| 17 | Super Bonus lights | 0 on making No 9, 1 on making number 8 |
| 18 | Three bank WOW starts | 0 on 7x multiplier, 1 on 6x and on 7x multiplier |
| 19 | Wow award type | 0 70K, 1 shoot again |
| 20 | Special award type | 0 90K, 1 130K, 2 shoot again, 3 Nothing |
| 21 | Specials limitation | 0 one per ball, 1 unlimited per ball |
| 22 | Save and exit | P2 to save and exit |

The available diagnostics are:

| No | Description | Options |
| -- | ----------- | ------- |
| 01 | Blink lamp matrix columns | P2 selects the column number |
| 02 | Blink lamp matrix rows | P2 selects the row number |
| 03 | Blink on lamp only | P2 selects the lamp. Number of lamp is displayed |
| 04 | All score displays show same number | P2 selects which digit [0-9] |
| 05 | Each score display shows no 8 | P2 selects which display [0-15] |
| 06 | Show closed switch | Display shows no of first closed switch |
| 07 | Fire coils | P2 selects, diszplays and fires the next coil |
| 08 | Set flippers relay | P2 selects state of flippers relay 0/1 for off/on |
| 09 | Trigger sounds | P2 selects, displays and triggers the next sound |






[1]: https://www.ipdb.org/machine.cgi?id=1678
[2]: https://www.ipdb.org/images/1678/image-4.jpg
[3]: https://www.ipdb.org/files/1678/Stern_1980_Nine_Ball_Manual.pdf


