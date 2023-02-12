#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include "pageio.h"
#include "indexio.h"
#include "hash.h"
#include "queue.h"

#define hsize 1000    // hashtable size

static int total_count = 0;

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

/* frees queue of docs and word in index entry */
static void free_entry(void *ep){
	entry_t *p = (entry_t*)ep;
	free(p->word);
	qclose(p->documents);
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
	if (argc!=3){
		printf("usage: indexer <pagedir> <indexnm>\n");
		exit(EXIT_FAILURE);
	}

	char *dirname = argv[1];
	struct stat st_dir;

	/*check if <pagedir> exists*/
	if (stat(dirname, &st_dir) != 0 || !S_ISDIR(st_dir.st_mode)){
		printf("%s doesn't exist\n", dirname);
		exit(EXIT_FAILURE);
	}

	hashtable_t *index = hopen(hsize);
	webpage_t *page;
	DIR *dir;
	struct dirent *dir_entry;

    dir = opendir(dirname);
	/*open directory*/
	if (dir == NULL){
		printf("Failed to open %s\n", dirname);
		exit(EXIT_FAILURE);
	}

	/*looping over files in dir*/
	while ((dir_entry = readdir(dir)) != NULL){ 
		if (dir_entry->d_name[0] != '.'){
			int id = (int)((char)*dir_entry->d_name)- '0';
			page = pageload(id, dirname);
			
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
				free(word);
			}
			webpage_delete(page);
		}	

	}

	happly(index, total_sum_fn);
	printf("Total word count in hashtable: %d\n", total_count);

	closedir(dir);
    if (indexsave(index, argv[2]) != 0){
		exit(EXIT_FAILURE);
	}
	happly(index, free_entry);
	hclose(index);
	exit(EXIT_SUCCESS);
}

/*
 * 1 - 141
 * 2 - 73
 * 3 - 109
*/