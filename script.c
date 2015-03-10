#include <stdio.h>
#include <stdlib.h>
#include "script.h"

/* This is an overview of the tasks and functions that will need
 * to run on the OBC to deal with INMS scripts. This is extendable
 * to some measure to other QB50 science units - check the relevant ICD.
 * 
 * This file contains functions for extracting information from the 
 * script, as well as Salvo RTOS tasks (in pseudocode), that will
 * use these functions.
 * 
 * // This is a comment explaining something
 * //this is pseudocode, yet to be implemented (no space after //)
 * 
 * Author: Jan Kolmas
 * Contact: kolmas@stanford.edu
 * Created: 	2015-03-05
 * Last update: 2015-03-05
 * Version: 0.1
 * 
 * This code has been tested on example scripts, but not 
 * on a microcontroller yet. If you find bugs, please report them on
 * the QB50 forum: https://www.qb50.eu/forum/ under INMS.
 */




/* TaskScriptHandler()
 * 
 * Salvo RTOS task, charged with executing the current script.
 * The programming of this task follows the flowchart outlined in
 * the INMS ICD.
 * 
 * No inputs, no outputs. 
 */
void TaskScriptHandler(){
      // initialize:
	    int index_script = -1;
	    int index_tt = 0;
	    int index_sequence = 0;
	    int index_command = 0;
	    struct Script script;
	    char *binary;
      
      // loop:
      while(1){     
	    if(index_script==-1){
		  //no script to run yet, wait for flag from TaskScriptTimer
	    }

	    // New script loaded. Read, parse and reset indices.
	    binary = NULL; //add function to read from memory
	    script = parseScript(binary);
	    index_tt = 0;
	    index_sequence = 0;
	    index_command = 0;
		  
	    // Loop of timetable entries
	    while(index_tt<script.tt_length){
		  //wait for timetable entry
		  index_sequence = script.tt_entries[index_tt].Script_INDEX & 0b00001111;;
		  
		  // Loop of commands
		  while(index_command<script.sequences[index_sequence].num_commands){
			//command for obc or su?
			//execute command
			//wait for time dt
			index_command++;
		  }
		  index_command = 0;
		  
		  index_tt++; // move to the next timetable entry
		  
		  // Check if a newer script is ready
		  
		  //check flag for new script, if true: 
			//update index_script to new value
			//clean up dynamically allocated memory:
			      /*
			      free(buffer);
			      free(script.tt_entries);
			      for(j=0;j<script.num_sequences;j++){ // for each sequence
				    for(k=0;k<script.sequences[j].num_commands;k++){ // for each command
					  if(script.sequences[j].commands[k].LEN>1){
						free(script.sequences[j].commands[k].parameters);				    
					  }
				    }     
			      }
			      for(j=0;j<script.num_sequences;j++) free(script.sequences[j].commands); 
			      */
			//break; (from the timetable loop)
	    }
	    index_tt = 0; // Reschedules sequences for next day      
      }  
return; // We never get here
}

/* TaskScriptTimer()
 * 
 * Salvo RTOS task monitoring idle scripts. If a UTC time is 
 * reached for one of the idle scripts, this task will signal
 * the script handler to load this new script.
 * 
 * No inputs, no outputs.
 */
void TaskScriptTimer(){
      
      // initialize: 
	    //read number of scripts in memory
	    //read start time for each using scriptUTC() and store
	    
      // loop:
      while(1){
	    //check flag for change in script memory banks set by receiving task    
		  //if true, re-read number of scripts
		  //read start time for each and store 
		      
	    //compare all script times agains current times
		  //if script is ready, erect flag for handler with new script lot number
      }
      return; // We never get here
}

/* scriptUTC()
 * 
 * This function extracts the starting time from a binary script.
 * Time is defined in seconds from the 2000-1-1 00:00:00 UTC mark.
 * This function gets called by the script monitoring idle scripts.
 * 
 * Inputs:	buffer: binary script
 * Outputs:	UTC time
 */
unsigned int scriptUTC(unsigned char* buffer){
      return buffer[2] | ((int)buffer[3] << 8) | ((int)buffer[4] << 16) | ((int)buffer[5] << 24);
}

/* readBinFile()
 * 
 * This function reads a binary file into an array of bytes, and
 * returnes the size of the array. This may not be necessary 
 * in the flight software, but it is useful for ground testing.
 * 
 * Inputs: 	buffer: a byte array to store the script into
 * 		filename: path and file name of the script to be loaded
 * Outputs: 	Length of script
 */
int readBinFile(unsigned char** buffer, char* filename){
      
      // Declare variables
      unsigned char lenbuf[2];
      unsigned int filesize;
      FILE *f;
      
      // Open file for binary read
      f = fopen(filename, "rb");
      if(!f) return 0;
      
      // Get length information from the script itself
      fread(lenbuf, 2, 1, f);
      filesize = lenbuf[0] | ((int)lenbuf[1] << 8);
      
      // Allocate memory and read file
      *buffer = malloc((filesize+1)*sizeof(**buffer));
      rewind(f);
      fread(*buffer, filesize, 1, f);
      
      fclose(f);
      return filesize;
      
}

/* scriptLength()
 * 
 * This function returns the script length information from the
 * binary array script format. It is determined from the script 
 * header.
 * 
 * Inputs:	buffer: binary script
 * Outputs: 	Length (in bytes) of the script
 */
unsigned int scriptLength(unsigned char* buffer){
      return buffer[0] | ((int)buffer[1] << 8);
}

/* checkScript()
 * 
 * This function checks the consistency of the script by running 
 * it through the Fletcher-16 algorithm. It returns 0 if the script
 * is correct.
 * 
 * Inputs: 	buffer: binary script
 * Outputs:	Algorithm result, 0 if the script is correct
 */
char checkScript(unsigned char* buffer){
      //waiting for update from INMS team at UCL
      return 0;
}

/* parseScript()
 * 
 * This function reads a script in form of a binary array
 * and parses it according to the specifications defined in
 * the INMS ICD. Caution: if an invalid script is received, unexpected
 * errors can occur. The result is a structure as defined in the 
 * header file.
 * 
 * Inputs:	buffer: binary script
 * Outputs: 	Parsed script structure
 */
struct Script parseScript(unsigned char* buffer){
      
      // Declare variables
      struct Script script;	// the structure that gets returned in the end
      int i; 			// index used in the binary buffer
      int j,k,l; 		// other indices used in for loops
      char eos; 		// binary flag used in loops
      int script_body_start; 	// used to save the buffer index position
      int num,cmds; 		// variables counting sequences and commands      

      // First 12 bytes are the header
      script.Script_LENGTH = buffer[0] | ((int)buffer[1] << 8);
      script.T_STARTTIME = buffer[2] | ((int)buffer[3] << 8) | ((int)buffer[4] << 16) | ((int)buffer[5] << 24);
      script.FILE_SN = buffer[6] | ((int)buffer[7] << 8) | ((int)buffer[8] << 16) | ((int)buffer[9] << 24);
      script.SW_ver = buffer[10] & 0b00011111;
      script.SU_ID = (buffer[10] & 0b00100000) >> 5;
      script.SCRIPT_TYPE = buffer[11] & 0b00011111;
      script.SU_MD = (buffer[11] & 0b00100000) >> 5;
      
      // Read the checksum
      script.checksum = buffer[script.Script_LENGTH-1] | ((int)buffer[script.Script_LENGTH-2] << 8);

      // Determine the length of the timetable
      i = 16;
      while(buffer[i]!=0x55 && i<script.Script_LENGTH){ // Go through the script looking for EOT (0x55)
	    i = i+4;
      }
      script.tt_length = (i-12)/4;
      script.tt_entries = (struct Entry *)malloc(script.tt_length);
      i++;

      // Read the timetable entries
      for(j=0;j<script.tt_length;j++){
	    script.tt_entries[j].TIME_sec = buffer[12+j*4];
	    script.tt_entries[j].TIME_min = buffer[12+j*4+1];
	    script.tt_entries[j].TIME_hrs = buffer[12+j*4+2];
	    script.tt_entries[j].Script_INDEX = buffer[12+j*4+3];
      }
      
     

      // Determine the number of sequences
      num = 0;
      script_body_start = i;
      while(i<script.Script_LENGTH-2){
	    cmds = 0;
	    eos = 0;
	    while(eos==0 && i<script.Script_LENGTH){ // Look for OBC_EOT (0xFE) marking the end of a sequence
		  if(buffer[i+2]==0xFE) eos = 1;
		  i += buffer[i+3]+4; // Account for variable length of commands due to parameters
		  cmds++;
	    }
	    script.sequences[num].num_commands = cmds;
	    script.sequences[num].commands = malloc(cmds*sizeof(struct Command));
	    num++;
      }
      script.num_sequences = num;

      // Read the sequences
      i = script_body_start;
      for(j=0;j<script.num_sequences;j++){ // for each sequence
	    for(k=0;k<script.sequences[j].num_commands;k++){ // for each command
		  script.sequences[j].commands[k].deltaTimeSec = buffer[i];
		  script.sequences[j].commands[k].deltaTimeMin = buffer[i+1];
		  script.sequences[j].commands[k].CMD_ID = buffer[i+2];
		  script.sequences[j].commands[k].LEN = buffer[i+3];
		  script.sequences[j].commands[k].SEQ_CNT = buffer[i+4];
		  if(buffer[i+3]>1){ // Check if command has parameters	
			script.sequences[j].commands[k].parameters = malloc(buffer[i+3]);
			for(l=0;l<buffer[i+3]-1;l++){
			      script.sequences[j].commands[k].parameters[l] = buffer[i+5+l];	
			}
		  }
		  i += buffer[i+3]+4;
	    }     
      }      
     
      return script;
}


/* main()
 * 
 * This is included here as an example of usage of the parseScript()
 * and readbinFile functions. Since many fields are dynamically
 * allocated, they need to be freed.
 */
int main(){
      unsigned char* buffer;
      int size = readBinFile(&buffer,"example_script.bin");
      int i,j,k;
      struct Script script;
      
      
      script = parseScript(buffer);
      
      /*
       * use script here
       * 
       */
      printf("First command of the first sequence: %X\n",script.sequences[0].commands[0].CMD_ID);
      
      // Clean up. Free all dynamically allocated memory
      // This needs to be done every time a new script gets loaded
      // to prevent memory leaks
      free(buffer);
      free(script.tt_entries);
      for(j=0;j<script.num_sequences;j++){ // for each sequence
	    for(k=0;k<script.sequences[j].num_commands;k++){ // for each command
		  if(script.sequences[j].commands[k].LEN>1){
			free(script.sequences[j].commands[k].parameters);				    
		  }
	    }     
      }
      for(j=0;j<script.num_sequences;j++) free(script.sequences[j].commands);
      
      
}
      
    
