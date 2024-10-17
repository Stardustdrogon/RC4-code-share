#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "sdram.h"

// Define chip
typedef struct
{
    // Simulate RAM
    uint8_t memory[MEMORY_SIZE];
    PIN pins[PIN_NUMBER];
    // Vcc & GND, Used to simulate short-circuit
    int pin_vcc, pin_gnd;
    // Storing and binary calculations
    int addressArr[ADDRESS_PIN];
    int dataArr[DATA_PIN];
    // short-circuit pins
    int short_pin1, short_pin2;
} Chip;

// Primary function prototypes
void init_chip(Chip* chip);
void set_short_pin(Chip* chip, int short_pin1, int short_pin2);
bool judge_short_legal(Chip* chip, int short_pin1, int short_pin2);
void pin_short_circuit(Chip* chip, int short_pin1, int short_pin2);
void write_memory(Chip* chip, const char *addressStr, const char *dataStr);
void read_memory(Chip* chip, const char *addressStr);
void print_binary_data(uint8_t data);

int main()
{
    Chip* chip = (Chip*)malloc(sizeof(Chip));
    init_chip(chip);

    printf("operation format:\n");
    printf("1.Setting Short PIN: s pin1 pin2\n");
    printf("2.Write Memory Data: w addr data\n");
    printf("3.Read Memory Data : r addr \n");
    printf("4.End the program  : e \n");

    while(1)
    {
        // Buffer to receive three types of data
        char buffer[256];
        char operation;
        // Character arrays to store input binary
        char addressStr[ADDRESS_PIN + 1];
        char dataStr[DATA_PIN + 1];
        
        // Refresh register
        for(int i = 0; i < ADDRESS_PIN; i++)
        {
            addressStr[i] = '0';
        }
        for(int i = 0; i < DATA_PIN; i++)
        {
            dataStr[i] = '0';
        }

        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%c", &operation);

        switch (operation)
        {
            case 's':
                sscanf(buffer + 1, " %d %d", &chip -> short_pin1, &chip -> short_pin2);
                set_short_pin(chip, chip -> short_pin1, chip -> short_pin2);
                break;
            case 'w':
                sscanf(buffer + 1, "%15s %8s", &addressStr, &dataStr);
                write_memory(chip, addressStr, dataStr);
                break;
            case 'r':
                sscanf(buffer + 1, " %15s", &addressStr);
                read_memory(chip, addressStr);
                break;
            case 'e':
                return 0;
            default:
                printf("Please enter the correct opreation!\n");
                break;
        }

    }
    return 0;
}

void init_chip(Chip* chip)
{
    // Init PIN
    chip -> pins[1].type = PIN_TYPE_ADDRESS,  chip -> pins[1].side = PIN_SIDE_LEFT;   chip -> pins[1].index = 5;
    chip -> pins[2].type = PIN_TYPE_ADDRESS,  chip -> pins[2].side = PIN_SIDE_LEFT;   chip -> pins[2].index = 6;
    chip -> pins[3].type = PIN_TYPE_ADDRESS,  chip -> pins[3].side = PIN_SIDE_LEFT;   chip -> pins[3].index = 7;
    chip -> pins[4].type = PIN_TYPE_ADDRESS,  chip -> pins[4].side = PIN_SIDE_LEFT;   chip -> pins[4].index = 8;
    chip -> pins[5].type = PIN_TYPE_ADDRESS,  chip -> pins[5].side = PIN_SIDE_LEFT;   chip -> pins[5].index = 9;
    chip -> pins[6].type = PIN_TYPE_ADDRESS,  chip -> pins[6].side = PIN_SIDE_LEFT;   chip -> pins[6].index = 10;
    chip -> pins[7].type = PIN_TYPE_ADDRESS,  chip -> pins[7].side = PIN_SIDE_LEFT;   chip -> pins[7].index = 11;
    chip -> pins[8].type = PIN_TYPE_ADDRESS,  chip -> pins[8].side = PIN_SIDE_RIGHT;  chip -> pins[8].index = 12;
    chip -> pins[9].type = PIN_TYPE_ADDRESS,  chip -> pins[9].side = PIN_SIDE_RIGHT;  chip -> pins[9].index = 13;
    chip -> pins[10].type = PIN_TYPE_ADDRESS, chip -> pins[10].side = PIN_SIDE_RIGHT; chip -> pins[10].index = 14;
    chip -> pins[11].type = PIN_TYPE_DATA,    chip -> pins[11].side = PIN_SIDE_RIGHT; chip -> pins[11].index = 0;
    chip -> pins[12].type = PIN_TYPE_DATA,    chip -> pins[12].side = PIN_SIDE_RIGHT; chip -> pins[12].index = 1;
    chip -> pins[13].type = PIN_TYPE_DATA,    chip -> pins[13].side = PIN_SIDE_RIGHT; chip -> pins[13].index = 2;
    chip -> pins[14].type = PIN_TYPE_GND,     chip -> pins[14].side = PIN_SIDE_RIGHT; chip -> pins[14].index = 0;
    chip -> pins[15].type = PIN_TYPE_DATA,    chip -> pins[15].side = PIN_SIDE_RIGHT; chip -> pins[15].index = 3;
    chip -> pins[16].type = PIN_TYPE_DATA,    chip -> pins[16].side = PIN_SIDE_RIGHT; chip -> pins[16].index = 4;
    chip -> pins[17].type = PIN_TYPE_DATA,    chip -> pins[17].side = PIN_SIDE_RIGHT; chip -> pins[17].index = 5;
    chip -> pins[18].type = PIN_TYPE_DATA,    chip -> pins[18].side = PIN_SIDE_RIGHT; chip -> pins[18].index = 6;
    chip -> pins[19].type = PIN_TYPE_DATA,    chip -> pins[19].side = PIN_SIDE_RIGHT; chip -> pins[19].index = 7;
    chip -> pins[20].type = PIN_TYPE_CONTROL, chip -> pins[20].side = PIN_SIDE_RIGHT; chip -> pins[20].index = 0;
    chip -> pins[21].type = PIN_TYPE_ADDRESS, chip -> pins[21].side = PIN_SIDE_RIGHT; chip -> pins[21].index = 0;
    chip -> pins[22].type = PIN_TYPE_CONTROL, chip -> pins[22].side = PIN_SIDE_LEFT;  chip -> pins[22].index = 0;
    chip -> pins[23].type = PIN_TYPE_ADDRESS, chip -> pins[23].side = PIN_SIDE_LEFT;  chip -> pins[23].index = 1;
    chip -> pins[24].type = PIN_TYPE_ADDRESS, chip -> pins[24].side = PIN_SIDE_LEFT;  chip -> pins[24].index = 2;
    chip -> pins[25].type = PIN_TYPE_ADDRESS, chip -> pins[25].side = PIN_SIDE_LEFT;  chip -> pins[25].index = 3;
    chip -> pins[26].type = PIN_TYPE_ADDRESS, chip -> pins[26].side = PIN_SIDE_LEFT;  chip -> pins[26].index = 4;
    chip -> pins[27].type = PIN_TYPE_CONTROL, chip -> pins[27].side = PIN_SIDE_LEFT;  chip -> pins[27].index = 0;
    chip -> pins[28].type = PIN_TYPE_VCC,     chip -> pins[28].side = PIN_SIDE_LEFT;  chip -> pins[28].index = 1;

    chip -> pin_vcc = 1;
    chip -> pin_gnd = 0;

    // Init Memory
    for(int i = 0; i < MEMORY_SIZE; i++)
    {
        chip -> memory[i] = 0;
    }
    for(int i = 0; i < ADDRESS_PIN; i++)
    {
        chip -> addressArr[i] = 0;
    }
    for(int i = 0; i < DATA_PIN; i++)
    {
        chip -> dataArr[i] = 0;
    }
    return;
}

// get pin infomation
const char* get_pin_type(pin_type type)
{
    switch(type)
    {
        case PIN_TYPE_VCC: return "VCC";
        case PIN_TYPE_GND: return "GND";
        case PIN_TYPE_CONTROL: return "CONTROL";
        case PIN_TYPE_ADDRESS: return "ADDRESS";
        case PIN_TYPE_DATA: return "DATA";
        default: return "ERROR";
    }
}
const char* get_pin_side(pin_side side)
{
    switch(side)
    {
        case PIN_SIDE_LEFT: return "LEFT";
        case PIN_SIDE_RIGHT: return "RIGHT";
        default: return "ERROR";
    }
}

// Presume short-circuit
void set_short_pin(Chip* chip, int short_pin1, int short_pin2)
{
    if(!judge_short_legal(chip, chip -> short_pin1, chip -> short_pin2))
    {
        printf("The short-circuit illegal, please enter again:\n");
        return;
    }

//    /*
    printf("%d %d are legal!\n", short_pin1, short_pin2);
    // print pin infomation
    printf("pin1: %s %s %d\n", get_pin_type(chip -> pins[short_pin1].type), get_pin_side(chip -> pins[short_pin1].side), chip -> pins[short_pin1].index);
    printf("pin2: %s %s %d\n", get_pin_type(chip -> pins[short_pin2].type), get_pin_side(chip -> pins[short_pin2].side), chip -> pins[short_pin2].index);
    printf("\n");
//    */
}

// cheak short-circuit
bool judge_short_legal(Chip* chip, int short_pin1, int short_pin2)
{
    // 1. Adjacent is true
    // and pin28 & pin1 is adjacent
    if(abs(short_pin1 - short_pin2) % 26 != 1)
    {
        return false;
    }
    // 2. Control pin is illegal
    if(chip -> pins[short_pin1].type == PIN_TYPE_CONTROL || chip -> pins[short_pin2].type == PIN_TYPE_CONTROL)
    {
        return false;
    }
    // 3. Only pins on the same side can be adjacent
    if(chip -> pins[short_pin1].side != chip -> pins[short_pin2].side)
    {
        return false;
    }
    return true;
}

// Find the address for pin level
int *find_pin_addr(Chip* chip, int pin_number)
{
    if(chip -> pins[pin_number].type == PIN_TYPE_VCC)
        return &chip -> pin_vcc;
    if(chip -> pins[pin_number].type == PIN_TYPE_GND)
        return &chip -> pin_gnd;
    // Convert index
    if(chip -> pins[pin_number].type == PIN_TYPE_ADDRESS)
    {
        return &chip -> addressArr[ADDRESS_PIN - 1 - chip -> pins[pin_number].index];
    }
    if(chip -> pins[pin_number].type == PIN_TYPE_DATA)
    {
        return &chip -> dataArr[DATA_PIN - 1 - chip -> pins[pin_number].index];
    }

    // Pin illegal
    printf("pin type is illegal!\n");
    return NULL;
}

// simulate short-circuit
void pin_short_circuit(Chip* chip, int short_pin1, int short_pin2)
{
    int *pin1_addr = find_pin_addr(chip, short_pin1);
    int *pin2_addr = find_pin_addr(chip, short_pin2);
    // OR operation
    int result = *pin1_addr | *pin2_addr;
    *pin1_addr = result;
    *pin2_addr = result;
    if(pin1_addr == &chip -> pin_gnd || pin2_addr == &chip -> pin_gnd)
    {
        *pin1_addr = 0;
        *pin2_addr = 0;
    }
}

unsigned int arr_to_uint(const int *arr, int length)
{
    unsigned int value = 0;
    for (int i = 0; i < length; i++)
    {
        value = (value << 1) | arr[i];
    }
    return value;
}

// simulate write
void write_memory(Chip* chip, const char *addressStr, const char *dataStr)
{
    for(int i = 0; i < ADDRESS_PIN; i++)
    {
        chip -> addressArr[i] = addressStr[i] - '0';
    }
    for(int i = 0; i < DATA_PIN; i++)
    {
        chip -> dataArr[i] = dataStr[i] - '0';
    }
    pin_short_circuit(chip, chip -> short_pin1, chip -> short_pin2);

    // 15bit address translation row & column
    int address = arr_to_uint(chip -> addressArr, ADDRESS_PIN);
    if(address >= MEMORY_SIZE)
    {
        printf("Address out of range.\n");
        return;
    }
    chip -> memory[address] = arr_to_uint(chip -> dataArr, DATA_PIN);
    printf("Write operation complete.\n\n");
}

// simulate read
void read_memory(Chip* chip, const char *addressStr)
{
    for(int i = 0; i < ADDRESS_PIN; i++)
    {
        chip -> addressArr[i] = addressStr[i] - '0';
    }
    pin_short_circuit(chip, chip -> short_pin1, chip -> short_pin2);
    
    // Translation
    int address = arr_to_uint(chip -> addressArr, ADDRESS_PIN);
    if(address >= MEMORY_SIZE)
    {
        printf("Address out of range.\n");
        return;
    }
    uint8_t data = chip -> memory[address];

    printf("Read binary data: ");
    print_binary_data(data);
    printf("\n\n");
}

void print_binary_data(uint8_t data)
{
    for (int i = DATA_PIN - 1; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
}
