#include <sys/types.h>//data types
#include <sys/ipc.h>
#include "ShMem.h"


#define IPC_RESULT_ERROR (-1)
//Memory allocation error code

//Index[0~7]=I/O[0~7],DATAPINS[Index[0~7]]=Data physical(SRAM) Pin number
const short DATAPINS[8]={11,12,13,15,16,17,18,19};
//Index[0~14]=A[0~14],ADDRPINS[Index[0~14]]=Addr physical(SRAM) Pin number
const short ADDRPINS[15]={21,23,24,25,26,1,2,3,4,5,6,7,8,9,10};
//CE=20,OE=22,WE=27
const short CTRLPINS[3]={20,22,27};
//TSOPLayout [0] = left hand side, [1]=right hand side
const short TSOPPackage[2][14]={{22,23,24,25,26,27,28,1,2,3,4,5,6,7},{21,20,19,18,17,16,15,14,13,12,11,10,9,8}};


bool IsAddrPin(short Pin)
{
    int len=sizeof(ADDRPINS)/sizeof(ADDRPINS[0]);
    for(int i=0;i<len;i++)
    {
        if(ADDRPINS[i]==Pin)
        {
            return true;
        }
    }
    return false;
}

int PinToIndex(char Type,int Pin)
{//1. find Addr/Data Index from pin number
//2. then return index
    int len;
    if(Type=='A')//Addr
    {
        len=sizeof(ADDRPINS)/sizeof(ADDRPINS[0]);
        for(int i=0;i<len;i++)
        {
            if(ADDRPINS[i]==Pin)
            {
                return i;
            }
        }
    }
    else//Data
    {
        len=sizeof(DATAPINS)/sizeof(DATAPINS[0]);
        for(int i=0;i<len;i++)
        {
            if(DATAPINS[i]==Pin)
            {
                return i;
            }
        }

    }
}

static int get_shared_block(char *filename, int size)
{
    key_t key;
    //Request a key 
    //then this key is linked to a filename, so that other programs can access it
    key =ftok(filename,0);
    if (key==IPC_RESULT_ERROR)
    {
        return IPC_RESULT_ERROR;
    }
    //Get shared block--create it if it doesn't exist
    return shmget(key,size,0644|IPC_CREAT);
}
char * attach_memory_block(char *filename,int size)
{
    int shared_block_id=get_shared_block(filename,size);
    char *result;

    if(shared_block_id==IPC_RESULT_ERROR)
    {
        return NULL;
    }

    //map the shared block into process's memory
    //and give me a pointer to it
    result=shmat(shared_block_id,NULL,0);
    if(result == (char *)IPC_RESULT_ERROR)
    {
        return NULL;
    }
    //return this pointer
    return result;
}
bool detach_memory_block(char *block)
{
    return (shmdt(block)!=IPC_RESULT_ERROR);
}
bool destory_memory_block(char *filename)
{
    int shared_block_id = get_shared_block(filename,0);
    if(shared_block_id==IPC_RESULT_ERROR)
    {
        return NULL;
    }
    return (shmctl(shared_block_id,IPC_RMID,NULL)!=IPC_RESULT_ERROR);
}