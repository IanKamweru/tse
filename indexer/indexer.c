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
#include "hash.h"

#define hsize 1000    // hashtable size


typedef struct index{
	char *word;
	int count;
}index_p;

index_p *new_index(char *word, int count){
	if (count < 0 || word == NULL){
		return NULL;
	}

	index_p *index = malloc(sizeof(index_p));
	if (index == NULL){
		return NULL;
	}

	index->word = malloc(strlen(word)+1);
	if (index->word == NULL){
		return NULL;
	}

	strcpy(index->word, word);
	index->count = count;
	
	return index;
}

static bool searchfn(void* elementp, const void* searchkeyp){
    index_p *p = (index_p*)elementp;
    return strcmp(p->word,(char*)searchkeyp) == 0;
}

int* count;

void fn(void* ep){
	index_p *p = (index_p*)ep;
	*count += p->count;
}


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
	index_p *ip;
	hashtable_t *hp = hopen(hsize);
	while((pos=webpage_getNextWord(page,pos,&word)) > 0){
		NormalizeWord(word);
		if(word[0]!='\0'){
			if (hsearch(hp, searchfn, word,strlen(word)) != NULL){
				ip = (index_p*)hsearch(hp, searchfn, word, strlen(word));
				ip->count = ip->count + 1;
			}
			else{
				ip = new_index(word, 1);
				hput(hp, ip, word, strlen(word));
			}
			printf("%s\n",word);
		}
	}
	happly(hp, fn);
	printf("Total word count in hashtable: %d", *count);
	return 0;
}

/*
 * Normalized word count - 141
*/