#include <stdio.h>
#include <stdlib.h>
#include "sdram.h"

//#define DEBUG_ON

unsigned char SDRAM_BUF[32768];  /* emulated buffer */

void dump_pins()
{
#ifdef DEBUG_ON
        int i;

        for (i=1; i<sizeof(pin_no)/sizeof(pin_no[0]); i++)
                printf("pin%d : %d, %d, %d, [%d]\n", i, pin_no[i].ptype, pin_no[i].seq_no, pin_no[i].next_pin, pin_no[i].value);
#endif
}

void set_addr_value(int addr_value)
{
        int index;

        for (index=0; index<sizeof(addr_pins)/sizeof(addr_pins[0]); index++)
        {
                pin_no[addr_pins[index]].value=addr_value & 1;
                addr_value>>=1;
        }
}

int get_addr_value()
{
        int index;
        int value=0;

        for (index=sizeof(addr_pins)/sizeof(addr_pins[0])-1; index>=0; index--)
        {
//              printf("index=%x value=%x\n",index,value);
                value<<=1;
                value|=pin_no[addr_pins[index]].value;
        }
        return value;
}

void set_data_value(int data_value)
{
        int index;

        for (index=0; index<sizeof(data_pins)/sizeof(data_pins[0]); index++)
        {
                pin_no[data_pins[index]].value=data_value & 1;
                data_value>>=1;
        }
}

int get_data_value()
{
        int index;
        int value=0;

        for (index=sizeof(data_pins)/sizeof(data_pins[0])-1; index>=0; index--)
        {
                value<<=1;
                value|=pin_no[data_pins[index]].value;
        }
        return value;
}

void apply_short_pin(int pin1, int pin2)
{
        enum short_case sh_case;

        sh_case = FIND_SHORT_CASE(pin1, pin2);

        switch (sh_case)
        {
                case NO_HANDLE :
                        break;
                case VCC_ADDR_DATA:  /* Vcc short to address or data pin, all set to 1 */
                        pin_no[pin1].value=1;
                        pin_no[pin2].value=1;
                        break;
                case GND_ADDR_DATA:  /* GND short to address or data pin, all set to 0 */
                        pin_no[pin1].value=0;
                        pin_no[pin2].value=0;
                        break;
                case ADDR_DATA:      /* address or data pin short */
                        if ((pin_no[pin1].value==1 && pin_no[pin2].value==0) ||
                            (pin_no[pin1].value==0 && pin_no[pin2].value==1))
                        {  /* if both pin have different values, the output will be affected */
                                pin_no[pin1].value=1;
                                pin_no[pin2].value=1;
                        }
                        break;
        }
}

int update_sdram(int addr_value, int data_value, int pin1, int pin2)
{
        /* set the address and data pins */
        set_addr_value(addr_value);
        dump_pins();
        set_data_value(data_value);
        dump_pins();
        apply_short_pin(pin1, pin2);
        dump_pins();
        SDRAM_BUF[get_addr_value()]=get_data_value();
//#ifdef DEBUG_ON
        printf("update sdram addr %x to value %d\n", get_addr_value(), get_data_value());
//#endif

}

int read_sdram(int addr_value, int pin1, int pin2)
{
        int result;

        set_addr_value(addr_value);
        dump_pins();
        apply_short_pin(pin1, pin2);
        dump_pins();
        result=SDRAM_BUF[get_addr_value()];
        set_data_value(result);
        dump_pins();
        apply_short_pin(pin1, pin2);
        dump_pins();
//#ifdef DEBUG_ON
        printf("read sdram addr %x get value %d\n", addr_value, get_data_value());
//#endif
        return get_data_value();
}


int main(int argc, char **argv)
{

        int pin1, pin2;
        int result;

        if (argc<3)
        {
                fprintf(stderr, "Usage :\n     %s pin_number1 pin_number2\n",argv[0]);
                exit(-1);
        }

        pin1=atoi(argv[1]);
        pin2=atoi(argv[2]);

        if ((pin1<1) || (pin1>sizeof(pin_no)/sizeof(pin_no[0])) || (pin2<1) || (pin2>sizeof(pin_no)/sizeof(pin_no[0])))
        {
                fprintf(stderr, "Usage :\n     %s pin_number1 pin_number2\n",argv[0]);
                exit(-1);
        }

        if ((pin_no[pin1].next_pin!=pin2) && (pin_no[pin2].next_pin!=pin1))   /* if both pins are not neighbor, quit */
        {
                fprintf(stderr, "Error : Pin %d and pin %d are not next to each other\n",pin1, pin2);
                exit(-1);
        }

        if (FIND_SHORT_CASE(pin1, pin2)==NO_HANDLE)
        {
                fprintf(stderr, "Error : Cannot handle pin %d and pin %d case\n",pin1, pin2);
                exit(-1);
        }

        //update_sdram(0x310, 0xff, pin1, pin2);
        //result=read_sdram(0x310, pin1, pin2);

        {
                int i,j;

                for (i=0; i<256; i++)
                        update_sdram(i, i&0xff, pin1, pin2);


                for (i=0; i<256; i++)
                        if ((result=read_sdram(i, pin1, pin2))!=i & 0xff)
                                printf("error in addr %x value %x\n",i,result);
        }
}
