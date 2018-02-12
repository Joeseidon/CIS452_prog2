/*******************************************************************************
 @author   - Joseph Cutino
 @version  - Winter 2018 

This program creates instances of fileSearch.c in order to search multiple files
for a user provided key word. When te user enters an non-alphabetic character,
this program will kill all instances of fileSearch.c and close.
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
#include <ctype.h>

/*Global Defines*/
#define MAX_CHILDREN 10
#define CHAR_BUFFER_LENGTH 256

/*Function Prototypes*/
void flush(void);
void exitHandler(int sigNum);
void waitForInstructions(void);
void waitForChildProcesses(void);
void summerizeResults(void);

/*Global Variables*/
FILE* collection;											//User provided file containing search file names
char searchFiles[MAX_CHILDREN][CHAR_BUFFER_LENGTH];			//Search file names read from collection
char collection_filename[CHAR_BUFFER_LENGTH];				//User provided file name containing search file names
char searchString[CHAR_BUFFER_LENGTH];						//Users provided key word to search for
int childProcessesCreated = 0;								//Number of child processes that have been created
int numProcessesNeeded = 0;									//Number of processes needed based on number of search files

int main_run = 1;											//Should main process remain running

int pvc[MAX_CHILDREN][2][2];								//Up and down stream pipes for parent <-> child communication

pid_t childpids[10];										//Child process pids

int searchCount[MAX_CHILDREN];								//Count return from each search

int main(int argc, char *argv[]){
	//prompt user for file which contains the files names to search
	do{
		fprintf(stdout, "Enter collection file name: ");
		fflush(stdout);
		fgets(collection_filename, sizeof(collection_filename),stdin);
		
		//Remove trailing '\n'
		if((collection_filename[strlen(collection_filename)-1]='\n')){
			collection_filename[strlen(collection_filename)-1]='\0';
		}
		
		//Attempt to open collection
		collection = fopen(collection_filename,"r");
	}while(collection == NULL);

	//read file which holds file names to search (max 10 files)
	int i=0;
	for(i=0; i< MAX_CHILDREN; i++){
		if(fgets(searchFiles[i],CHAR_BUFFER_LENGTH, collection)!= NULL){
			/*Remove trailing '\n' if it exists*/
			if(searchFiles[i][strlen(searchFiles[i])-1]=='\n'){
				searchFiles[i][strlen(searchFiles[i])-1] = '\0';
			}
		}else{
			//all processes created
			break;
		}
	}

	//close file collection
	fclose(collection);
	
	//determine number of children 
	numProcessesNeeded=i;
		
	printf("Found %i files to search in collection\n",numProcessesNeeded);
	
	//define pipes
	for(i=0; i<numProcessesNeeded; i++){
		pipe(pvc[i][0]);
		pipe(pvc[i][1]);
	}
	
	while(main_run){
		pid_t pids[10];
		int i,k=0;

		/* Create Child Processes */
		for (i = 0; i < numProcessesNeeded; i++) 
		{
			//Create parameter package for exec call
			char upstream[CHAR_BUFFER_LENGTH],downstream[CHAR_BUFFER_LENGTH];
			sprintf(upstream,"%d",pvc[i][1][1]);
			sprintf(downstream,"%d",pvc[i][0][0]);
			
			char *cmd[4]={"fileSearch",upstream,downstream,NULL};
			
			if((pids[i] = fork()) < 0) 
			{
				perror("fork error");
			}
			else if (pids[i] == 0) 
			{
				//Child Process
				
				//Call exec passing upstream pipe and downstream pipe as args
				if (execvp(cmd[0],cmd) < 0) {
					perror("exec failed");
					exit(7);
				}
			}else{
				//Add child pids to storage area of later signal use
				childpids[k]=pids[i];
				k++;
			}
			
		}
		/*Parent Work Space*/
		printf("%d fileSearch instances created.\n",numProcessesNeeded);
		
		/*Assign close Signal to parent only*/
		signal (SIGINT, exitHandler);
		
		while(main_run){
			/* Wait for search string */
			waitForInstructions();
			/* Pass search file to child processes with search string */
			 int j = 0;
			 char tmp[CHAR_BUFFER_LENGTH];
			 for(j=0; j<numProcessesNeeded; j++){
				 strcpy(tmp,searchFiles[j]);
				 strcpy(tmp,strcat(strcat(tmp,","),searchString));
				 printf("Sent search criteria to child: %ld\n", (long)childpids[j]);
				 write(pvc[j][0][1],tmp,sizeof(tmp));
			 }
			/* Get search responses from pipes */
			for(j=0; j<numProcessesNeeded; j++){
				printf("Waiting for Search Results From Process %d...\n",j);
				int count=0;
				read(pvc[j][1][0],&count,sizeof(int));
				printf("Process %i sent back: %i\n",j,count);
				searchCount[i]=count;
			}
			summerizeResults();
		}
	}		
	return 0;
}

void summerizeResults(void){
	int x;
	int total;
	for(x=0; x<numProcessesNeeded; x++){
		total+=searchCount[x];
	}
	printf("We Found, %s, %d times in the provided files.\n",searchString,total);
}

void waitForInstructions(void){
	printf("\nEnter Search String: ");

	fgets(searchString,CHAR_BUFFER_LENGTH,stdin);
	
	if(!isalpha(searchString[0])){
		raise(SIGINT);
	}
	
	/*Remove trailing '\n' if it exists*/
	if(searchString[strlen(searchString)-1]=='\n'){
		searchString[strlen(searchString)-1] = '\0';
	}
}
void exitHandler(int sigNum){
	printf("\nShutdown sequence:\n");
	
	//wait for children
	int status,i;
	pid_t childPid;
	for(i=0; i<numProcessesNeeded; i++){
		printf("Sent kill cmd to child: %ld\n",(long)childpids[i]);
		/*Signal Child Process to Abort*/
		kill(childpids[i],SIGUSR1);
		/*Wait for process to return*/
		childPid = wait(&status);
		printf("Child %ld, exited with status = %d.\n", (long)childPid, WEXITSTATUS(status));
		
		//Close Pipes
		close(pvc[i][0][0]);
		close(pvc[i][0][1]);
		close(pvc[i][1][0]);
		close(pvc[i][1][1]);
	}
	main_run = 0;
	raise(SIGQUIT);
}
void flush(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
//File END