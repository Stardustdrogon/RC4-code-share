#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>

#define SRAM_SIZE (32*1024)
#define ADDRESS_PIN 15
#define IO_PINS 8
#define CONTROL_PINS 3
#define TOTAL_PINS (ADDRESS_PIN+IO_PINS+CONTROL_PINS+2)//include VCC,GND


//pin_type
typedef enum{
    PIN_TYPE_ADDRESS,
    PIN_TYPE_IO,
    PIN_TYPE_VCC,
    PIN_TYPE_GND,
    PIN_TYPE_CE,
    PIN_TYPE_OE,
    PIN_TYPE_WE
}PinType;

//pin structure
typedef struct pin_str{
    int number;//pin number
    PinType type;//pin type
    int index;//pin index 
    
}pin_str;

//short pin number
int pin_1=0;
int pin_2=0;
//sram memory
uint8_t sram[SRAM_SIZE]={0};
//pin table
pin_str pin_table[TOTAL_PINS];
//init pin attribute
void init_pin_table()
{
    pin_table[0].number=1; pin_table[0].type=PIN_TYPE_ADDRESS; pin_table[0].index=5; 
    pin_table[1].number=2; pin_table[1].type=PIN_TYPE_ADDRESS; pin_table[1].index=6; 
    pin_table[2].number=3; pin_table[2].type=PIN_TYPE_ADDRESS; pin_table[2].index=7; 
    pin_table[3].number=4; pin_table[3].type=PIN_TYPE_ADDRESS; pin_table[3].index=8; 
    pin_table[4].number=5; pin_table[4].type=PIN_TYPE_ADDRESS; pin_table[4].index=9; 
    pin_table[5].number=6; pin_table[5].type=PIN_TYPE_ADDRESS; pin_table[5].index=10; 
    pin_table[6].number=7; pin_table[6].type=PIN_TYPE_ADDRESS; pin_table[6].index=11; 

    pin_table[7].number=8; pin_table[7].type=PIN_TYPE_ADDRESS; pin_table[7].index=12; 
    pin_table[8].number=9; pin_table[8].type=PIN_TYPE_ADDRESS; pin_table[8].index=13; 
    pin_table[9].number=10; pin_table[9].type=PIN_TYPE_ADDRESS; pin_table[9].index=14;

    pin_table[10].number=11; pin_table[10].type=PIN_TYPE_IO; pin_table[10].index=0;
    pin_table[11].number=12; pin_table[11].type=PIN_TYPE_IO; pin_table[11].index=1;
    pin_table[12].number=13; pin_table[12].type=PIN_TYPE_IO; pin_table[12].index=2; 
    pin_table[13].number=14; pin_table[13].type=PIN_TYPE_GND; pin_table[13].index=0; 
    pin_table[14].number=15; pin_table[14].type=PIN_TYPE_IO; pin_table[14].index=3;
    pin_table[15].number=16; pin_table[15].type=PIN_TYPE_IO; pin_table[15].index=4;
    pin_table[16].number=17; pin_table[16].type=PIN_TYPE_IO; pin_table[16].index=5; 
    pin_table[17].number=18; pin_table[17].type=PIN_TYPE_IO; pin_table[17].index=6;
    pin_table[18].number=19; pin_table[18].type=PIN_TYPE_IO; pin_table[18].index=7; 
    pin_table[19].number=20; pin_table[19].type=PIN_TYPE_CE; pin_table[19].index=0;
    pin_table[20].number=21; pin_table[20].type=PIN_TYPE_ADDRESS; pin_table[20].index=0; 
    pin_table[21].number=22; pin_table[21].type=PIN_TYPE_OE; pin_table[21].index=0;
    
    pin_table[22].number=23; pin_table[22].type=PIN_TYPE_ADDRESS; pin_table[22].index=1; 
    pin_table[23].number=24; pin_table[23].type=PIN_TYPE_ADDRESS; pin_table[23].index=2; 
    pin_table[24].number=25; pin_table[24].type=PIN_TYPE_ADDRESS; pin_table[24].index=3; 
    pin_table[25].number=26; pin_table[25].type=PIN_TYPE_ADDRESS; pin_table[25].index=4; 

    pin_table[26].number=27; pin_table[26].type=PIN_TYPE_WE; pin_table[26].index=0; 
    pin_table[27].number=28; pin_table[27].type=PIN_TYPE_VCC; pin_table[27].index=0; 
}