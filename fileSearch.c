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
int numberOfMatches = 0;
int searchComplete = 0;
int remain_active=1;
char *filename, *searchWord, parentMSG[CHAR_BUFFER_LENGTH];
FILE *target;

int main(int argc, char *argv[]){
	/* Assign Signal Handler For Exit */ 
    signal(SIGUSR1, exitHandler);
	//expect two cmd args for pipe 
	dup2(atoi(argv[1]),fileno(stdout));
	dup2(atoi(argv[2]),fileno(stdin));
	
	while(remain_active){
		//Wait for fileName from Parent on downstream pipe
		waitForInstructions();
		//On filename receive start search 
		numberOfMatches=wordSearch(searchWord);
		//following search report results on upstream pipe
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
	
	//confirm file is valid 
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
	//Clean Up if needed 
	
	fprintf(stdout,"Exiting Child: %d\n",getpid());
	
	remain_active=0; //exits main while loop
	
	exit(0);
}