CC=gcc
CFLAGS=-g -Wall
OBJS=ShMem.o
EXE=SDRAMSimulator.elf TestingSRAM.elf destory_shmem.elf

all: $(EXE)
%.elf: %.o $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@ -lm
#for object file ShMem.o
%.o:  %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
#for all elf file
%.o:  %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(OBJS) $(EXE)