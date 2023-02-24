#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lqueue.h"
#include "queue.h"

typedef struct person{
    char name[10];
    int age;
}person_t;


static person_t* makeperson(char *personName, int age){
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

static void assert(bool expression){
    if(!expression){
        printf("Assertion failed!\n");
        exit(1);
    }
}

/* search function */
static bool searchfn(void *elementp, const void *keyp){
    return strcmp(((person_t*)(elementp))->name, (char*)keyp) == 0;
}

static void put_test(void){
    queue_t *q = lqopen();

    person_t *p1 = makeperson("one",1);
    person_t *p2 = makeperson("two",1);

    printf("================================================\n");
    /* put to empty locked queue */
    printf("adding to empty queue...\n");
    int status = lqput(q,p1);
    assert(status==0);
    printf("test put() to empty queue successful\n");

    /* put to non-empty queue */
    printf("adding to non-empty queue...\n");
    status = lqput(q,p2);
    assert(status==0);
    person_t *p = lqget(q);
    assert(p==p1);
    free(p);
    p = lqget(q);
    assert(p==p2);
    free(p);
    printf("test put() to non-empty queue successful\n");
    lqclose(q);

    printf("================================================\n");
}

static void get_test(void){
    person_t *p1, *p2, *p3;
    p1 = makeperson("one",1), p2 = makeperson("two",2), p3 = makeperson("three",3);
    queue_t *qp = NULL;
    person_t *p;

    /* null queue */
    printf("===================================================\n");
    printf("getting from null queue...\n");
    p = lqget(qp);
    assert(p==NULL);
    printf("test get() from null queue successful\n");

    qp = lqopen();
    /* empty queue */
    printf("getting from empty queue...\n");
    p = lqget(qp);
    assert(p==NULL);
    printf("test get() from empty queue successful\n");

    /* queue with one element */
    printf("getting from queue with one element...\n");
    int status = lqput(qp, p1);
    assert(status==0);
    p = lqget(qp);
    assert(p==p1);
    printf("test get() from one-element queue successful\n");

    /* queue with multiple elements */
    printf("getting from queue with multiple elements...\n");
    lqput(qp, p); lqput(qp, p2); lqput(qp, p3);
    p = lqget(qp);
    assert(p==p1);
    free(p);
    printf("test get() from multiple-element queue successful\n");

    printf("======================================================\n");

    lqclose(qp);
}


static void search_test(void){
    queue_t *qp = NULL;
    person_t *p1, *p2, *p3, *p4;
    person_t *search_res;   //return qsearch
    int res1, res2, res3, res4; //return of qput
    const void *str;  //string parsed 

    printf("Search with unopened queue: ...");
    search_res = (person_t*)lqsearch(qp, searchfn, "one");

    assert(search_res == NULL);
    printf("Success!\n");

    printf("Search with empty/invalid string: ...");
    qp = lqopen();

    p1 = makeperson("one", 30); p2 = makeperson("two", 50);
    p3 = makeperson("three", 21); p4 = makeperson("four", 40);
    res1 = lqput(qp, p1); res2 = lqput(qp, p2); res3 = lqput(qp, p3);
    res4 = lqput(qp, p4);
    assert(res1 == 0 && res2 == 0 && res3 == 0 && res4 == 0);
    search_res = lqsearch(qp, searchfn, " ");
    assert(search_res == NULL);
    printf("Success!\n");

    printf("Search with valid string:...");
    str = "one";
    search_res = lqsearch(qp, searchfn, str);
    assert(strcmp(((person_t*)search_res)->name, p1->name)== 0);
    printf("Success!\n");
    lqclose(qp);
}

int main(void){
    put_test();
    get_test();
    search_test();

    return 0;
}