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

/*Global Defines*/
#define CHAR_BUFFER_LENGTH 256

/*Function Prototypes*/
void exitHandler(int sigNum);
int wordSearch(char *word);
char *strlwr(char *str);
void flush(void);
void waitForInstructions(void);

/*Global Variables*/
int numberOfMatches = 0;
int searchComplete = 0;
char *filename, *searchWord, parentMSG[CHAR_BUFFER_LENGTH],*token2;
FILE *target;

int main(int argc, char *argv[]){
	/* Assign Signal Handler For Exit */ 
    signal(SIGUSR1, exitHandler);
	
	while(1){
		//Wait for fileName from Parent on downstream pipe
		waitForInstructions();
		//On filename receive start search 
		numberOfMatches=wordSearch(searchWord);
		//following search report results on upstream pipe
		fprintf(stdout, "%d",numberOfMatches);
		break;
	}
	return 0;
}
void waitForInstructions(void){
	int i;
	fscanf(stdin, "%256[^\n]", parentMSG);
	flush();
	
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
		
	printf("Filename: %s  SearchWord: %s",filename,searchWord);
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
	exit(0);
}