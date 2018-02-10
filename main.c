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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

/*Global Defines*/
#define MAX_CHILDREN 10
#define CHAR_BUFFER_LENGTH 256

/*Function Prototypes*/
void flush(void);
void exitHandler(int sigNum);
void waitForInstructions(void);
void waitForChildProcesses(void);
/*Global Variables*/
FILE* collection;
char searchFiles[MAX_CHILDREN][CHAR_BUFFER_LENGTH];
char collection_filename[CHAR_BUFFER_LENGTH];
char searchString[CHAR_BUFFER_LENGTH];
int childProcessesCreated = 0;
int numProcessesNeeded = 0;
int main_run = 1;
int remain_active = 1;

char *filename;
int pvc[MAX_CHILDREN][2][2];
int process_active[MAX_CHILDREN];


int main(int argc, char *argv[]){
	
	//prompt user for file which contains the files names to search
	do{
		fprintf(stdout, "Enter collection file name: ");
		fflush(stdout);
		fgets(collection_filename, sizeof(collection_filename),stdin);
		//Remove trailing '\n'
		collection_filename[strlen(collection_filename)-1]='\0';
		collection = fopen(collection_filename,"r");
	}while(collection == NULL);

	printf("File Found and Opened\n");
	//read file which holds file names to search (max 10 files)
	int i=0;
	for(i=0; i< MAX_CHILDREN; i++){
		printf("Entered For Loop\n");
		if(fgets(searchFiles[i],CHAR_BUFFER_LENGTH, collection)!= NULL){
			printf("Found: %s\n",searchFiles[i]);
			/*Remove trailing '\n' if it exists*/
			if(searchFiles[i][strlen(searchFiles[i])-1]=='\n'){
				searchFiles[i][strlen(searchFiles[i])-1] = '\0';
			}
		}else{
			printf("No files left in collection. Break.\n");
			break;
		}
	}

	//close file collection
	fclose(collection);
	printf("Collection closed.\n");
	
	//determine number of children 
	numProcessesNeeded=i;
	
	/* MOVED TO GLOBAL FOR NOW. NEEDS DYNAMIC MEMEORY ALLOCATION*/
		/*ANYTHING WITH numProcessesNeeded should be dynamicly allocated not globally declared */
	//int pvc[numProcessesNeeded][2][2];
	//int process_active[numProcessesNeeded];
	
	printf("Found %i files to search in collection\n",numProcessesNeeded);
	
	//define pipes
	for(i=0; i<numProcessesNeeded; i++){
		printf("Up and Down stream pipes %i created.\n",i);
		pipe(pvc[i][0]);
		pipe(pvc[i][1]);
	}
	
	while(main_run){
		pid_t pids[10];
		int i;

		/* Create Child Processes */
		for (i = 0; i < numProcessesNeeded; i++) 
		{
			//set up parent side of pipe[i]
			//close(pvc[i][0][0]);	//Child out parent in
			//close(pvc[i][1][1]);	//Parent out Child in
			
			
			// //set up child side of pipe[i]
			// close(pvc[i][0][0]);	//Child out parent in
			// close(pvc[i][1][1]);	//Parent out Child in
			char upstream[CHAR_BUFFER_LENGTH],downstream[CHAR_BUFFER_LENGTH];
			sprintf(upstream,"%d",pvc[i][1][1]);
			sprintf(downstream,"%d",pvc[i][0][0]);
			
			char *cmd[4]={"fileSearch",upstream,downstream,NULL};
			
			//Used to control child process
			process_active[i]=1;
			
			if((pids[i] = fork()) < 0) 
			{
				perror("fork error");
				
			}
			else if (pids[i] == 0) 
			{
				dup2(pvc[i][1][1],fileno(stdout));
				dup2(pvc[i][0][0],fileno(stdin));
				//Child Process
				printf("Child %i created.\n",i);
				
				//call exec passing upstream pipe and downstream pipe as args
				if (execvp(cmd[0],cmd) < 0) {
					perror("exec failed");
					exit(7);
				}
			}
		}
		/*Parent Work Space*/
		printf("Parent Process: Work Space Reached\n");
		
		/*Assign close Signal to parent only*/
		signal (SIGINT, exitHandler);
		
		while(main_run){
			/* Wait for child processes to signal waiting */
			//waitForChildProcesses();
			/* Wait for search string */
			waitForInstructions();
			/* pass search string to child processes with search string */
			 int j = 0;
			 char tmp[CHAR_BUFFER_LENGTH];
			 for(j=0; j<numProcessesNeeded; j++){
				 dup2(pvc[j][0][1],fileno(stdout));
				 strcpy(tmp,searchFiles[j]);
				 strcpy(tmp,strcat(strcat(tmp,","),searchString));
				 //fprintf(stdout,"%s",strcat(strcat(tmp,","),searchString));
				 write(fileno(stdout),tmp,sizeof(tmp));
			 }
			/* Get search responses from pipes */
			for(j=0; j<numProcessesNeeded; j++){
				int count=0;
				dup2(pvc[j][1][0],fileno(stdin));
				//fscanf(stdin,"%i",&count);
				read(fileno(stdin),&count,sizeof(int));
			}
		}
	}
	
	//signal children to stop
	
	//wait for children
	// int status;
	// pid_t childPid;
	// for(i=0; i<numProcessesNeeded; i++){
		// process_active[i]=0; //cancel child process loop
		// /*Signal Child Process to Abort*/
		
		// /*Wait for process to return*/
		// childPid = wait(&status);
		// printf("Child %ld, exited with status = %d.\n", (long)childPid, WEXITSTATUS(status));
	// }
	
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
			
	return 0;
}
void waitForChildProcesses(void){
	printf("Waiting For Children...\n");
	int readyFlag=0,j;
	for(j=0; j<numProcessesNeeded; j++){
		dup2(pvc[j][1][0],fileno(stdin));
		while(readyFlag != 7){
			flush();
			//fscanf(stdin,"%i",&readyFlag);
			read(fileno(stdin),&readyFlag,sizeof(int));
			printf("Ready Flag Recieved: %i",readyFlag);
		}
		readyFlag=0;
	}
}
void waitForInstructions(void){
	int i;
	dup2(0,fileno(stdout));
	dup2(1,fileno(stdin));
	printf("\nEnter Search String: ");
	//fscanf(stdin, "%256[^\n]", searchString);
	fgets(searchString,CHAR_BUFFER_LENGTH,stdin);
	//flush();
	printf("Search String: %s",searchString);
	
	/*Remove trailing '\n' if it exists*/
	if(searchString[strlen(searchString)-1]=='\n'){
		searchString[strlen(searchString)-1] = '\0';
	}
}
void exitHandler(int sigNum){
	dup2(0,fileno(stdout));
	printf("Exit handler reached\n");
	
	//wait for children
	int status,i;
	pid_t childPid;
	for(i=0; i<numProcessesNeeded; i++){
		process_active[i]=0; //cancel child process loop
		/*Signal Child Process to Abort*/
		
		/*Wait for process to return*/
		childPid = wait(&status);
		printf("Child %ld, exited with status = %d.\n", (long)childPid, WEXITSTATUS(status));
	}
	main_run = 0;
	raise(SIGSTOP);
}
void flush(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
//File END