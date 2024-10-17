#ifndef SHARRD_MEMORY_H
#define SHARRD_MEMORY_H

#include <stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <semaphore.h>
#include <sys/shm.h>
#include<math.h>

//Hardware boundary
#define MAXADDR "7FFF"
#define MAXDATA "FF"
#define NUMADDR (4)
#define NUMDATA (2)
//Input String Size Length 4~10
#define TESTSTRLEN (10)
#define MINSTRLEN (4)
//PINs
#define VCCPIN (28)
#define GNDPIN (14)
//3rd special case for Testing
#define Addr14Pin (10)
#define Data0Pin (11)

//Share memory
#define BLOCK_SIZE (4096)
#define FILENAME "TestingSRAM.c"
//Create a shared memory block associated with filename
char * attach_memory_block(char *filename,int size);
bool detach_memory_block(char *block);
bool destory_memory_block(char *filename);

//Semaphore
#define SEM_PRODUCER_FNAME "/myproducer"
#define SEM_CONSUMER_FNAME "/myconsumer"

//Index[0~7]=I/O[0~7],DATAPINS[Index[0~7]]=Data physical(SRAM) Pin number
extern const short DATAPINS[8];
//Index[0~14]=A[0~14],ADDRPINS[Index[0~14]]=Addr physical(SRAM) Pin number
extern const short ADDRPINS[15];
//CE=20,OE=22,WE=27
extern const short CTRLPINS[3];
//TSOPLayout [0] = left hand side, [1]=right hand side
extern const short TSOPPackage[2][14];

extern bool IsAddrPin(short Pin);
extern int PinToIndex(char Type,int Pin);

#endif