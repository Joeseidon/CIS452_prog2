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
#define MAX_CHILDREN 10

/*Function Prototypes*/

/*Global Variables*/
FILE* collection;
char *searchFiles[MAX_CHILDREN];
char *collection_filename;
int childProcessesCreated = 0;
int numProcessesNeeded = 0;
int pvc[MAX_CHILDREN][2];


void main(int argc, char *argv[]){
	
	//prompt user for file which contains the files names to search
	do{
		fprintf(stdout, "Enter collection file name: ");
		fscanf(stdin, "%s\n",collection_filename);
		collection = fopen(collection_filename,"r");
	}while(collection == NULL);
	
	//read file which holds file names to search (max 10 files)
	int i=0;
	while(fgets(searchFiles[i],MAX_CHAR_COUNT, collection)!= NULL && i<MAX_CHILDREN){
		i++;
	}
	
	//determine number of children 
	numProcessesNeeded=i;
	
	//define pipes
	for(i=0; i<numProcessesNeeded; i++){
		pipe(pvc[i]);
	}
	
	while(1){
		
	}
	
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

