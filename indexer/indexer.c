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

static int total_count = 0;

/* index entry struct */
typedef struct entry{
	char *word;
	int count;
}entry_p;

/* allocate entry */
entry_p *new_entry(char *word, int count){
	if (count < 0 || word == NULL)
		return NULL;

	entry_p *entry = malloc(sizeof(entry_p));
	if (!entry)
		return NULL;

	entry->word = malloc(strlen(word)+1);
	if (entry->word == NULL)
		return NULL;

	strcpy(entry->word, word);
	entry->count = count;
	
	return entry;
}

static bool searchfn(void *elementp, const void *searchkeyp){
    entry_p *ep = (entry_p*)elementp;
    return strcmp(ep->word,(char*)searchkeyp) == 0;
}

static void sum_fn(void* ep){
	entry_p *p = (entry_p*)ep;
	total_count += p->count;
}

static void NormalizeWord(char *word){
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
	entry_p *ep;
	hashtable_t *index = hopen(hsize);

	while((pos=webpage_getNextWord(page,pos,&word)) > 0){
		NormalizeWord(word);
		if(word[0]!='\0'){
			if (hsearch(index, searchfn, word, strlen(word))){
				ep = (entry_p*)hsearch(index, searchfn, word, strlen(word));
				ep->count = ep->count + 1;
			}
			else{
				ep = new_entry(word, 1);
				hput(index, ep, word, strlen(word));
			}
			printf("%s\n",word);
		}
	}

	happly(index, sum_fn);
	printf("Total word count in hashtable: %d\n", total_count);
	return 0;
}

/*
 * Normalized word count - 141
*/