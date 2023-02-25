#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <lqueue.h>


#define NUM_THREADS 2
#define NUM_ELEMENTS 10

queue_t* lqp;

typedef struct person{
    char name[10];
    int age;
}person_t;


person_t* makeperson(char *personName, int age){
    person_t *mp;
    
    if (personName == NULL || age <= 0) {
        return NULL;
    }

    if (!(mp=(person_t*)malloc(sizeof(person_t)))) {
        printf("Error in allocating memory\n");
        return NULL;
    }
    strcpy(mp->name, personName);
    mp->age = age;
    return mp;
}

void assert(bool expression){
    if(!expression){
        printf("Assertion failed!\n");
        exit(1);
    }
}

void *put_test(void *arg){
    person_t *p1 = makeperson("one",1);
    person_t *p2 = makeperson("two",2);
    person_t *p3 = makeperson("three",3);
    person_t *p4 = makeperson("four",4);

    printf("================================================\n");
    /* put to empty locked queue */
    printf("adding to empty queue...\n");
    int status = lqput(lqp,p1);
    assert(status==0);
    printf("test put() to empty queue successful\n");

    /* put to non-empty queue */
    printf("adding to non-empty queue...\n");
    status = lqput(lqp,p2);
    assert(status==0);
    status = lqput(lqp,p3);
    assert(status==0);
    status = lqput(lqp,p4);
    assert(status==0);
    printf("test put() to non-empty queue successful\n");

    printf("================================================\n");

    pthread_exit(NULL);
}

void *get_test(void *arg){

   
    person_t *p;

    /* testing when queue is empty queue */
    printf("checking if queue is not empty ...\n");
    p = lqget(lqp);
    printf("%s\n", p->name);
    assert(p!=NULL);
    printf("queue is not empty successful\n");

    /* queue with multiple elements */
    printf("getting from queue with multiple elements...\n");
    p = lqget(lqp);
    printf("%s\n", p->name);
    assert( (strcmp(p->name, "two")) == 0);
    free(p);
    p = lqget(lqp);
    printf("%s\n", p->name);
    assert( (strcmp(p->name, "three")) == 0);
    p = lqget(lqp);
    printf("%s\n", p->name);
    assert( (strcmp(p->name, "four")) == 0);
    printf("test get() from multiple-element queue successful\n");
    printf("======================================================\n");

    pthread_exit(NULL);
}


int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    int t;

    lqp = lqopen();

    // Create threads
    rc = pthread_create(&threads[0], NULL, put_test, NULL);
    if (rc) {
        printf("Error creating put_test thread; return code: %d\n", rc);
        exit(EXIT_FAILURE);
    }

    rc = pthread_create(&threads[1], NULL, get_test, NULL);
    if (rc) {
        printf("Error creating get_test thread; return code: %d\n", rc);
        exit(EXIT_FAILURE);
    }

    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    lqclose(lqp);

    return 0;
}