/*
 * lqueue.c -- Locked Queue ADT
 *
 * Author: Ian Kamweru, Abdibaset, Nathaniel Mensah
 * Version: 1.0
 * 
 * Description: implementation of Locked Queue ADT for multi-threaded processing
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include <pthread.h>

;
pthread_mutex_t mutex;

/* initialize empty locked queue */
queue_t* lqopen(void){
    pthread_mutex_lock(&mutex); // Lock the mutex
    queue_t* lqueue = qopen();
    pthread_mutex_unlock(&mutex); // unlock the mutex
    return lqueue;
}

/* deallocate a locked queue, frees everything in it */
void lqclose(queue_t *lqueue) {
    pthread_mutex_lock(&mutex); // Lock the mutex
    qclose(lqueue);
    pthread_mutex_unlock(&mutex); // unlock the mutex
}

/* put element at the end of the locked queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t lqput(queue_t *lqueue, void* elementp) {
    int32_t status; // keep track of whether the operation was successful
    pthread_mutex_lock(&mutex); // Lock the mutex
    status = qput(lqueue, elementp);
    pthread_mutex_unlock(&mutex); // unlock the mutex
    return status;
}

/* get the first first element from locked queue, removing it from the queue */
void* lqget(queue_t *lqueue) {
    pthread_mutex_lock(&mutex); // Lock the mutex
    void *data = qget(lqueue);
    pthread_mutex_unlock(&mutex); // unlock the mutex
    return data;
}

/* apply a function to every element of the locked queue */
void lqapply(queue_t *lqueue, void (*fn)(void* elementp)){
    pthread_mutex_lock(&mutex); // Lock the mutex
    qapply(lqueue, fn);
    pthread_mutex_unlock(&mutex); // unlock the mutex
}

/* search a locked queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- element - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* lqsearch(queue_t *lqueue, bool (*searchfn)(void* element,const void* keyp),const void* skeyp){
    pthread_mutex_lock(&mutex); // Lock the mutex
    void* data = qsearch(lqueue, searchfn, skeyp);
    pthread_mutex_unlock(&mutex); // unlock the mutex
    return data;
}