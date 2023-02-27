/* 
 * lhash_test.c -- tests for the lhash.c module 
 *
 * Author: Ian Kamweru, Abdibaset, Nathaniel Mensah
 * Version: 1.0
 * 
 * Description: tests for single and multithreading for the locked lhash_test.c module
 * 
 */
#include <stdio.h>
#include <lhash.h>
#include <hash.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

typedef struct element{
    char *name;
}element_t;

element_t* entry_init(char* name){
    element_t *entry;
    if((entry = malloc(sizeof(element_t))) == NULL){
        exit(1);
    }
    if((entry->name = malloc(strlen(name)+1)) == NULL){
        exit(1);
    }
    strcpy(entry->name, name);
    return entry;
}
static bool searchfn(void*ep, const void* num){
    element_t* nump = (element_t*)ep;
    return atoi(nump->name) == atoi((char*)num);
}

static void fn(void* ep){
    element_t* nump = (element_t*)ep;
    printf("%d ", atoi(nump->name));
}

void* single_thread(void *arg){
    lhash_t *lhtp = lhopen(100);
    int i = 0;

    while (i < 10){
        int *num = malloc(sizeof(int)); *num = i;
        char key[100];
        sprintf(key, "%d", *num);
        element_t *ep = entry_init(key);
        if (lhput(lhtp, ep, (char*)ep->name, strlen(ep->name)+1) != 0){
            exit(EXIT_FAILURE);
        }
        i++;
        free(num);
    }
    printf("Elements in table before removing ... ");
    lhapply(lhtp, fn);
    printf("\n");

    i--;
    while( i > 5){
        int *n = malloc(sizeof(int)); *n = i;
        char key[100];
        sprintf(key, "%d", *n);
        element_t* ep = (element_t*)lhremove(lhtp, searchfn, key, strlen(key)+1);
        if (ep == NULL || atoi(ep->name) != *n){
           exit(1);
        }
        i--;
        free(ep->name);
        free(ep);
        free(n);

    }

    printf("Elements in table after removing ... ");
    lhapply(lhtp, fn);
    printf("\n");

    //remove all elements before to free memory before closing table
    while (i >= 0){
        int *n = malloc(sizeof(int)); *n = i;
        char key[100];
        sprintf(key, "%d", *n);
        element_t* ep = (element_t*)lhremove(lhtp, searchfn, key, strlen(key)+1);
        if (ep == NULL || atoi(ep->name) != *n){
           exit(1);
        }
        i--;
    
        free(ep->name);
        free(ep); 
        free(n);
    }
    lhclose(lhtp);
    return NULL;

}

void* multi_thread(void* arg){
    lhash_t* lhtp = (lhash_t*)arg;
    static int thread_counter = 1;
    int thread_id = thread_counter++;

    printf("Thread %d starts\n", thread_id);
    for (int i = 0;  i<10; i++){
        int *num = malloc(sizeof(int)); *num = i;
        char key[100];                
        sprintf(key, "%d", *num);
        element_t *ep = entry_init(key);
        printf("Element %s by thread %d\n", ep->name, thread_id);   
        if (lhput(lhtp, ep, ep->name, strlen(ep->name)+1) != 0){
            exit(EXIT_FAILURE);
        }
        free(num);
        sleep(1);
    }
    printf("\nBefore removing from hashtable ");
    lhapply(lhtp, fn);
    printf("\n");

    //remove some elements
    for(int i=0; i < 5; i++){
        int *n = malloc(sizeof(int)); *n = i;
        char key[100];
        sprintf(key, "%d", *n);
        element_t* ep = (element_t*)lhremove(lhtp, searchfn, key, strlen(key)+1);
        printf("Thread %d removed element %s\n", thread_id, ep->name);
        if (ep!=NULL){
            free(ep->name);
            free(ep);
        }
        free(n);
        sleep(1);
    }
    //print remaining elements
    printf("Elements in table after removing ");
    lhapply(lhtp, fn);
    printf("\n");

    //free all memory
    for(int i = 5; i < 10; i++){
        int *n = malloc(sizeof(int)); *n = i;
        char key[100];
        sprintf(key, "%d", *n);
        element_t* ep = (element_t*)lhremove(lhtp, searchfn, key, strlen(key)+1);
        if (ep!=NULL){
            free(ep->name);
            free(ep);
        }
        free(n);
    }

    thread_counter = 1;
    return NULL;
}


int main(void){
    pthread_t sthread_id, mthread_id[3]; 
    lhash_t *lh_shared;
    
    if (pthread_create(&sthread_id, NULL, single_thread, NULL) != 0){
        printf("Couldn't start threadm %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("===========================================\n");
    printf("Single thread begins...\n");
    pthread_join(sthread_id, NULL);
    pthread_detach(sthread_id);
    printf("single thread ends ...\n");
    printf("============================================\n");


    if ((lh_shared = lhopen(1000))== NULL){
        printf("Failed to create shared hashtable\n");
        exit(EXIT_FAILURE);
    }


    printf("Multi_threadst starts...\n");
    for (int i = 0; i < 3; i++){
       if (pthread_create(&mthread_id[i], NULL, multi_thread, lh_shared) != 0){
            printf("Couldn't start mthread %d\n", errno);
            exit(EXIT_FAILURE);
        } 
    }


    for(int i=0; i < 3; i++){
        if(pthread_join(mthread_id[i], NULL));
    }
    lhclose(lh_shared);

    for(int i=0; i < 3; i++){
        pthread_detach(mthread_id[i]);
    } 
    return 0;
}