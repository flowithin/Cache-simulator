/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/
#define READ 0
#define WRITE 1
// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
} stateType;

extern void cache_init(int blockSize, int numSets, int blocksPerSet);
extern int cache_access(int addr, int write_flag, int write_data);
extern void printStats();
static stateType state;
static int num_mem_accesses = 0;
int mem_access(int addr, int write_flag, int write_data) {
    ++num_mem_accesses;
    if (write_flag) {
        state.mem[addr] = write_data;
        if(state.numMemory <= addr) {
            state.numMemory = addr + 1;
        }
    }
    /*printf("addr = %x, state[mem] = %x\n",addr, state.mem[addr]);*/
    return state.mem[addr];
}
int get_num_mem_accesses(){
	return num_mem_accesses;
}
void printState(stateType *);

static inline int convertNum(int32_t);

int 
main(int argc, char **argv)
{
  cache_init(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    char line[MAXLINELENGTH];
    FILE *filePtr;

    if (argc != 5) {
        printf("error: usage: ./simulator program.mc blockSizeInWords numberOfSets blocksPerSet\n");
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(2);
    }
    /* read the entire machine-code file into memory */
    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		    if (state.numMemory >= MEMORYSIZE) {
			      fprintf(stderr, "exceeded memory size\n");
			      exit(2);
		    }                                       
		    if (sscanf(line, "%x", state.mem+state.numMemory) != 1) {
			      fprintf(stderr, "error in reading address %d\n", state.numMemory);
			      exit(2);
		    }
		    printf("memory[%d]=0x%x\n", state.numMemory, state.mem[state.numMemory]);
    }
        for(int i = 0; i < NUMREGS; i++) state.reg[i]=0;
    int hex = 0,arg0,arg1,arg2,instrNum = 0;
    while(state.pc < state.numMemory)
    {
        /*printState(&state);*/
        hex = 
        cache_access(state.pc, READ, 0);
        if(((hex >> 22) & 0x7) == 0b000)//add
        {
            arg0 = (hex >> 19 ) & 0x7;
            arg1 = (hex >> 16 ) & 0x7;
            arg2 = hex & 0x7;
            state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
            state.pc++;
        }
        else if(((hex >> 22) & 0x7) == 0b001)//nor
        {
            arg0 = (hex >> 19 ) & 0x7;
            arg1 = (hex >> 16 ) & 0x7;
            arg2 = hex & 0x7;
            state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
            state.pc++;
        }
        else if(((hex >> 22) & 0x7) == 0b010)//lw
        {
            arg0 = (hex >> 19 ) & 0x7;
            arg1 = (hex >> 16 ) & 0x7;
            arg2 = hex & 0xFFFF;
            /*state.reg[arg1] = state.mem[state.reg[arg0] + convertNum(arg2)];*/
            printf("lw called cache_access\n");
            state.reg[arg1] = cache_access(state.reg[arg0] + convertNum(arg2), READ, 0);
            state.pc++;
        }
        else if(((hex >> 22) & 0x7) == 0b011)//sw
        {
            arg0 = (hex >> 19 ) & 0x7;
            arg1 = (hex >> 16 ) & 0x7;
            arg2 = hex & 0xFFFF;
            /*state.mem[state.reg[arg0] + convertNum(arg2)] = state.reg[arg1];*/
            printf("sw called cache_access\n");
            cache_access(state.reg[arg0] + convertNum(arg2), WRITE, state.reg[arg1]);  
            state.pc++;
        }
        
        else if(((hex >> 22) & 0x7) == 0b100)//beq
        {
            arg0 = (hex >> 19 ) & 0x7;
            arg1 = (hex >> 16 ) & 0x7;
            arg2 = hex & 0xFFFF;
            if(state.reg[arg0] == state.reg[arg1])
            state.pc += 1+convertNum(arg2);
            else state.pc++;
        }   
        else if(((hex >> 22) & 0x7) == 0b101)//jalr
        {
            arg0 = (hex >> 19 ) & 0x7;
            arg1 = (hex >> 16 ) & 0x7;
            state.reg[arg1]=state.pc+1;
            state.pc = state.reg[arg0];
        }     
        else if(((hex >> 22) & 0x7) == 0b110)//halt
        {
            state.pc++;
            instrNum++;
            break;
        }
        else if(((hex >> 22) & 0x7) == 0b111)//noop
        {
            state.pc++;
        }
        instrNum ++;     
    }
    printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:\n",instrNum);
    printState(&state);
    printStats();
    //Your code ends here! 

    return(0);
    }//main
    





















/*
* DO NOT MODIFY ANY OF THE CODE BELOW. 
*/

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
	  for (i=0; i<NUMREGS; i++) {
	      printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	  }
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) 
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
* Write any helper functions that you wish down here. 
*/
