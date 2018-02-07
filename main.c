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
#define CHAR_BUFFER_LENGTH 256

/*Function Prototypes*/
void exitHandler(int sigNum);
int wordSearch(char *word);

/*Global Variables*/
FILE* collection;
char *searchFiles[MAX_CHILDREN];
char *collection_filename;
int childProcessesCreated = 0;
int numProcessesNeeded = 0;
int main_run = 1;
int remain_active = 1;



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
	//close file collection
	fclose(collection);
	
	//determine number of children 
	numProcessesNeeded=i;
	
	int pvc[numProcessesNeeded][2][2];
	int process_active[numProcessesNeeded];
	
	//define pipes
	for(i=0; i<numProcessesNeeded; i++){
		pipe(pvc[i]);
	}
	
	while(main_run){
		pid_t pids[10];
		int i;

		/* Create Child Processes */
		for (i = 0; i < numProcessesNeeded; i++) {
			//set up parent side of pipe[i]
			close(pvc[i][0][1]);	//Child out parent in
			close(pvc[i][1][0]);	//Parent out Child in
			
			//Used to control child process
			process_active[i]=1;
			
			if ((pids[i] = fork()) < 0) {
				perror("fork");
				abort();
			} else if (pids[i] == 0) {
				//Child Process

				//set up child side of pipe[i]
				close(pvc[i][0][0]);	//Child out parent in
				close(pvc[i][1][1]);	//Parent out Child in
				char upstream[MAX_CHAR_COUNT],downstream[MAX_CHAR_COUNT];
				sprintf(upstream,"%s",pvc[i][0][1]);
				sprintf(downstream,"%s",pvc[i][1][0]);
				
				char *cmd[4]={"fileSearch",upstream,downstream,NULL};
				
				//call exec passing upstream pipe and downstream pipe as args
				if (execvp(cmd[0],cmd) < 0) {
					perror("exec failed");
					exit(7);
				}
		}
		
		printf("Parent Thread: Work Space Reached\n");
	}
	
	//signal children to stop
	
	//wait for children
	int status;
	pid_t childPid;
	for(i=0; i<numProcessesNeeded; i++){
		process_active[i]=0; //cancel child process loop
		childPid = wait(&status);
		printf("Child %ld, exited with status = %d.\n", (long)pid, WEXITSTATUS(status));
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

int wordSearch(char *word){
	int word_count = 0; //num of target word found
	char line[CHAR_BUFFER_LENGTH];
	char *token;
	
	target = fopen (filename,"r" );
	
	//confirm file is valid 
	if(target == NULL)
		return 0;
	
	//read line from file
	while(fgets(line, sizeof(line),target) != NULL){		
		token = strtok(line, " ");
		while(token != NULL){
			//compare tokens to search value
			if(strstr(token,word))
				word_count++;
			token = strtok(NULL, " ");
		}		
	}	
	return word_count;
}

void exitHandler(int sigNum){
	//Clean Up if needed	
	remain_active=0; //exits main while loop
}
