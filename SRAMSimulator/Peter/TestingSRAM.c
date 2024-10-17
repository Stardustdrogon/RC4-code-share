/*
Program: This Program offer (R/W) Read/Write/Testing short-circuit conditions for SRAMSimulator
Prerequisite:
     1.You need to execute SDRAMSimulator object file before executing TestingSRAM.c object file.
Input : Using string to manipulate SRAMSimulator
	 1. W addr data
     2. R addr
     3. quit
     4. Testing  
Output : corresponds to Input String.
     1. No response
     2. print Read data from Simulator buffer address location
     3. End of TestingSRAM.c
     4. Test for short-circuit 
History: 
     1.2024/10/7 Divide simulator to tester and simulator
     2.2024/10/7~2024/10/10 Implement IPC between simulator and tester
*/
#include "ShMem.h"
#include <fcntl.h>
int Parsing_Command(char *SentCommand,char *Addr,char *Data);
void SemShmInit(sem_t **Prod,sem_t **Cons,char **Shm);
void WriteShmAccess(sem_t *Prod,sem_t *Cons,char *Shm,char *Str);
void ReadFromShm(sem_t *Cons,char *Shm,char *Str);

bool TSpecialPins(char *Address,char *Data,sem_t *Sem_Producer,sem_t *Sem_ConSumer,char *Shm);
bool CheckCTRLPin(int Pin);
bool TAddrPins(sem_t *Sem_Producer,sem_t *Sem_ConSumer,char *Data,char *Shm);
bool TDataPins(sem_t *Sem_Producer,sem_t *Sem_ConSumer,char *Addr,char *Shm);
int main(int argc,char *argv[])
{
    sem_t *Sem_Producer,*Sem_ConSumer;
    char *block;
    //shm,sem init
    SemShmInit(&Sem_Producer,&Sem_ConSumer,&block);
    char ControlArgument[TESTSTRLEN];
    char Address[NUMADDR+1];//hold hexademical for 4 char + 1 null
    char Data[NUMDATA+1];//hold hexademical for 2 char + 1 null
    //Collect R/W command
    bool done = true;
    while(done)
    {
        printf("Please input \"W addr data\" or \"R addr\" for manipulating SRAM Simulator\n");
        printf("Or \"quit\" to exit Simulator and Testing Program\n");
        printf("Or \"Testing\" to test shorts-circuit condition\nYour Command = ");
        while((fgets(ControlArgument,TESTSTRLEN,stdin)))
        {
            if(strlen(ControlArgument)<MINSTRLEN)
            {//Discard when string is lower than 4 char 
                break;
            }
            printf("Your Command to SRAM Simulator = %s \n",ControlArgument);
            //SRAM status
            int Status=Parsing_Command(ControlArgument,Address,Data);
            switch(Status)
            {
            case 0:
            //for Write
                WriteShmAccess(Sem_Producer,Sem_ConSumer,block,ControlArgument);
                break;
            case 1:
                //for Read
                WriteShmAccess(Sem_Producer,Sem_ConSumer,block,ControlArgument);
                char R_value[NUMDATA+1];
                ReadFromShm(Sem_ConSumer,block,R_value);
                printf("Read From Shm : %s \n",R_value);
                break;
            case 3:
                //Soldering Pin Testing(R/Ws...)
                /*
                在利用下3個function呼叫W、R
                TDataPins()
                1.寫入(W) Addr=0000 for特定DataPins之data
                2.R 0000=R_value，String比對用
                */ 
                strncpy(Address,"0000",NUMADDR);
                Address[NUMADDR+1]='\0';
                strncpy(Data,"FF",NUMDATA);
                Data[NUMDATA+1]='\0';
                if(TSpecialPins(Address,Data,Sem_Producer,Sem_ConSumer,block))
                {
                    //printf("T SpecialCase");
                    break;
                }
                else if(TAddrPins(Sem_Producer,Sem_ConSumer,Data,block))
                {
                    //printf("T AddrBingo");
                    break;
                }
                strncpy(Address,"0000",NUMADDR);
                Address[NUMADDR+1]='\0';
                if(TDataPins(Sem_Producer,Sem_ConSumer,Address,block))
                {
                    //printf("T DataBingo");
                    break;
                }
                //printf("T Not Found");
                break;
            case 2:
                //"quit"  -> Stop Simulator
                WriteShmAccess(Sem_Producer,Sem_ConSumer,block,ControlArgument);
                done=false;
                break;
            case -1:
                printf("String Invalid command format");
                break;
            case -2:
                printf("SRAM out of memory range");
                break;
            case -3:
                printf("SRAM out of data range");
                break;
            default://other than R/W
                printf("UnKnown Command. Please follow format as follow\n\"W addr data\"\n\"R addr\"\n\"quit\"\nNote:addr=4 Hex Num,data=2 Hex Num");
                break;
            }
        }
    }
    printf("End of TestingSRAM.c\n");
    sem_close(Sem_ConSumer);
    sem_close(Sem_Producer);
    detach_memory_block(block);

    return 0;
}
int Parsing_Command(char *SentCommand,char *Addr,char *Data)
{
    //1.W address Data
    //2.R address
    //3.quit
    //4.Testing 
    //5.unknown command re-enter
    bool ExitCode=(strncmp(SentCommand,"quit",MINSTRLEN)==0);
    if(ExitCode)
    {
        return 2;
    }
    if(SentCommand[0]=='W')
    {//Command should be 9 char 
        if(sscanf(SentCommand,"W %s %s",Addr,Data)!=2)
        {//write to string error
            return -1;
        }
        if(strncmp(Addr,MAXADDR,NUMADDR)>0)
        {//Addr>MAXADDR out of memory
            return -2;
        }
        if(strncmp(Data,MAXDATA,NUMDATA)>0)
        {//Data>MAXDATA out of memory
            return -3;
        }
        return 0;
    }
    else if(SentCommand[0]=='R')
    {
        if(sscanf(SentCommand,"R %s",Addr)!=1)
        {//write to string error
            return -1;
        }
        if(strncmp(Addr,MAXADDR,NUMADDR)>0)
        {//Addr>MAXADDR out of memory
            return -2;
        }
        return 1;
    }
    else if(strncmp(SentCommand,"Testing",7)==0)
    {
        return 3;
    }
    return 100;//for default
}
void SemShmInit(sem_t **Prod,sem_t **Cons,char **Shm)
{//Semaphore and Share memory Initialization
    *Prod=sem_open(SEM_PRODUCER_FNAME,O_CREAT);
    //return a sem_t* pointer
    if(*Prod==SEM_FAILED)
    {
        perror("sem_open/Write/producer");
        exit(EXIT_FAILURE);

    }
    *Cons=sem_open(SEM_CONSUMER_FNAME,O_CREAT);
    if(*Cons==SEM_FAILED)
    {
        perror("sem_open/Write/consumer");
        exit(EXIT_FAILURE);

    }
    //grab the shared memory block
    *Shm=attach_memory_block(FILENAME,BLOCK_SIZE);
    if(*Shm==NULL)
    {
        printf("ERROR: couldn't get block\n");
        exit(EXIT_FAILURE);
    }
    
}
void WriteShmAccess(sem_t *Prod,sem_t *Cons,char *Shm,char *Str)
{
    sem_wait(Cons);//Wait for Consumer(Reader) to Print shared memory
    snprintf(Shm,TESTSTRLEN,"%s",Str);
    sem_post(Prod);
    //Notify Reader "You can take data from shm"
}
void ReadFromShm(sem_t *Cons,char *Shm,char *Str)
{
    sem_wait(Cons);
    strncpy(Str,Shm,NUMDATA);
    Str[NUMDATA]='\0';
    sem_post(Cons);
}
bool TSpecialPins(char *Address,char *Data,sem_t *Sem_Producer,sem_t *Sem_ConSumer,char *Shm)
{
    /*
    SpecialCase Short-Circuit   
    1.W 0000 FF
    2.R 0000 for testing GND
    3.R 0020 for testing VCC
    4.R 4000 for testing A14&I/O0
    */
    char R_value[NUMDATA+1];
    char tempStr[TESTSTRLEN];
    snprintf(tempStr,TESTSTRLEN,"W %s %s",Address,Data);
    WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write W 0000 FF
    snprintf(tempStr,TESTSTRLEN,"R %s",Address);
    WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write R 0000
    ReadFromShm(Sem_ConSumer,Shm,R_value);//Read 0000
    if(strncmp(R_value,"00",NUMDATA)!=0)
    {//R_vaule!="00" =>R_value isnt empty
        if(strncmp(R_value,Data,NUMDATA)!=0)
        { //Rvalue!=FF
          printf("GNDPIN were Soldered\n");
          return true;
        }
        //Rvalue==FF
        return false;
    }
    else//R_value=="00"
    {   //Test VCC
        strncpy(Address,"0020",NUMADDR);
        Address[NUMADDR+1]='\0';
        snprintf(tempStr,TESTSTRLEN,"R %s",Address);
        WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//Command R 0020
        ReadFromShm(Sem_ConSumer,Shm,R_value);//R
        if(strncmp(R_value,Data,NUMDATA)==0)
        {
            printf("VCCPIN were Soldered\n");
            return true;
        }
        else//Test A14&I/O0
        {
            strncpy(Address,"4000",NUMADDR);
            Address[NUMADDR+1]='\0';
            snprintf(tempStr,TESTSTRLEN,"R %s",Address);
            WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//Command R 4000
            ReadFromShm(Sem_ConSumer,Shm,R_value);//Read 4000
            if(strncmp(R_value,Data,NUMDATA)==0)
            {
                printf("A14&I/O0 were Soldered\n");
                return true;
            }
        }
    }
    return false;

}
bool TAddrPins(sem_t *Sem_Producer,sem_t *Sem_ConSumer,char *Data,char *Shm)
{
    /*
    1.寫入(W) for特定AddrPins "FF"(fixed)data
    2.R此AddrPins=R_value，String比對用
      */
    char R_value[NUMDATA+1];
    //tempStr just keep W/R command for temporary
    //because Shm R/W need semaphore to notify SRAM
    char tempStr[TESTSTRLEN];
    unsigned int Dec;
    int AddrLen=sizeof(ADDRPINS)/sizeof(ADDRPINS[0]);
    for(int i=0;i<AddrLen;i++)
    {   //format AddrPin to Hex
        Dec=pow(2,i);
        snprintf(tempStr,TESTSTRLEN,"W %04X %s",Dec,Data);
        WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write W Addr FF
        snprintf(tempStr,TESTSTRLEN,"R %04X",Dec);
        WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write R Addr
        ReadFromShm(Sem_ConSumer,Shm,R_value);//Read Addr
        if(strncmp(R_value,Data,NUMDATA)==0)
        {//No solderingPin
            //Restore to default
            snprintf(tempStr,TESTSTRLEN,"W %04X 00",Dec);
            WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write W Addr FF
            continue;
        }
        else//Find SolderingPin
        {//index to Pin
            int AdjancetPinIndex[2]={0};
            //Pin to index,index find adjacent 2 pin(left,right)
            int RowOfTSOP=sizeof(TSOPPackage)/sizeof(TSOPPackage[0]);
            int ColOfTSOP=sizeof(TSOPPackage[0])/sizeof(TSOPPackage[0][0]);
            for (int z = 0; z < RowOfTSOP; z++)//checking for Jump soldering
            {
                for (int j = 0; j < ColOfTSOP; j++)
                {
                    if(TSOPPackage[z][j]==ADDRPINS[i])//find Address PIN 
                    {//then compare bith sides
                        if(((j-1)>=0))//left hand side
                        {
                            AdjancetPinIndex[0]=TSOPPackage[z][j-1];
                        }
                        if(((j+1)<=ColOfTSOP))//right hand side
                        {
                            AdjancetPinIndex[1]=TSOPPackage[z][j+1];
                        }
                    }
                }
            }
            //for in AdjacentPins(Left,right)
            //then Pin to find Index
            //then Index transfer(to dec) then to hex(R 1"/2")
            //2 = #SolderingPin
            for(int z=0;z<2;z++)
            {
                if(CheckCTRLPin(AdjancetPinIndex[z])||AdjancetPinIndex[z]==VCCPIN||AdjancetPinIndex[z]==GNDPIN||!IsAddrPin((AdjancetPinIndex[z])))
                {//Exclude CTRL、VCC、GND、DataPin
                    continue;
                }
                int TempIndex=PinToIndex('A',AdjancetPinIndex[z]);
                int TempDec=Dec+pow(2,TempIndex);//Index to dec
                snprintf(tempStr,TESTSTRLEN,"R %04X",TempDec);
                WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write R Addr to Shm
                ReadFromShm(Sem_ConSumer,Shm,R_value);//Read Addr from Shm
                if(strncmp(R_value,Data,NUMDATA)==0)
                {
                    printf("Two address pins Short Circuit => Pin= A[%d] to A[%d]\n",i,TempIndex);
                    return true;
                }
            }
        }
    }
    return false;
}
bool TDataPins(sem_t *Sem_Producer,sem_t *Sem_ConSumer,char *Addr,char *Shm)
{
    /*
    1.寫入(W) for特定AddrPins "FF"(fixed)data
    2.R此AddrPins=R_value，String比對用
      */
    char R_value[NUMDATA+1];
    char DataStr[NUMDATA+1];
    //tempStr keep W/R command to notify SRAM
    char tempStr[TESTSTRLEN];
    unsigned int Data=0;//DataPin(Decimal)
    int DataLen=sizeof(DATAPINS)/sizeof(DATAPINS[0]);
    for(int i=0;i<DataLen;i++)
    {   Data=pow(2,i);
        snprintf(DataStr,NUMDATA+1,"%02X",Data);
        snprintf(tempStr,TESTSTRLEN,"W %s %s",Addr,DataStr);
        WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write W 0000 "for each only one DataPin=1"
        snprintf(tempStr,TESTSTRLEN,"R %s",Addr);
        WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write R Addr
        ReadFromShm(Sem_ConSumer,Shm,R_value);//Read Addr
        if(strncmp(R_value,DataStr,NUMDATA)==0)
        {//This Di Pin not solderingPin
        //Write Data to default
            snprintf(tempStr,TESTSTRLEN,"W %s 00",Addr);
            WriteShmAccess(Sem_Producer,Sem_ConSumer,Shm,tempStr);//write W Addr FF
            continue;
        }
        else//Find SolderingPin
        {//index to Pin
            int AdjancetPinIndex[2]={0};
            //Pin to index,index find adjacent 2 pin(left,right)
            int ColOfTSOP=sizeof(TSOPPackage[0])/sizeof(TSOPPackage[0][0]);
            for (int j = 0; j < ColOfTSOP; j++)
            {//find Adjacent Pin Index
                if(TSOPPackage[1][j]==DATAPINS[i])//find DataPin
                {
                    if(((j-1)>=0))//left hand side
                    {
                        AdjancetPinIndex[0]=TSOPPackage[1][j-1];
                    }
                    if(((j+1)<=ColOfTSOP))//right hand side
                    {
                        AdjancetPinIndex[1]=TSOPPackage[1][j+1];
                    }
                }
            }
            //for in AdjacentPins(Left,right)
            //then Pin to find Index
            //then Index transfer(to dec) then to hex(Data 1"/2")
            //2 = #SolderingPin
            int TempIndex,TempDec;
            for(int z=0;z<2;z++)
            {
                if(CheckCTRLPin(AdjancetPinIndex[z])||AdjancetPinIndex[z]==VCCPIN||AdjancetPinIndex[z]==GNDPIN||IsAddrPin((AdjancetPinIndex[z])))
                {//Exclude CTRL、VCC、GND、AddrPin
                    continue;
                }
                TempIndex=PinToIndex('D',AdjancetPinIndex[z]);
                TempDec=Data+pow(2,TempIndex);//Data Index to dec
                char DataStrForCompare[NUMDATA+1];//dec to String
                snprintf(DataStrForCompare,NUMDATA+1,"%02X",TempDec);
                if(strncmp(R_value,DataStrForCompare,NUMDATA)==0)
                {
                    printf("Two Data pins Short Circuit => Pin= I/O[%d] to I/O[%d]\n",i,TempIndex);
                    return true;
                }
            }
        }
    }
    return false;
}

bool CheckCTRLPin(int Pin)
{
    int len=sizeof(CTRLPINS)/sizeof(CTRLPINS[0]);
    for(int i=0;i<len;i++)
    {
        if(Pin==CTRLPINS[i])
        {
            return true;
        }
    }
    return false;
}
