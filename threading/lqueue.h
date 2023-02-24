#pragma once
/* 
 * lqueue.h -- public interface to the locked queue module
 */
#include <stdint.h>
#include <stdbool.h>

/* the queue representation is hidden from users of the module */
typedef void queue_t;		

/* create an empty locked queue */
queue_t* lqopen(void);        

/* deallocate a locked queue, frees everything in it */
void lqclose(queue_t *lqueue);   

/* put element at the end of the locked queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t lqput(queue_t *qp, void *elementp); 

/* get the first first element from locked queue, removing it from the queue */
void* lqget(queue_t *qp);

/* apply a function to every element of the locked queue */
void lqapply(queue_t *qp, void (*fn)(void* elementp));

/* search a locked queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* lqsearch(queue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp);


