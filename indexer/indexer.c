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
#include "queue.h"

#define hsize 1000    // hashtable size

static int total_count = 0;

/* index entry struct 
 *
 * @param word - the word to add to the index
 * @param documents - the queue of crawled docs containing the word
*/
typedef struct entry{
	char *word;
	queue_t *documents;
}entry_t;

/* document struct
 *
 * @param id - document id designated by crawler
 * @param word_count - the count of a specific word in the index in this doc
 */
typedef struct document{
	int id;
	int word_count;
} document_t;

/* allocate entry */
entry_t *new_entry(char *word){
	if (!word)
		return NULL;

	entry_t *entry = malloc(sizeof(entry_t));
	if (!entry)
		return NULL;

	entry->documents = qopen();
	if (entry->documents == NULL)
		return NULL;

	entry->word = malloc(strlen(word)+1);
	if (entry->word == NULL)
		return NULL;

	strcpy(entry->word, word);

	return entry;
}

/* allocate document */
document_t *new_doc(int id, int word_count){
	document_t *dp = (document_t*)malloc(sizeof(document_t));
	if(!dp)
		return NULL;
	
	dp->id = id;
	dp->word_count = word_count;
	return dp;
}

/* searches for entry in the hash table */
static bool entry_searchfn(void *elementp, const void *searchkeyp){
    entry_t *ep = (entry_t*)elementp;
    return strcmp(ep->word,(char*)searchkeyp) == 0;
}

/* searches for document in queue */
static bool doc_searchfn(void *elementp, const void *id){
	document_t *dp = (document_t*)elementp;
	return dp->id == *(int*)id;
}

/* total word count in the queue ie. word count for a specific word */
static void queue_sum_fn(void* elementp){
	document_t *dp = (document_t*)elementp;
	total_count+=dp->word_count;
}

/* total word count */
static void total_sum_fn(void* ep){
	entry_t *p = (entry_t*)ep;
	qapply(p->documents,queue_sum_fn);
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

int main(int argc, char *argv[]){
	char *dirname = "../pages";
	int end_id = atoi(argv[1]);
	int id = 1;

	hashtable_t *index = hopen(hsize);
	webpage_t *page;

	while((page = pageload(id, dirname)) && id<=end_id){
		if(!page)
			exit(EXIT_FAILURE);

		int pos = 0;
		char *word;
		entry_t *ep;

		while((pos=webpage_getNextWord(page,pos,&word)) > 0){
			NormalizeWord(word);
			if(word[0]!='\0'){
				if (hsearch(index, entry_searchfn, word, strlen(word))){
					ep = (entry_t*)hsearch(index, entry_searchfn, word, strlen(word));
					document_t *dp;
					if((dp = qsearch(ep->documents,doc_searchfn,&id))){
						dp->word_count = dp->word_count + 1;
					}
					else{
						dp = new_doc(id,1);
						qput(ep->documents,dp);
					}
				}
				else{
					ep = new_entry(word);
					document_t *dp = new_doc(id,1);
					qput(ep->documents,dp);
					hput(index, ep, word, strlen(word));
				}
				printf("%s\n",word);
			}
		}
		id++;
	}
	happly(index, total_sum_fn);
	printf("Total word count in hashtable: %d\n", total_count);
	exit(EXIT_SUCCESS);
}

/*
 * 1 - 141
 * 2 - 73
 * 3 - 109
*/