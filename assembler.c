/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

//#include <iterator>
//#include <cstddef>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
static inline bool strequ(char *str1, char*str2);
struct labels{
    char *label;
    int labelnum;
};
struct symbel{
    char type;
    char* symbel;
    int offset;
};
struct reloc{
    int offset;
    char* name;
    char* instruction;
};
bool hasDuplicate(struct reloc relocs[],int size,char* name)
{
    for (int i=0; i < size;i++)
    {
        if(!strcmp(relocs[i].name, name)) return true;
    }
    return false;
}
int main(int argc, char **argv)
{   
    struct labels l[MAXLINELENGTH] = {};
    struct reloc relocLabel[MAXLINELENGTH] ={};
    struct symbel symbelLabel[MAXLINELENGTH]={};
    int hex[MAXLINELENGTH];
    int hexc=0;
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    bool found=false;
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }
    inFileString = argv[1];
    outFileString = argv[2];
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
   // int dataBegin;
    int linenum = 0,n=0,numInstr=0, numData = 0,numSymb=0,numReloc=0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        //I want to get the first line of output here;
        if (strcmp(opcode, ".fill")) numInstr++;
        else numData++;
        if(label[0] != '\0')
        {
            char *tempt = malloc (strlen(label)*sizeof(char));
            //printf("strlen(label) = %lld",strlen(label));
            for(int i = 0; i < strlen(label)+1;i++) tempt[i] = label[i]; 
            l[n].label = tempt;
            if(tempt[0] >= 'A' && tempt[0] <= 'Z'){
            if(!strcmp(opcode, ".fill")) 
                {
                    symbelLabel[numSymb].type = 'D';
                    symbelLabel[numSymb].offset = linenum-numInstr;
                     symbelLabel[numSymb].symbel = malloc(strlen(label) + 1); 
                    strcpy(symbelLabel[numSymb++].symbel ,label);
                } else {
                    symbelLabel[numSymb].type = 'T';
                    symbelLabel[numSymb].offset = linenum;
                     symbelLabel[numSymb].symbel = malloc(strlen(label) + 1); 
                    strcpy(symbelLabel[numSymb++].symbel , label);
                }
            }
            l[n++].labelnum = linenum;
           // printf("n = %d labelnum = %d label = %s\n",n,l[n-1].labelnum, l[n-1].label);
        
        }
        linenum++;
    }
    for(int j=0;j<n-1;j++) {
        for(int i = j+1; i < n;i++)
    {
        if(strequ(l[j].label, l[i].label))
        {
        printf("repeat label def");
        exit(1);
        }
        }
    }
    printf("first going through done\n");
    rewind(inFilePtr);
    linenum = 0;
    int hexoutput = 0;
    //first output the header:
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
       found = false;
        //printf("%s\n",label);
        hexoutput = 0;
        //Rtype
    if (!strcmp(opcode, "add")) {
        /* do whatever you need to do for opcode "add" */
        if(atoi(arg0)>7 ||atoi(arg0)<0 ||atoi(arg1)>7 ||atoi(arg1)<0 || atoi(arg2)>7||atoi(arg2)<0) 
        {
            printf("add register out bound");
            exit(1);
        }
        if(!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)) {
            printf("add register not number");
            exit(1);
        }

        hexoutput = (atoi(arg0) << 19 )+ (atoi(arg1) << 16) + atoi(arg2);
        printf("add\n");

    }
    else if(!strcmp(opcode, "nor" ))
    {
       
        if(!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)) 
        {
            printf("nor register not number");
            exit(1);
        }
        if(atoi(arg0)>7 ||atoi(arg0)<0 ||atoi(arg1)>7 ||atoi(arg1)<0|| atoi(arg2)>7||atoi(arg2)<0) 
        {
            printf("nor register out bound");
            exit(1);
        }
        hexoutput = (0b001 << 22 )+ (atoi(arg0) << 19 )+ (atoi(arg1) << 16) + atoi(arg2);
        //printf("nor\n");
    }
    //IType
    else if(!strcmp(opcode, "lw" ))
    {        //printf("lw\n");
        if(atoi(arg0)>7 ||atoi(arg0)<0 ||atoi(arg1)>7 ||atoi(arg1)<0) 
        {
            printf("lw register out bound");
            exit(1);
        }
        if(!isNumber(arg0) || !isNumber(arg1) ) 
        {
            printf("lw register not number");
            exit(1);
        }
        if(!isNumber(arg2)){
            //printf("not a number\n");
           //if(arg2 == NULL) printf("NULL\n"); 
            printf("arg2: %s\n",arg2);
            int i =0;
            for( i = 0; i < n; i++){
            //printf("%s",l[i].label);
            if(strequ(l[i].label,arg2)) 
            {
            hexoutput = (0b010 << 22)+ (atoi(arg0)<< 19) +(atoi(arg1) << 16) + l[i].labelnum;
            found = true;
                        //for sw and lw, arg2 label would be a relocation table
                    relocLabel[numReloc].instruction = malloc(strlen(opcode) + 1); 
                    strcpy(relocLabel[numReloc].instruction,opcode);
                    relocLabel[numReloc].offset = linenum;//it is the relatice position of where is appear
                    relocLabel[numReloc].name = malloc(strlen(arg2) + 1); 
                    strcpy(relocLabel[numReloc++].name, arg2);
            break;
            }
        }
          if (arg2[0] >= 'A' && arg2[0] <= 'Z')
            {
                    //global variable
           if(!found){
             hexoutput = (0b010 << 22)+ (atoi(arg0)<< 19) +(atoi(arg1) << 16);//globel = 0
                   if(!hasDuplicate(relocLabel, numReloc, arg2)) 
                        {
                    symbelLabel[numSymb].symbel = malloc(strlen(arg2) + 1); 
                    strcpy(symbelLabel[numSymb].symbel,arg2);
                    symbelLabel[numSymb].type = 'U';
                    symbelLabel[numSymb++].offset = 0;
                        }
                    relocLabel[numReloc].instruction = malloc(strlen(opcode) + 1); 
                    strcpy(relocLabel[numReloc].instruction,opcode);
                    relocLabel[numReloc].offset = linenum;//it is the relatice position of where is appear
                    relocLabel[numReloc].name = malloc(strlen(arg2) + 1); 
                    strcpy(relocLabel[numReloc++].name, arg2);
            
                } 
                } else {if(!found){printf("not found label: %s",arg2);exit(1);} }
        }
        else
        { 
        //printf("offset = %d,%s",atoi(arg2),arg2);
        if(atoi(arg2)< -(1<<15) || atoi(arg2) >= (1<<15)) //check range for lw
        {
            printf("lw offset out bound");
            exit(1);
        }
        if(atoi(arg2 )< 0)
        {
            hexoutput = (0b010 <<22)+ (atoi(arg0)<< 19) + (atoi(arg1) << 16) + (1<<15)*2 + atoi(arg2);
        }
        else hexoutput = (0b010 <<22)+ (atoi(arg0)<< 19) + (atoi(arg1) << 16)+ atoi(arg2);  
        }
    }
    else if(!strcmp(opcode, "sw" ))
    {
        if(atoi(arg0)>7 ||atoi(arg0)<0 ||atoi(arg1)>7 ||atoi(arg1)<0) 
        {
            printf("sw register out bound");
            exit(1);
        }
        if(!isNumber(arg0) || !isNumber(arg1) ) 
        {
            printf("sw register not number");
            exit(1);
        }
        if(!isNumber(arg2)){
            //printf("not a number\n");
            int i =0;
                for( i = 0; i < n; i++){
            //printf("%s",l[i].label);
            if(strequ(l[i].label,arg2)) 
            {
            hexoutput = (0b011 << 22)+ (atoi(arg0)<< 19) +(atoi(arg1) << 16) + l[i].labelnum;
            found = true;
                        //for sw and lw, arg2 label would be a relocation table
           relocLabel[numReloc].instruction = malloc(strlen(opcode) + 1); 
           strcpy(relocLabel[numReloc].instruction,opcode);
           relocLabel[numReloc].offset = linenum;//it is the relatice position of where is appear
           relocLabel[numReloc].name = malloc(strlen(arg2) + 1); 
           strcpy(relocLabel[numReloc++].name, arg2);
            break;
            }
        }
          if (arg2[0] >= 'A' && arg2[0] <= 'Z')
            {
                    //global variable
           if(!found){
            printf("not found: %s",arg2);
             hexoutput = (0b011 << 22)+ (atoi(arg0)<< 19) +(atoi(arg1) << 16);//globel = 0
             if(!hasDuplicate(relocLabel, numReloc, arg2)) 
                        {
                    symbelLabel[numSymb].symbel = malloc(strlen(arg2) + 1); 
                    strcpy(symbelLabel[numSymb].symbel,arg2);
                    symbelLabel[numSymb].type = 'U';
                    symbelLabel[numSymb++].offset = 0;
                        }
             relocLabel[numReloc].instruction = malloc(strlen(opcode) + 1); 
             strcpy(relocLabel[numReloc].instruction,opcode);
             relocLabel[numReloc].offset = linenum;//it is the relatice position of where is appear
             relocLabel[numReloc].name = malloc(strlen(arg2) + 1); 
             strcpy(relocLabel[numReloc++].name, arg2);
                } 
                } else {if(!found) exit(1);}
        }
        else
        {
        if(atoi(arg2)< -(1<<15) || atoi(arg2) >= (1<<15)) //check range for sw
        {
            printf("sw offset out bound");
            exit(1);
        }
            if(atoi(arg2)<0)
            hexoutput =( 0b011 <<22) + (atoi(arg0)<< 19) + (atoi(arg1) << 16)+ (1<<15)*2 + atoi(arg2); 
            else  hexoutput = (0b011 <<22)+ (atoi(arg0)<< 19) + (atoi(arg1) << 16) + atoi(arg2);
        }
    }
    else if(!strcmp(opcode, "beq" ))
    {
        if(atoi(arg0)>7 ||atoi(arg0)<0 ||atoi(arg1)>7 ||atoi(arg1)<0 ) 
        {
            printf("beq register out bound");
            exit(1);
        }
        if(!isNumber(arg0) || !isNumber(arg1) ) 
        {
            printf("beq register not number");
            exit(1);
        }
        if(!isNumber(arg2)){
            for(int i = 0; i < n; i++){
            if(strequ(l[i].label,arg2)) 
            {
                printf("%s\n",arg2); 
                printf("%d",linenum);
                if(l[i].labelnum - linenum -1 <0)
                hexoutput = (0b100<< 22) + (atoi(arg0)<< 19) + (atoi(arg1) << 16) + (1<<15)*2+l[i].labelnum-linenum-1;
                else 
                hexoutput = (0b100 << 22)+ (atoi(arg0)<< 19) +( atoi(arg1) << 16)+ l[i].labelnum-linenum-1;
                found=true;
                break;
            }
        }
        if(!found) exit(1);
        }
        else
        {
        if(atoi(arg2)< -(1<<15) || atoi(arg2) >= (1<<15)) //check off set range for beq
        {
            printf("offset out bound");
            exit(1);
        }
        if(atoi(arg2)<0)
        hexoutput =( 0b100 <<22) + (atoi(arg0)<< 19) + (atoi(arg1) << 16)+ (1<<15)*2 + atoi(arg2); 
        else  hexoutput = (0b100 <<22)+ (atoi(arg0)<< 19) + (atoi(arg1) << 16) + atoi(arg2);
    }
    }
    else if(!strcmp(opcode, "jalr" ))
    {   
    if(!isNumber(arg0) || !isNumber(arg1) ) 
    {
        printf("jalr register not number");
        exit(1);
    }
    if(atoi(arg0)>7 ||atoi(arg0)<0 ||atoi(arg1)>7 ||atoi(arg1)<0) 
    {
        printf("jalr register out bound");
        exit(1);
    }
    hexoutput = (0b101 << 22) + (atoi(arg0)<< 19) +( atoi(arg1) << 16);
    }
    else if(!strcmp(opcode, "noop" ))
    {
        hexoutput = 0b111 << 22 ;
    }
    else if(!strcmp(opcode, "halt" ))
    {
        hexoutput = 0b110 << 22;
    }
    else if(!strcmp(opcode, ".fill" ))
    {
       if(!isNumber(arg0))
       {int i=0; 
        for(i = 0; i < n; i++){
            if(strequ(l[i].label,arg0)) 
            {
               hexoutput = l[i].labelnum;
               found=true;     
                relocLabel[numReloc].instruction = malloc(strlen(opcode) + 1); 
                strcpy(relocLabel[numReloc].instruction,opcode);
                relocLabel[numReloc].offset = linenum-numInstr      ;//it is the relatice position of where is appear
                relocLabel[numReloc].name = malloc(strlen(arg0) + 1); 
                strcpy(relocLabel[numReloc++].name, arg0);
               break;
            }
        }
            if (arg0[0] >= 'A' && arg0[0] <= 'Z')
            {
                    //global variable
                    if(!found){
                    hexoutput = 0;    
                    if(!hasDuplicate(relocLabel, numReloc, arg0)) 
                        {
                    symbelLabel[numSymb].symbel = malloc(strlen(arg0) + 1); 
                    strcpy(symbelLabel[numSymb].symbel,arg0);
                    symbelLabel[numSymb].type = 'U';
                    symbelLabel[numSymb++].offset = 0;
                        }
                    relocLabel[numReloc].instruction = malloc(strlen(opcode) + 1); 
                    strcpy(relocLabel[numReloc].instruction,opcode);
                    relocLabel[numReloc].offset = linenum-numInstr;//it is the relatice position of where is appear
                    relocLabel[numReloc].name = malloc(strlen(arg0) + 1); 
                    strcpy(relocLabel[numReloc++].name, arg0);
                 }  
            } else {if(!found) exit(1);}
        }
        else hexoutput = atoi(arg0);
    }
    else 
    {printf("opcode not found\n");
            exit(1);
        }
    //printHexToFile(outFilePtr, hexoutput);
   hex[hexc++]=hexoutput; 
        linenum++;
        printf("opcode now: %s\n",opcode);
    }//while
    printf("while loop finished\n");
    fprintf(outFilePtr,"%d %d %d %d\n",numInstr,numData,numSymb,numReloc);
    for(int i=0;i<hexc;i++){
        printHexToFile(outFilePtr, hex[i]);
    }
    for(int i = 0; i < numSymb;i++)
    {
   fprintf(outFilePtr, "%s %c %d\n", symbelLabel[i].symbel,symbelLabel[i].type,symbelLabel[i].offset);
    }
    for(int i = 0; i < numReloc;i++)
    {
    fprintf(outFilePtr, "%d %s %s\n", relocLabel[i].offset,relocLabel[i].instruction,relocLabel[i].name);
    }
    
    for(int i = 0; i < n;i++)
    {
        free (l[i].label);
    }
    
    printf("program done\n");
    return(0);
}
































// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}
static inline bool
strequ(char *str1, char*str2)
{
    if(strlen(str1) < strlen(str2)){
     
    for(int i = 0; i< strlen(str1)+1 ;i++)
    {
        if(str1[i] != str2[i])  return false;
    }
    }
    else
    {
    for(int i = 0; i< strlen(str2)+1 ;i++)
    {
        if(str1[i] != str2[i])  return false;
    }
    }
    return true;
}
static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}


// Prints a machine code word in the proper hex format to the file
static inline void 
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}
