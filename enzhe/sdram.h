// Set chip parameters
#define MEMORY_SIZE 32768 // 32K x 8 RAM
#define PIN_NUMBER 28
#define ADDRESS_PIN 15
#define DATA_PIN 8

// Define PIN   
typedef enum
{
    PIN_TYPE_VCC,
    PIN_TYPE_GND,
    PIN_TYPE_CONTROL,
    PIN_TYPE_ADDRESS,
    PIN_TYPE_DATA
} pin_type;

typedef enum
{
    PIN_SIDE_LEFT,
    PIN_SIDE_RIGHT
} pin_side;

typedef struct
{
    pin_type type;
    pin_side side;
    int index;
} PIN;