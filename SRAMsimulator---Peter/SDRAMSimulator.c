/*
Program: This Program will simulate CY62256N-SRAM behavior based on setting two soldering Pins
Prerequisite:
    This program only get data from shm (TestingSRAM.c)
Input :
	 1. Reading From Shm  
     -  W Addr Data, write Data to Address 
     -  R Addr, read Data from Address
     -  "quit", end simulator program
Output: 
     1. W, no Output,but data Write to SRAM buffer.
     2. R, buffer[address] data to Shm
     3. "quit",close simulator program
History: 
     1.2024/9/30 Simulator contains testing part
     2.2024/10/7 Divide simulator to tester and Simulator
*/

#include <fcntl.h>
#include "ShMem.h"

//#define FLUSH while(getchar()!='\n');

//SRAM Init
bool ReadSolderingPin(unsigned short *SolderingPins,int num);
bool CheckCTRLPin(unsigned short *SolderingPins,int num);
bool IsSolderingPinAdjacent(unsigned short *SolderingPins,int num);
int Parsing_Command(char *SentCommand,char *Addr,char *Data);
void PinRefresh(bool *PinArray,int num);
//String to Pin 
void WritePin16To2(bool *SRAMPins,char *Str,int num);
void SolderingToPin(bool *AddrIn,bool *DataIn,unsigned short *SolderingPins);
//SDRAM R/W
void WriteToBuffer(unsigned short Addr10,unsigned short Data10,unsigned short *Buffer,unsigned int BufferSize);
void ReadBuffer(unsigned short Addr10,unsigned short *Buffer,unsigned int BufferSize);
void PrintInputPin(bool *Input,int num);
//Data type conversion
unsigned int HexToDec(char *Str,int num);//16 to 10
void DecToBinary(bool *SRAMPins,unsigned int SumOfHex);//10 to 2
unsigned short BinaryToDec(bool *SRAMPins,unsigned int num);//2 to 10
//Pin to Index
//int PinToIndex(char Type,int Pin);
//T or F
//bool IsAddrPin(short Pin);
bool IsVccPin(short Pin);
bool IsGNDPin(short Pin);
int main()
{
    //Recording which two pins was soldered
    unsigned short SolderingPin[2];
    unsigned short uAddr10,uData10;
    //Number of Pins ex : data,address,solderingPin
    int NumberOfDataPins=sizeof(DATAPINS)/sizeof(DATAPINS[0]);
    int NumberOfAddrPins=sizeof(ADDRPINS)/sizeof(ADDRPINS[0]);
    int NumberOfSolderingPin=sizeof(SolderingPin)/sizeof(SolderingPin[0]);
    //Input array for simulating SRAM Input pins , all init to zero
    bool DataInput[NumberOfDataPins];//0~7 = data bit D0~D7
    bool AddrInput[NumberOfAddrPins];//0~14 =addr bit A0~A14
    //BufferSize=2^NumberOfAddrPins
    const unsigned int BufferSize = pow(2,NumberOfAddrPins);
    //SRAM　buffer index[0~32767] = 0000~7FFF, Buffer[0~32767]=Input Data to SRAM.
    unsigned short Buffer[BufferSize];
    //Semaphore lock
    sem_t *Sem_ConSumer,*Sem_Producer;
    //Shm
    char *block;

    //Clear Strings for Semaphore
    sem_unlink(SEM_PRODUCER_FNAME);
    sem_unlink(SEM_CONSUMER_FNAME);
    //Semaphore init
    Sem_Producer=sem_open(SEM_PRODUCER_FNAME,O_CREAT,0660,0);
    if(Sem_Producer==SEM_FAILED)
    {
        perror("sem_open/read/Producer");
        exit(EXIT_FAILURE);

    }
    Sem_ConSumer=sem_open(SEM_CONSUMER_FNAME,O_CREAT,0660,1);
    if(Sem_ConSumer==SEM_FAILED)
    {
        perror("sem_open/read/Consumer");
        exit(EXIT_FAILURE);

    }
    //shared memory block Init
    block=attach_memory_block(FILENAME,BLOCK_SIZE);
    if(block==NULL)
    {
        printf("ERROR: couldn't get block\n");
        return -1;
    }
    //Buffer initialization
    for(int i=0;i<BufferSize;i++)
    {
        Buffer[i]=0;
    }
    //Simulator start
    printf("CY62256N-SRAM Simulator\n");
    printf("Please input two soldering pin number,num = %d\n",NumberOfSolderingPin);
    //Soldering Checking
    while(ReadSolderingPin(SolderingPin,NumberOfSolderingPin));
    //char ControlArgument[TestStringLen];
    char Address[NUMADDR+1];//hold hexademical for 4 char + 1 null
    char Data[NUMDATA+1];//hold hexademical for 2 char + 1 null
    //Collect R/W command
    printf("SRAM waiting for Testing Program W/R/Testing Command\n");
    bool done=true;
    while(done)
    {
            //Producer Locking
            sem_wait(Sem_Producer);
            printf("Your Command to SRAM Simulator =%s.\n",block);
            int Status=Parsing_Command(block,Address,Data);//SRAM status
            //printf("Status code=%d\n",Status);
            switch(Status)
            {
            case 0:
                //for Write
                //Refresh init
                PinRefresh(AddrInput,NumberOfAddrPins);
                PinRefresh(DataInput,NumberOfDataPins);
                WritePin16To2(AddrInput,Address,NUMADDR);
                WritePin16To2(DataInput,Data,NUMDATA);
                SolderingToPin(AddrInput,DataInput,SolderingPin);
                uAddr10=BinaryToDec(AddrInput,NumberOfAddrPins);
                uData10=BinaryToDec(DataInput,NumberOfDataPins);
                WriteToBuffer(uAddr10,uData10,Buffer,BufferSize);
                
                //Release Sem_Lock
                sem_post(Sem_ConSumer);
                continue;
            case 1:
                //for Read
                //Refresh init
                PinRefresh(AddrInput,NumberOfAddrPins);
                PinRefresh(DataInput,NumberOfDataPins);
                WritePin16To2(AddrInput,Address,NUMADDR);
                //Uncommented when Testing
                //SolderingToPin(AddrInput,DataInput,SolderingPin); 
                uAddr10=BinaryToDec(AddrInput,NumberOfAddrPins);
                
                //print Buffer[uAddr10]
                ReadBuffer(uAddr10,Buffer,BufferSize);
                //寫入Shm
                snprintf(block,TESTSTRLEN,"%02X",Buffer[uAddr10]);
                //Release Sem_Lock
                sem_post(Sem_ConSumer);
                continue;
            case 2:
                //for quit Simulator
                done=false;
                printf("Quitting SRAM  Simulator\n");
                //Release Sem_Lock
                sem_post(Sem_ConSumer);
                break;
            }
    }
    //Memory Release
    sem_close(Sem_ConSumer);
    sem_close(Sem_Producer);
    detach_memory_block(block);
    return 0;
}
bool ReadSolderingPin(unsigned short *SolderingPins,int num)
{//Pin number Input & Checking Soldering Pin
//true=re-input pin number for soldering
//false=Skip Checking 
    printf("--format = pin1 number  to pin2 number \n");
    scanf("%hu to %hu",&SolderingPins[0],&SolderingPins[1]);
    printf("print : %hu to %hu\n",SolderingPins[0],SolderingPins[1]);
    if(CheckCTRLPin(SolderingPins,num))
    {
        printf("SRAM were soldered control signal pin,CANT Simulate\n");
        printf("Please re-input two soldering pin number\n");
        return true;
    }
    if(!IsSolderingPinAdjacent(SolderingPins,num))
    {
        printf("You were not soldered adjacent pins,CANT Simulate\n");
        printf("Please re-input two adjacent pin number\n");
        return true;
    }
    return false;
}
bool CheckCTRLPin(unsigned short *SolderingPins,int num)
{
//ture=CTRLPIN in SolderingPin
//false=No CTRLPIN
    int NumberOfCtrlPin=sizeof(CTRLPINS)/sizeof(CTRLPINS[0]);
    for (int i = 0; i <NumberOfCtrlPin; i++)//Checking for soldering CTRLPINs
    {
        for (int j = 0; j < num; j++)
        {
            if(CTRLPINS[i]==SolderingPins[j])
            {
                return true;
            }
        }
    }
    return false;
}
bool IsSolderingPinAdjacent(unsigned short *SolderingPins,int num)
{
//ture=find adjacent pair of soldering pins goto next R/W
//false=pair of soldering pins were not adjacent不相鄰 or jump pin跳接
    int RowOfTSOP=sizeof(TSOPPackage)/sizeof(TSOPPackage[0]);
    int ColOfTSOP=sizeof(TSOPPackage[0])/sizeof(TSOPPackage[0][0]);
    for (int i = 0; i < RowOfTSOP; i++)//checking for Jump soldering
    {
        for (int j = 1; j < ColOfTSOP-1; j++)
        {
            if(TSOPPackage[i][j]==SolderingPins[0])//compare first number 
            {//then compare bith sides
                if(TSOPPackage[i][j-1]==SolderingPins[1])//compare left hand side
                {
                    return true;
                }
                if(TSOPPackage[i][j+1]==SolderingPins[1])//compare right hand side
                {
                    return true;
                }
            }
            if(TSOPPackage[i][j]==SolderingPins[1])//compare second number 
            {//then compare bith sides
                    if(TSOPPackage[i][j-1]==SolderingPins[0])//compare left hand side
                    {
                     return true;
                    }
                    if(TSOPPackage[i][j+1]==SolderingPins[0])//compare right hand side
                    {
                        return true;
                    }
            }
            
        }
    }
    return false;
}
int Parsing_Command(char *SentCommand,char *Addr,char *Data)
{
    //format 
    //1.W address Data
    //2.R address
    //3.quit
    bool ExitCode=(strncmp(SentCommand,"quit",4)==0);
    if(ExitCode)
    {
        return 2;
    }
    if(SentCommand[0]=='W')
    {
        sscanf(SentCommand,"W %s %s",Addr,Data);
        return 0;
    }
    else if(SentCommand[0]=='R')
    {
        sscanf(SentCommand,"R %s",Addr);
        return 1;
    }
}
void PinRefresh(bool *PinArray,int num)
{
    for(int i=0;i<num;i++)
    {
        PinArray[i]=0;
    }
}
void WritePin16To2(bool *SRAMPins,char *Str,int num)//16 to 2
{//Input: SRAM Pin, Hex, and HexLength
//1.16 to 10
//2.10 to 2
    unsigned int SumOfHex;
    SumOfHex=HexToDec(Str,num);
    DecToBinary(SRAMPins,SumOfHex);
}
unsigned int HexToDec(char *Str,int num)
{//16 to 10
    unsigned int result=0; 
    for(int i=0;i<num;i++)
    {//16 to 10

        if (Str[i]<'A')
        {//lower than 'A' = number 
            result+=(Str[i]-'0')*pow(16,num-1-i);
        }
        else// >='A'
        {
            result+=((Str[i]-'A')+10)*pow(16,num-1-i);
        }
    }
    return result;
}
void DecToBinary(bool *SRAMPins,unsigned int Dec)
{
//1. 10 to 2
//2. put 0/1 to Pins
    int iterator=0;
    while(Dec!=0)
    {   SRAMPins[iterator++]=Dec%2;//S = S%2;
        Dec/=2;//S = S/2
    }
}
void PrintInputPin(bool *Input,int num)
{
    for(int i=num-1;i>=0;i--)
    {
        printf("%d \t",Input[i]);
    }
    printf("\n");
}
void SolderingToPin(bool *AddrIn,bool *DataIn,unsigned short *SolderingPins)
{//input : Address,Data, and Soldering pin 
//Function: Checking soldering condition then adjust Pins value
    if(IsVccPin(SolderingPins[0])||IsVccPin(SolderingPins[1]))
    {
        int TargetPin= SolderingPins[0]==VCCPIN ? SolderingPins[1] : SolderingPins[0];
        if(IsAddrPin(TargetPin))
        {
            TargetPin=PinToIndex('A',TargetPin);
            AddrIn[TargetPin]=1;
            return;
        }
        else//TargetPin=DataPin
        {
            TargetPin=PinToIndex('D',TargetPin);
            DataIn[TargetPin]=1;
            return;
        }
    }
    else if(IsGNDPin(SolderingPins[0])||IsGNDPin(SolderingPins[1]))
    {
        int TargetPin= SolderingPins[0]==GNDPIN ? SolderingPins[1] : SolderingPins[0];
        if(IsAddrPin(TargetPin))
        {//TargetPin=AddrPin
            TargetPin=PinToIndex('A',TargetPin);
            AddrIn[TargetPin]=0;
            return;
        }
        else
        {//TargetPin=DataPin
            TargetPin=PinToIndex('D',TargetPin);
            DataIn[TargetPin]=0;
            return;
        }
    }
    else
    {//2AddrPin||2DataPin||1Data1Addr||1Addr1Data
     //need to Specify each pin for index
        int Index1,Index2;
        bool OrValue;
        if(IsAddrPin(SolderingPins[0]))//Pin[0]=1A
        {   //1. find index for 2pin 
            //2. take binary to OR operation then take result
            //3. result that assign to both pins
            Index1=PinToIndex('A',SolderingPins[0]);
            if(IsAddrPin(SolderingPins[1]))
            {//2A
                Index2=PinToIndex('A',SolderingPins[1]);
                OrValue=AddrIn[Index1]||AddrIn[Index2];
                AddrIn[Index1]=OrValue;
                AddrIn[Index2]=OrValue;
                return;
            }
            //1A1D
            Index2=PinToIndex('D',SolderingPins[1]);
            OrValue=AddrIn[Index1]||DataIn[Index2];
            AddrIn[Index1]=OrValue;
            DataIn[Index2]=OrValue;
            return;
        }
        else//Pin[0]=1D
        {
            Index1=PinToIndex('D',SolderingPins[0]);
            if(IsAddrPin(SolderingPins[1]))
            {//1D1A
                Index2=PinToIndex('A',SolderingPins[1]);
                OrValue=DataIn[Index1]||AddrIn[Index2];
                DataIn[Index1]=OrValue;
                AddrIn[Index2]=OrValue;
                return;
            }
            //2D
            Index2=PinToIndex('D',SolderingPins[1]);
            OrValue=DataIn[Index1]||DataIn[Index2];
            DataIn[Index1]=OrValue;
            DataIn[Index2]=OrValue;
            return;
        }
    }  
}
bool IsVccPin(short Pin)
{
    if(Pin==VCCPIN)
    {
        return true;
    }
    return false;
}
bool IsGNDPin(short Pin)
{
    if(Pin==GNDPIN)
    {
        return true;
    }
    return false;
}
void ReadBuffer(unsigned short Addr10,unsigned short *Buffer,unsigned int BufferSize)
{
    if(Addr10<BufferSize)
    {
        printf("addr=%04X,Data in Address = %02X \n",Addr10,Buffer[Addr10]);
        return;
    }
    perror("Error : Addr10>=BufferSize...");
}
void WriteToBuffer(unsigned short Addr10,unsigned short Data10,unsigned short *Buffer,unsigned int BufferSize)
{
    if(Addr10<BufferSize)
    {
        Buffer[Addr10]=Data10;
        printf("Write addr=%04X,Data=%d(Hex=%2X) \n",Addr10,Data10,Buffer[Addr10]);
        return;
    }
    //if address decimal is over Buffersize only read 0
}
unsigned short BinaryToDec(bool *SRAMPins,unsigned int num)
{
    unsigned short Sum=0;
    for(int i=0;i<num;i++)
    {
        Sum+=SRAMPins[i]*pow(2,i);
    }
    return Sum;
}