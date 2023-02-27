/* lhash.c --- locked hashtable implementation
 * 
 * 
 * Author: Abdibaset Bare, Ian Kamweru and Nathaniel Mensah
 * Created: Sat Feb 25 22:30:54 2023 (-0500)
 * Version: 1.0
 * 
 * Description: locked hastable that utilizes multithreading
 */

#include <queue.h>
#include <hash.h>
#include <stdio.h>
#include <lhash.h>
#include <pthread.h>
#include <stdint.h>



pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;//creating a mutext

/*lhopen - opens a locked hashtable
 * return a locked hashtable
*/
lhash_t* lhopen(uint32_t lhsize){
    if (lhsize <= 0)
        return NULL;
    pthread_mutex_lock(&m);
    lhash_t* lhtp = (lhash_t*)hopen(lhsize);
    pthread_mutex_unlock(&m);
    
    if (!lhtp)
        return NULL;
    
    return lhtp;
}

/*lhput - adds an element to the table
 * returns 0 if put was successfully or 1 otherwise 
*/

int32_t lhput(lhash_t* lhtp, void *ep, const char *key, int keylen){
    if (lhtp == NULL || keylen <= 0){
        return 1;
    }
    pthread_mutex_lock(&m);
    int32_t status = hput(lhtp,ep,key,keylen);
    pthread_mutex_unlock(&m);
    return status;
}

/*search through the hashtable to do something with one of the keys*/
void lhapply(lhash_t* lhtp, void(*fn)(void *ep)){
    if (lhtp == NULL || fn == NULL){
        return;
    }
    pthread_mutex_lock(&m);
    happly((hashtable_t*)lhtp, fn);
    pthread_mutex_unlock(&m);
}


/*search for an element using keywords and a helper function -- searchfn
 * lhtp - locked hashtable 
 * searchfn - implemented searchfn that compares current element and searchkey
 * key - search key
 * keylen - length of the searchkey
*/
void* lhsearch(lhash_t *lhtp, bool(*searchfn)(void *ep, const void *searchkeyp), 
                const char *key, int keylen){
    
    if (lhtp == NULL){
        return NULL;
    }
    pthread_mutex_lock(&m);
    void* entry = hsearch((hashtable_t*)lhtp, searchfn, key, keylen);
    pthread_mutex_unlock(&m);
    
    if (!entry){
        return NULL;
    }
    return entry;
}


/*close the locked hashtable upon call*/
void lhclose(lhash_t* lhtp){
    if (lhtp == NULL){
        return;
    }
    pthread_mutex_lock(&m);
    hclose((hashtable_t*)lhtp);
    pthread_mutex_unlock(&m);
    pthread_mutex_destroy(&m);
}

void* lhremove(lhash_t *lhtp, bool(*searchfn)(void *ep, const void *searchkeyp), 
                const char *key, int keylen){
    
    if (lhtp == NULL){
        return NULL;
    }
    pthread_mutex_lock(&m);
    void* data = hremove((hashtable_t*)lhtp, searchfn, key, keylen);
    pthread_mutex_unlock(&m);
    return data;
}