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
#include <dirent.h>
#include "pageio.h"
#include "hash.h"
#include "queue.h"

#define hsize 1000   // hashtable size

static int total_count = 0;
static int count_allDir = 0;
/* index entry struct */
typedef struct entry{
	queue_t *docs;
	char *word;
	int count;
}entry_p;

/*added a queue to list of documents it appears*/
entry_p *entryAllDocs(char *word, queue_t *qp){
	if (word == NULL)
		return NULL;
	
	entry_p *entry = malloc(sizeof(entry_p));
	if (!entry)
		return NULL;
	
	entry->word = malloc(strlen(word)+1);
	if (entry->word == NULL)
		return NULL;
	
	strcpy(entry->word, word);
	entry->docs = qp;
	return entry;
}
/* allocate entry */
// entry_p *new_entry(char *word, int count){
	// if (count < 0 || word == NULL)
		// return NULL;
// 
	// entry_p *entry = malloc(sizeof(entry_p));
	// if (!entry)
		// return NULL;
// 
	// entry->word = malloc(strlen(word)+1);
	// if (entry->word == NULL)
		// return NULL;
// 
	// strcpy(entry->word, word);
	// entry->count = count;
	// 
	// return entry;
// }
// 
static bool searchfn(void *elementp, const void *searchkeyp){
    entry_p *ep = (entry_p*)elementp;
    return strcmp(ep->word,(char*)searchkeyp) == 0;
}

// static void sum_fn(void* ep){
	// entry_p *p = (entry_p*)ep;
	// total_count += p->count;
// }

static void sum_all(void* ep){
	printf("%d ", (int)ep);
	count_allDir = count_allDir + 1;
}

static void sum_fn_AllDir(void* ep){
	printf("\n");
	queue_t *qp = ((entry_p*)ep)->docs;
	entry_p *p = (entry_p*)ep;

	printf("'%s': ", p->word);
	qapply(qp, sum_all);
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
	DIR *dir;

	struct dirent *entry;	
	hashtable_t *allWords = hopen(hsize);
	
	if ((dir = opendir(dirname)) == NULL) {
		fprintf(stderr, "Can't open %s\n", dirname);
		exit(EXIT_FAILURE);
	};

	while ((entry=readdir(dir)) != NULL) {
		if ((int)*entry->d_name != 46){
			int id = (int)((char)((int)*entry->d_name)- '0');
			webpage_t *page = pageload(id, dirname);

			if(!page){
				printf("failed here\n");
				return 1;
			}

			int pos = 0;
			char *word;
			entry_p *ep;
			// hashtable_t *index = hopen(hsize);
	
			while((pos=webpage_getNextWord(page,pos,&word)) > 0){
				NormalizeWord(word);
				if(word[0]!='\0'){
					// if (hsearch(index, searchfn, word, strlen(word))){
						// ep = (entry_p*)hsearch(index, searchfn, word, strlen(word));
						// ep->count = ep->count + 1;
					// }
					// else{
						// ep = new_entry(word, 1);
						// hput(index, ep, word, strlen(word));
					// }
					// printf("%s\n",word);

					if (hsearch(allWords, searchfn, word, strlen(word))){
						ep = (entry_p*)hsearch(allWords, searchfn, word, strlen(word));
						int32_t status = qput(ep->docs, id);
						if (status != 0){
							exit(EXIT_FAILURE);
						}
					}
					else{
						queue_t *qp = qopen(); 
						ep = entryAllDocs(word, qp);
						int32_t status = qput(ep->docs, id);
						if (status != 0){
							printf("failed here");
							exit(EXIT_FAILURE);
						}
						hput(allWords, ep, word, strlen(word));
					}
				}
			}
			// happly(index, sum_fn);
			// happly(allWords, sum_fn_AllDir);
			// printf("Total word count in hashtable: %d\n", total_count);
		}
	}
	happly(allWords, sum_fn_AllDir);
	printf("\nTotal word count in all documents: %d\n", count_allDir);
	closedir(dir);
	return 0;
}

/*
 * Normalized word count - 141
*/