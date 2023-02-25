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

// /* search function */
// static bool searchfn(void *elementp, const void *keyp){
//     return strcmp(((person_t*)(elementp))->name, (char*)keyp) == 0;
// }

void *put_test(void *arg){
    person_t *p1 = makeperson("one",1);
    person_t *p2 = makeperson("two",1);

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
    printf("test put() to non-empty queue successful\n");

    printf("================================================\n");

    pthread_exit(NULL);
}

void *get_test(void *arg){

    person_t *p1, *p2, *p3;
    p1 = makeperson("one",1), p2 = makeperson("two",2), p3 = makeperson("three",3);
    person_t *p;

    /* empty queue */
    // printf("getting from empty queue...\n");
    // p = lqget(lqp);
    // assert(p==NULL);
    // printf("test get() from empty queue successful\n");

    /* queue with one element */
    // printf("getting from queue with one element...\n");
    // // int status = lqput(lqp, p1);
    // // assert(status==0);
    // p = lqget(lqp);
    // printf("%s\n", p->name);
    // assert(p==p1);
    // printf("test get() from one-element queue successful\n");

    /* queue with multiple elements */
    printf("getting from queue with multiple elements...\n");
    // lqput(lqp, p); lqput(lqp, p2); lqput(lqp, p3);
    p = lqget(lqp);
    assert(p==p1);
    free(p);
    p = lqget(lqp);
    assert(p==p2);
    p = lqget(lqp);
    assert(p==p3);
    printf("test get() from multiple-element queue successful\n");

    printf("======================================================\n");

    pthread_exit(NULL);
}


// static void *search_test(void *arg){
    
//     person_t *p1, *p2, *p3, *p4;
//     person_t *search_res;   //return qsearch
//     int res1, res2, res3, res4; //return of qput
//     const void *str;  //string parsed 

//     printf("Search with unopened queue: ...");
//     search_res = (person_t*)lqsearch(lqp, searchfn, "one");

//     assert(search_res == NULL);
//     printf("Success!\n");

//     printf("Search with empty/invalid string: ...");

//     p1 = makeperson("one", 30); p2 = makeperson("two", 50);
//     p3 = makeperson("three", 21); p4 = makeperson("four", 40);
//     res1 = lqput(lqp, p1); res2 = lqput(lqp, p2); res3 = lqput(lqp, p3);
//     res4 = lqput(lqp, p4);
//     assert(res1 == 0 && res2 == 0 && res3 == 0 && res4 == 0);
//     search_res = lqsearch(lqp, searchfn, " ");
//     assert(search_res == NULL);
//     printf("Success!\n");

//     printf("Search with valid string:...");
//     str = "one";
//     search_res = lqsearch(lqp, searchfn, str);
//     assert(strcmp(((person_t*)search_res)->name, p1->name)== 0);
//     printf("Success!\n");

//     pthread_exit(NULL);

// }



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

    // rc = pthread_create(&threads[2], NULL, search_test, NULL);
    // if (rc) {
    //     printf("Error creating search_test thread; return code: %d\n", rc);
    //     exit(EXIT_FAILURE);
    // }    

    // Wait for threads to finish
    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    lqclose(lqp);

    return 0;
}