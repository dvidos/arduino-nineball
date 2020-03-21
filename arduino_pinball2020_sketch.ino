/**
 * Created by Dimitris Vidos, for a custom Stern Nineball pinball machine.
 * All rights reserved.
 *
 * All .ino and .pde files in the sketch folder (shown in the IDE as tabs with no extension)
 * are concatenated together, starting with the file that matches the folder name followed
 * by the others in alphabetical order, and the .cpp extension is added to the filename.
 *
 * See also: https://github.com/arduino/Arduino/wiki/Build-Process
 */


/**
 * Whether we want to LOG() onto the serial monitor.
 * Commenting this out makes the LOG() macro to do nothing.
 */
#define LOG_ON_SERIAL_MONITOR

/**
 * Software tests is a function that runs various unit tests.
 * Results logged in serial monitor
 *
 * Expected hardware: USB cable only
 */
#define RUN_SOFTWARE_TESTS

/**
 * Hardware tests is a small applet to run various operations,
 * mainly verifying the cooperation of software and hardware.
 *
 * Expected hardware: A0-A7: buttons to earth, A8-A15: LEDs
 */
//#define RUN_HARDWARE_TESTS

/**
 * Serial emulator is a special mode, where characters from the Serial port
 * are taken as switches in the switch matrix. Therefore,
 * the game can be "emulated" to be played, with the messages
 * appearing on the serial monitor.
 *
 * Expected hardware: None, USB cable only.
 */
//#define RUN_SERIAL_EMULATOR




#include <Arduino.h>
#include <wavTrigger.h>
#include <EEPROM.h>
#include "defines.h"
#include "constants.h"
#include "bcdnum.h"
#include "event.h"

#include "infrastructure/pins.h"
#include "infrastructure/audio.h"
#include "infrastructure/lamp_matrix.h"
#include "infrastructure/switch_matrix.h"
#include "infrastructure/coils.h"
#include "infrastructure/score_display.h"
#include "infrastructure/time_keeper.h"

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CScoreDisplay ScoreDisplay;
CTimeKeeper TimeKeeper;
CCoils Coils;

#include "game/animations.h"
#include "game/game_settings.h"
#include "game/ball_keeper.h"
#include "game/bonus_multiplier.h"
#include "game/three_bank_targets.h"
#include "game/eight_bank_targets.h"
#include "game/loop_target.h"
#include "game/spinner.h"
#include "game/gameplay.h"
#include "game/attract.h"

CGameSettings GameSettings;
CAnimator Animator;
CAttract Attract;
CGameplay Gameplay;
LoopTargetClass LoopTarget;
SpinnerClass Spinner;
BonusMultiplierClass BonusMultiplier;
ThreeBankTargetsClass ThreeBankTargets;
EightBankTargetsClass EightBankTargets;
BallKeeperClass BallKeeper;

#include "game/bonus_multiplier.cpp"
#include "game/three_bank_targets.cpp"
#include "game/eight_bank_targets.cpp"
#include "game/loop_target.cpp"
#include "game/spinner.cpp"
#include "game/gameplay.cpp"
#include "game/attract.cpp"

#include "timers.h"
#include "tests_software.h"
#include "tests_hardware.h"

/**
 * Declaring global objects affords us avoiding malloc() and nasty out-of-memory surprises
 */



void setup() {
    FATAL_INIT();
    LOG_INIT();
    LOG("setup() starting");
    LED13_INIT();


    #if defined(RUN_SOFTWARE_TESTS)
        setup_timer_interrupts();
        run_software_tests();

    #elif defined(RUN_HARDWARE_TESTS)
        hardware_tests_init();

    #else
        noInterrupts();

        setup_timer_interrupts();

        Audio.init();
        LampMatrix.init();
        SwitchMatrix.init();
        ScoreDisplay.init();
        Coils.init();

        interrupts();

        GameSettings.load_from_eeprom();
        Attract.start();
    #endif

    LOG("setup() finished");
}

void loop() {
    CHECK_FATAL_IN_ISR();

    #if defined(RUN_SOFTWARE_TESTS)
        // we need to keep consuming timeout events,
        // because we are auto-generating recurring timing events (e.g. every second)
        // and we'll run out of space to store them.
        byte timeout_no;
        TimeKeeper.get_next_expiration_event(&timeout_no);

        ; // nothing
    #elif defined(RUN_HARDWARE_TESTS)
        hardware_tests_tick();
    #else
        // get event from switch matrix and timeouts
        // ask attract to handle it.
        // this way, we can run simulations with streams of events, checking the responses of the software.
        byte number;
        byte was_pressed;
        Event e;

        if (SwitchMatrix.get_next_switch_event(&number, &was_pressed))
        {
            e.type = was_pressed ? switch_closed : switch_opened;
            e.number = number;
            Attract.handle_event(e);
        }
        else if (TimeKeeper.get_next_expiration_event(&number))
        {
            e.type = timeout_expired;
            e.number = number;
            Attract.handle_event(e);
        }

        // otherwise nothing, maybe we could set a flag of FATAL_WITHIN_INTERRUPT,
        // and handle it here, in "user" space.
    #endif
}


