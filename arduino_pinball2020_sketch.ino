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
 * Enable this to run some software tests at startup, results on serial port
 */
//#define RUN_SOFTWARE_TESTS


/**
 * Enable this to allow getting switches events from the serial port.
 */
#define RUN_SERIAL_EMULATOR

/**
 * Enable this to have the LOG() macro send data to serial port.
 */
#define LOG_ON_SERIAL_MONITOR





#include <Arduino.h>
#include <wavTrigger.h>
#include <EEPROM.h>

#include "defines.h"
#include "constants.h"
#include "bcdnum.h"
#include "event.h"

#ifdef RUN_SERIAL_EMULATOR
  #include "infrastructure/emulator.h"
#endif
#include "infrastructure/pins.h"
#include "infrastructure/audio.h"
#include "infrastructure/lamp_matrix.h"
#include "infrastructure/coils.h"
#include "infrastructure/score_display.h"
#include "infrastructure/time_keeper.h"
#include "infrastructure/switch_matrix.h"

CAudio Audio;
CLampMatrix LampMatrix;
CSwitchMatrix SwitchMatrix;
CScoreDisplay ScoreDisplay;
CTimeKeeper TimeKeeper;
CCoils Coils;
#ifdef RUN_SERIAL_EMULATOR
  EmulatorClass Emulator;
#endif

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
#include "game/ball_keeper.cpp"
#include "game/gameplay.cpp"
#include "game/attract.cpp"

#include "timers.h"
#ifdef RUN_SOFTWARE_TESTS
  #include "tests.h"
#endif



void setup() {
    FATAL_INIT();
    LOG_INIT();
    LOG("setup() starting");
    LED13_INIT();

    setup_timer_interrupts();

    #if defined(RUN_SOFTWARE_TESTS)
        run_tests();
    #endif

    noInterrupts();

    setup_timer_interrupts();

    Audio.init();
    LampMatrix.init();
    SwitchMatrix.init();
    ScoreDisplay.init();
    Coils.init();

    interrupts();

    GameSettings.load_from_eeprom();
    BallKeeper.drain_any_captured_balls();
    Attract.start();

    #ifdef RUN_SERIAL_EMULATOR
        LOG("Running Serial Emulator, send \"?\" for help");
    #endif
    LOG("setup() finished");
}

void loop() {
    // if a FATAL had happened while in an ISR, we can now handle it.
    CHECK_FATAL_IN_ISR();

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
}


