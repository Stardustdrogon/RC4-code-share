#include "sram.h"



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
