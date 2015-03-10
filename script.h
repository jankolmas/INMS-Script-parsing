#include <stdio.h>
#include <stdlib.h>

/* Definition of structures relevant to the script.
 * 
 * Each script has up to 5 sequences
 * Each script has a number of timetable entries
 * Each sequence has a number of commands
 * Some commands can have an array of parameters
 */


struct Command{
      unsigned char deltaTimeSec;
      unsigned char deltaTimeMin;
      unsigned char CMD_ID;
      unsigned char LEN;
      unsigned char SEQ_CNT;
      unsigned char *parameters;
};

struct Entry{
      unsigned char TIME_sec;
      unsigned char TIME_min;
      unsigned char TIME_hrs;
      unsigned char Script_INDEX;
};

struct Sequence{
      unsigned char num_commands;
      struct Command *commands;     
};

struct Script{
      unsigned int Script_LENGTH;
      unsigned int T_STARTTIME;
      unsigned int FILE_SN;
      unsigned char SW_ver;
      unsigned char SU_ID;
      unsigned int SCRIPT_TYPE;
      unsigned char SU_MD;
      
      unsigned char tt_length;
      struct Entry *tt_entries;
      
      unsigned char num_sequences;
      struct Sequence sequences[5]; 
      
      unsigned int checksum;
};

/* Function prototypes. 
 * Their purpose is explained withe the definition
 */
void TaskScriptHandler();
void TaskScript();
unsigned int scriptUTC(unsigned char*);
int readBinFile(unsigned char**, char*);
unsigned int scriptLength(unsigned char*);
char checkScript(unsigned char*);
struct Script parseScript(unsigned char*);

