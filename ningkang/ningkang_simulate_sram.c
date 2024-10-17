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

//引脚
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
//check pin 
int check_pin_type()
{
    if((pin_1<1||pin_1>28)||(pin_2<1||pin_2>28))
    {
        return 0;
    }
    

    //vcc and adress or gnd and address
    if((pin_table[pin_1-1].type==PIN_TYPE_VCC||pin_table[pin_1-1].type==PIN_TYPE_GND)
        &&pin_table[pin_2-1].type==PIN_TYPE_ADDRESS)
    {
        return 1;
    }
    if((pin_table[pin_2-1].type==PIN_TYPE_VCC||pin_table[pin_2-1].type==PIN_TYPE_GND)
        &&pin_table[pin_1-1].type==PIN_TYPE_ADDRESS)
    {
        return 1;
    }
    //vcc and IO or gnd and IO
    if((pin_table[pin_1-1].type==PIN_TYPE_VCC||pin_table[pin_1-1].type==PIN_TYPE_GND)
        &&pin_table[pin_2-1].type==PIN_TYPE_IO)
    {
        return 1;
    }
    if((pin_table[pin_2-1].type==PIN_TYPE_VCC||pin_table[pin_2-1].type==PIN_TYPE_GND)
        &&pin_table[pin_1-1].type==PIN_TYPE_IO)
    {
        return 1;
    }

    //The distance between the two pins is large
    //Located on both sides
    if(abs(pin_1-pin_2)>1)
        return 0;
    if((pin_1==7&&pin_2==8)||(pin_1==21&&pin_2==22))
        return 0;

    //io and address
    if((pin_table[pin_1-1].type==PIN_TYPE_IO&&pin_table[pin_2-1].type==PIN_TYPE_ADDRESS)||(pin_table[pin_1-1].type==PIN_TYPE_ADDRESS&&pin_table[pin_2-1].type==PIN_TYPE_IO))
        return 1;
    //pin type different
    if(pin_table[pin_1-1].type!=pin_table[pin_2-1].type)
    {
        return 0;
    }
    
    return 1;
}


unsigned int binary_convert_decimal(const char *bin)
{
    unsigned int dec=0;
    int len=strlen(bin);
    int count=1;
    for(int i=len-1;i>=0;--i)
    {
        dec+=(bin[i]-'0')*count;
        count*=2;
    }
    return dec;
}

char* decimal_convert_binary(unsigned int dec)
{
    int len=8;
    char *bin = (char*)malloc(len+1); //  
    if (bin == NULL) {  
        return NULL;  
    }  
    for(int i=0;i<len;++i)
    {
        bin[i]='0';
    }
    bin[len]='\0';

    int i = len - 1;   
    while (dec > 0) {  
        bin[i--] = (dec % 2) + '0';  
        dec /= 2;  
    }  
     
    return bin;  
}
//input simulate pin number
void input_pin_number()
{
    printf("please input simulate Pin number:\n");
    scanf("%d %d",&pin_1,&pin_2);
}
//set pin short
void set_short(char *address_str, char *bin)
{
    int index1,index2=0;

    //one of is vcc
    if(pin_table[pin_1-1].type==PIN_TYPE_VCC||pin_table[pin_2-1].type==PIN_TYPE_VCC)
    {

        if(pin_table[pin_1 - 1].type==PIN_TYPE_VCC)
        {
            if(pin_table[pin_2-1].type==PIN_TYPE_IO)
            {
                index1=7-pin_table[pin_2-1].index;
                bin[index1]='1';
            }else{
                index1=14-pin_table[pin_2-1].index;
                address_str[index1]='1';
            }      
        }else{
            if(pin_table[pin_1-1].type==PIN_TYPE_IO)
            {
                index1=7-pin_table[pin_1-1].index;
                bin[index1]='1';
            }else{
                index1=14-pin_table[pin_1-1].index;
                address_str[index1]='1';
            } 
        }
    }//one of is gnd
    else if(pin_table[pin_1-1].type==PIN_TYPE_GND||pin_table[pin_2-1].type==PIN_TYPE_GND)
    {

        if(pin_table[pin_1 - 1].type==PIN_TYPE_GND)
        {
            if(pin_table[pin_2-1].type==PIN_TYPE_IO)
            {
                index1=7-pin_table[pin_2-1].index;
                bin[index1]='0';
            }else{
                index1=14-pin_table[pin_2-1].index;
                address_str[index1]='0';
            }      
        }else{
            if(pin_table[pin_1-1].type==PIN_TYPE_IO)
            {
                index1=7-pin_table[pin_1-1].index;
                bin[index1]='0';
            }else{
                index1=14-pin_table[pin_1-1].index;
                address_str[index1]='0';
            } 
        }
    }else if(pin_table[pin_1-1].type==PIN_TYPE_IO && pin_table[pin_2-1].type==PIN_TYPE_IO)//two io short
    {
        index1=7-pin_table[pin_1-1].index;
        index2=7-pin_table[pin_2-1].index;
        //bin[index2]=bin[index1];
        
        if(bin[index1]=='1')
        {
            bin[index2]=bin[index1];
        }else
            bin[index1]=bin[index2];
    }else if(pin_table[pin_1-1].type==PIN_TYPE_ADDRESS && pin_table[pin_2-1].type==PIN_TYPE_ADDRESS)  //two address short
    {
        index1=14-pin_table[pin_1-1].index;
        index2=14-pin_table[pin_2-1].index;
        //address_str[index2]=address_str[index1];
        if(address_str[index1]=='1')
        {
            address_str[index2]=address_str[index1];
        }else
            address_str[index1]=address_str[index2];
    }else{//io and address short
        if(pin_table[pin_1-1].type==PIN_TYPE_ADDRESS)
        {
            index1=14-pin_table[pin_1-1].index;
            index2=7-pin_table[pin_2-1].index;
            if(address_str[index1]=='1')
                bin[index2]=address_str[index1];
            else
                address_str[index1]=bin[index2];
        }else{
            index1=7-pin_table[pin_1-1].index;
            index2=14-pin_table[pin_2-1].index;
            if(bin[index1]=='1')
                address_str[index2]=bin[index1];
            else
                bin[index1]=address_str[index2];
        }
    }
}

//write
uint8_t write_to_address(char *address_real,char *date_real,char *address_str, char *bin)
{
    
    unsigned int address1=binary_convert_decimal(address_real);

    uint8_t result=binary_convert_decimal(date_real);
    printf("write to address:%s(0x%x),%s(%u)\n",address_real,address1,date_real,result);

    
    //write data
    unsigned int address=binary_convert_decimal(address_str);
    uint8_t data=binary_convert_decimal(bin);
    sram[address]=data;

    return data;
}


//read
void read_to_address(const char *address_str)
{
    unsigned int address=binary_convert_decimal(address_str);
    uint8_t result=sram[address];

    char *data=decimal_convert_binary(result);
    
    printf("read  to address:%s(0x%x),%s(%u)\n",address_str,address,data,result);
}


//check data are same or not same
void check_data(const char *address_str,const char *address_real,const char *write_data)
{
    
    unsigned int address=binary_convert_decimal(address_str);
    uint8_t result=sram[address];
    char *data=decimal_convert_binary(result);

    uint8_t w_data=binary_convert_decimal(write_data);
    
    unsigned int w_address=binary_convert_decimal(address_real);

    if(result != w_data)//check data same
    {
        printf("Writing and reading data not same!\n");
        if(pin_table[pin_1-1].type==PIN_TYPE_IO && pin_table[pin_2-1].type==PIN_TYPE_IO)
        {
            printf("two IO_PIN short:%d and %d short\n",pin_1,pin_2);
        }else if(pin_table[pin_1-1].type==PIN_TYPE_GND||pin_table[pin_2-1].type==PIN_TYPE_GND)
        {
            printf("IO_PIN and GND_PIN short:%d %d\n",pin_1,pin_2);
        }
        else if((pin_table[pin_1-1].type==PIN_TYPE_ADDRESS &&pin_table[pin_2-1].type==PIN_TYPE_IO)  || (pin_table[pin_2-1].type==PIN_TYPE_ADDRESS &&pin_table[pin_1-1].type==PIN_TYPE_IO))
        {
            printf("IO_PIN and ADDRESS_PIN short:%d %d\n",pin_1,pin_2);
        }
        else
            printf("not %d and %d short cause!\n",pin_1,pin_2);
    }else if(w_address != address){//check address same
        if(pin_table[pin_1-1].type==PIN_TYPE_VCC||pin_table[pin_2-1].type==PIN_TYPE_VCC)
        {
            printf("ADDRESS_PIN and VCC_PIN short:%d %d\n",pin_1,pin_2);
        }else if(pin_table[pin_1-1].type==PIN_TYPE_ADDRESS && pin_table[pin_2-1].type==PIN_TYPE_IO)
            printf("IO_PIN and ADDRESS_PIN short:%d %d\n",pin_1,pin_2);
        else
            printf("two ADDRESS_PIN short:%d %d\n",pin_1,pin_2);
    }else{
        printf("Writing and reading data are the same!\n");
    }
}

void test()
{
    init_pin_table();
    char address[16];//input address
    char data[9];//input data
    char write_data[9];//Store original data
    char write_address[16];//Store original address

    while(1)
    {
        printf("Please input address and data:\n");
        scanf("%s %s",address,data);
        strcpy(write_data,data);
        strcpy(write_address,address);
        
        input_pin_number();
        //pin check
        if(check_pin_type()==0)
        {
            printf("Pin type check not pass!\n");
            printf("--------------------\n");
            continue;
        }
        //simulate short
        set_short(address,data);
        write_to_address(write_address,write_data,address,data);
        
        read_to_address(address);

        check_data(address,write_address,write_data);
        printf("--------------------\n");
    }
}
int main()
{
    test();
    return 0;
}
