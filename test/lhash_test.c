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
#include <queue.h>
#include <hash.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static bool searchfn(void*ep, const void* num){
    int data = atoi((char*)ep);
    return data == atoi((char*)num);
}

static void fn(void* ep){
    printf("%d\n", atoi((char*)ep));
}

void* single_thread(void *arg){
    lhash_t *lhtp = lhopen(100);
    int i = 0;

    while (i < 10){
        int *num = malloc(sizeof(int)); *num = i;
        char key[100];
        sprintf(key, "%d", *num);
        if (lhput(lhtp, (void*)key, key, strlen(key)) != 0){
            exit(EXIT_FAILURE);
        }
        i++;
    }

    i--;
    while( i > 5){
        int *n = malloc(sizeof(int)); *n = i;
        char key[100];
        sprintf(key, "%d", *n);
        char* removed = (char*)lhremove(lhtp, searchfn, key, strlen(key));
        if (removed == NULL || atoi(removed) != *n){
           exit(1);
        }
        i--;
    }
    lhapply(lhtp, fn);
    //lhclose(lhtp);
    return NULL;

}
int main(void){
    pthread_t sthread; 

    if (pthread_create(&sthread, NULL, single_thread, NULL) != 0){
        printf("Couldn't start threadm %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Thread begins...\n");
    pthread_join(sthread, NULL);
    pthread_detach(sthread);
    return 0;
}