/* indexio.c --- indexes the words in crawled pages based on frequency
 * 
 * Author: Ian Kamweru, Abdibaset Bare, Nathaniel Mensah
 * Created: Fri Feb 10 08:30:15 2023 (-0400)
 * Version: 1.0
 * 
 * Description: save and load an index to a named file indexnm. The 
 * index file shall contain one line for each word in the index. 
 * Each line has the format: <word> <docID1> <count1> <docID2> <count2> ....<docIDN> <countN> 
 * <word> is a string of lowercase  letters, <docIDi> is a positive integer designating a document, 
 * <counti> is a positive integer designating the number of occurrences of <word> in <docIDi>; 
 * each entry should be placed on the line separated by a space. 
 * 
 */

#include "indexio.h"

#define hsize 1000    // hashtable size

static FILE *file;

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

/* appends doc id and word count in doc */
static void doc_queue_write_fn(void *elementp){
    document_t *dp = (document_t*)elementp;
    fprintf(file, "%d %d ", dp->id, dp->word_count);
}

static void index_write_fn(void *elementp){
    entry_t *ep = (entry_t*)elementp;
    fprintf(file, "%s ", ep->word);
    qapply(ep->documents, doc_queue_write_fn);
    fprintf(file, "\n");
}

/*
 * indexsave -- save the index to filename indexnm
 * line format: <word> <docID1> <count1> <docID2> <count2> ....<docIDN> <countN> 
 * returns: 0 for success; nonzero otherwise
 */
int32_t indexsave(hashtable_t *index, char *indexnm){

    /* open file */
    file = fopen(indexnm, "w");
    if (file == NULL || access(indexnm, W_OK) != 0){
        printf("Failed to create file: %s\n",indexnm);
        return 1;
    }

    /* write */
    happly(index, index_write_fn);

    fclose(file);
    return 0;
}

/* 
 * indexload -- loads the index from file indexnm
 * returns: non-NULL for success; NULL otherwise
 */
hashtable_t *indexload(char *indexnm){

    /* open file */
    file = fopen(indexnm, "r");
    if(file==NULL || access(indexnm,R_OK) != 0) {
        return NULL;
    }

    hashtable_t *index = hopen(hsize);
    const int MAX_LINE_LEN = 512;
    const int MAX_WORD_LEN = 32;
    char line_buffer[MAX_LINE_LEN];
    memset(line_buffer, 0, sizeof line_buffer);
    char word[MAX_WORD_LEN];
    memset(word, 0, sizeof line_buffer);
    int id=0, word_count=0;

    while (fgets(line_buffer, MAX_LINE_LEN, file)) {
        sscanf(line_buffer, "%s", word);
    
        entry_t *ep = new_entry(word);
        hput(index, ep, word, strlen(word));

        int i = strlen(word) + 1;
        document_t *dp;
        while (sscanf(line_buffer + i, "%d %d", &id, &word_count) == 2) {
            
            if((dp=new_doc(id,word_count))){
                qput(ep->documents,dp);
            }

            i += 2 * sizeof(int);
        }
    }

    fclose(file);
    return index;
}
