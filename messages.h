#ifndef MESSAGES_H
#define MESSAGES_H

#include <avr/pgmspace.h>



const char _eeprom_string_0[] PROGMEM = "String 0"; // "String 0" etc are strings to store - change to suit.
const char _eeprom_string_1[] PROGMEM = "String 1";
const char _eeprom_string_2[] PROGMEM = "String 2";
const char _eeprom_string_3[] PROGMEM = "String 3";
const char _eeprom_string_4[] PROGMEM = "sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf sadfkajsdklahsdflkjsadjkf";

const char *const _eeprom_strings_table[] PROGMEM = {
    _eeprom_string_0,
    _eeprom_string_1,
    _eeprom_string_2,
    _eeprom_string_3,
    _eeprom_string_4
};


void get_progmem_message(byte message_no, char *output, byte output_size)
{
    strncpy_P(output, (char *)pgm_read_word(&(_eeprom_strings_table[message_no])), output_size);
}




#endif // MESSAGES_H
