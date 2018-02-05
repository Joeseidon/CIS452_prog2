/*******************************************************************************
 @author   - Joseph Cutino
 @version  - Winter 2018 

This program ...
*******************************************************************************/
/*Standard Includes*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>

/*Global Defines*/

/*Function Prototypes*/

/*Global Variables*/
int childProcessesCreated = 0;

void main(int argc, char *argv[]){
	
	//prompt user for file which contains the files names to search
	
	
	//read file which holds file names to search
	
	//determine number of children 
	
	//define pipes
		
	
	//while (files left in list)
		//read in name 
		//create a child process with fork()
		
		//if child process
			//set up upstream pipe 
				//dup2 with stdout
			//set up down stream pipe 
				//dup2 with stdin
		
			//call exec using fileSearch.c
			
		//if parent
			//wait for all child process to be created 
			
			//send a file name to each child process
			
			//prompt user for search word 
			
			//send the search word to all children
			
			//wait for the children to report
			
			//loop until user quit //if prompt value is non-alphabetical close children then parent
			
	return;
}

