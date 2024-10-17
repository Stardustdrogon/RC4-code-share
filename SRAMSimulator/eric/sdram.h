/*  This file defines sdram pins for Infeneon CY52256 */

enum pintype{   /* 4 types of IC pins */
        POWER_GND,
        POWER_VCC,
        ADDR_PIN,
        DATA_PIN,
        CTRL_PIN
};

enum short_case{
        NO_HANDLE,
        VCC_ADDR_DATA,
        GND_ADDR_DATA,
        ADDR_DATA,
};

enum short_case pin_combination[5][5]={   /* 2 adjacent pins combinations and the interference case */
        /* POWER_GND,   POWER_VCC,      ADDR_PIN,       DATA_PIN,       CTRL_PIN */
/*GND*/ {NO_HANDLE,     NO_HANDLE,      GND_ADDR_DATA,  GND_ADDR_DATA,  NO_HANDLE},
/*VCC*/ {NO_HANDLE,     NO_HANDLE,      VCC_ADDR_DATA,  VCC_ADDR_DATA,  NO_HANDLE},
/*ADDR*/{GND_ADDR_DATA, VCC_ADDR_DATA,  ADDR_DATA,      ADDR_DATA,      NO_HANDLE},
/*DATA*/{GND_ADDR_DATA, VCC_ADDR_DATA,  ADDR_DATA,      ADDR_DATA,      NO_HANDLE},
/*CTRL*/{NO_HANDLE,     NO_HANDLE,      NO_HANDLE,      NO_HANDLE,      NO_HANDLE}
};


struct emu_pin{
        enum pintype ptype;
        int seq_no;       /* addr/data pin number */
        int next_pin;  /* neighbor pin number */
        int value;        /* addr/data pin value */
};

struct emu_pin pin_no[]={
        {0, 0, 0, 0},
        {ADDR_PIN, 5, 2, 0}, /* pin1 = A5 */
        {ADDR_PIN, 6, 3, 0}, /* pin2 = A6 */
        {ADDR_PIN, 7, 4, 0}, /* pin3 = A7 */
        {ADDR_PIN, 8, 5, 0}, /* pin4 = A8 */
        {ADDR_PIN, 9, 6, 0}, /* pin5 = A9 */
        {ADDR_PIN, 10, 7, 0}, /* pin6 = A10 */
        {ADDR_PIN, 11, 0, 0}, /* pin7 = A11 */
        {ADDR_PIN, 12, 9, 0}, /* pin8 = A12 */
        {ADDR_PIN, 13, 10, 0}, /* pin9 = A13 */
        {ADDR_PIN, 14, 11, 0}, /* pin10 = A14 */
        {DATA_PIN, 0, 12, 0},  /* pin11 = D0 */
        {DATA_PIN, 1, 13, 0},  /* pin12 = D1 */
        {DATA_PIN, 2, 14, 0},  /* pin13 = D2 */
        {POWER_GND, 0, 15, 0}, /* pin14 = GND */
        {DATA_PIN, 3, 16, 0},  /* pin15 = D3 */
        {DATA_PIN, 4, 17, 0},  /* pin16 = D4 */
        {DATA_PIN, 5, 18, 0},  /* pin17 = D5 */
        {DATA_PIN, 6, 19, 0},  /* pin18 = D6 */
        {DATA_PIN, 7, 20, 0},  /* pin19 = D7 */
        {CTRL_PIN, 0, 21, 0},  /* pin20 = CE */
        {ADDR_PIN, 0, 0, 0},   /* pin21 = A0 */
        {CTRL_PIN, 0, 23, 0},  /* pin22 = OE */
        {ADDR_PIN, 1, 24, 0},  /* pin23 = A1 */
        {ADDR_PIN, 2, 25, 0},  /* pin24 = A2 */
        {ADDR_PIN, 3, 26, 0},  /* pin25 = A3 */
        {ADDR_PIN, 4, 27, 0},  /* pin26 = A4 */
        {CTRL_PIN, 0, 28, 0},  /* pin27 = WE */
        {POWER_VCC, 0, 1, 0}  /* pin28 = Vcc */
};

int addr_pins[]={
        21, 23, 24, 25, 26, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int data_pins[]={
        11, 12, 13, 15, 16, 17, 18, 19};

#define FIND_SHORT_CASE(p1, p2) (pin_combination[(int)pin_no[p1].ptype][(int)pin_no[p2].ptype])
