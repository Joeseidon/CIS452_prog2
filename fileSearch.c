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
#include <ctype.h>
#include <unistd.h>

/*Global Defines*/
#define CHAR_BUFFER_LENGTH 256

/*Function Prototypes*/
void exitHandler(int sigNum);
int wordSearch(char *word);
char *strlwr(char *str);
void flush(void);
void waitForInstructions(void);
void reportFindings(void);

/*Global Variables*/
int numberOfMatches = 0;										//Number of word matches found in file
int searchComplete = 0;											//Indicates search process has completed
int remain_active=1;											//Should main loop remain active
char *filename, *searchWord, parentMSG[CHAR_BUFFER_LENGTH];		//Parent message and temp strings for parsing 
FILE *target;													//Target search file

int main(int argc, char *argv[]){
	/* Assign Signal Handler For Exit */ 
    signal(SIGUSR1, exitHandler);
	
	//Expect two cmd args for up and down stream pipes 
	dup2(atoi(argv[1]),fileno(stdout));
	dup2(atoi(argv[2]),fileno(stdin));
	
	while(remain_active){
		//Wait for fileName from Parent on downstream pipe
		waitForInstructions();
		//On filename receive start search 
		numberOfMatches=wordSearch(searchWord);
		//Following search report results on upstream pipe
		reportFindings();
	}
	return 0;
}
void reportFindings(void){
	write(fileno(stdout),&numberOfMatches,sizeof(int));
}
void waitForInstructions(void){
	int i;
	
	read(fileno(stdin),parentMSG,sizeof(parentMSG));
	
	char *token2;
	i=0;
	token2 = strtok(parentMSG, ",");
	while(token2 != NULL){
		//compare tokens to search value
		if(i==0)
			filename = token2;
		else
			searchWord = token2;
		token2 = strtok(NULL, " ");
		i++;
	}
}
void flush(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
int wordSearch(char *word){
	int word_count = 0; //num of target word found
	char line[CHAR_BUFFER_LENGTH];
	char *token;
	
	target = fopen (filename,"r" );
	
	//Confirm file is valid 
	if(target == NULL)
		return 0;
	
	//read line from file	
	while(fscanf(target, "%s", line) == 1){	
		token = strtok(line, " ");
		while(token != NULL){
			//compare tokens to search value
			if(strcmp(token,word) == 0)
				word_count++;
			token = strtok(NULL, " ");
		}		
	}	
	fclose(target);
	return word_count;
}

char *strlwr(char *str)
{
	int i;
	for(i=0; i<sizeof(str); i++){
		str[i]=tolower(str[i]);
	}
	return str;
}

void exitHandler(int sigNum){
	//Clean Up
	if(target != NULL)
		fclose(target);
	
	fprintf(stdout,"Exiting Child: %d\n",getpid());
	
	remain_active=0; //exits main while loop
	
	exit(0);
}