/* 
 * indexio_test.c -- tests the indexio module
 *
 * Author: Ian Kamweru, Abdibaset, Nathaniel Mensah
 * Version: 1.0
 * 
 * Description: tests the indexsave() and indexload() functions
 * of the indexio utils
 */

#include <stdio.h>
#include "indexio.h"

void free_entry(void *ep){
    entry_t *entryp = (entry_t*)ep;
    free(entryp->word);
    qclose(entryp->documents);
}

int main(void){
    char *indexnm = "test_index";
    printf("Loading index...\n");
    hashtable_t *index = indexload(indexnm);
    if(index){
        printf("Index loaded successfully from: %s\n",indexnm);
    }
    else {
        exit(EXIT_FAILURE);
    }

    char *indexcp = "test_indexcp";
    int status = indexsave(index,indexcp);
    if(status != 0){
        printf("Failed to save index to %s\n",indexcp);
        exit(EXIT_FAILURE);
    }
    printf("Saved index successfully to: %s\n",indexcp);
    happly(index,free_entry);
    hclose(index);

    index = indexload(indexcp);
    if(index){
        printf("Index loaded successfully from: %s\n",indexcp);
    }

    happly(index,free_entry);
    hclose(index);
    exit(EXIT_SUCCESS);
}
