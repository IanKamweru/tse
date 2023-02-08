/* indexer.c --- indexes the words in crawled pages based on frequency
 * 
 * Authors: Abdibaset Bare, Ian Kamweru and Nathaniel Mensah
 * Created: Tue Jan 31 23:57:45 2023 (-0500)
 * Version: 1.0
 * 
 * Description: The indexer is a program that reads the html associated with 
 * every webpage fetched by the crawler; it constructs in memory an index 
 * data structure that can be used to look up a word and find out 1) which documents (in the crawler 
 * directory) contain the word, and 2) how many times the word occurs in that document.  
 * 
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pageio.h"

void NormalizeWord(char *word){
	if(!word)
		return;
	
	int len = strlen(word);

	if(len < 3){
		word[0] = '\0';
	}

	int i;
	for(i=0; i<len; i++){
		if(!isalpha(word[i])){
			word[0] = '\0';
			return;
		}
		word[i] = tolower(word[i]);
	}
}

int main(void){
	char *dirname = "../pages";
	int id = 1;

	webpage_t *page = pageload(id, dirname);
	if(!page)
		return 1;

	int pos = 0;
	char *word;
	while((pos=webpage_getNextWord(page,pos,&word)) > 0){
		NormalizeWord(word);
		if(word[0]!='\0')
			printf("%s\n",word);
	}

	return 0;
}

/*
 * Normalized word count - 141
*/