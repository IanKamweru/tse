#pragma once
/* lhash.h --- locked hashtable header file
 * 
 * 
 * Author: Abdibaset Bare, Ian Kamweru and Nathaniel Mensah
 * Created: Sat Feb 25 22:30:54 2023 (-0500)
 * Version: 1.0
 * 
 * Description: prototypes for locked hashtable functions
 * 
 */
#include <stdint.h>
#include <stdbool.h>

typedef void lhash_t;

/*lhopen - creates a new locked hashtable 
 *returns the created hashtable 
*/
lhash_t* lhopen(uint32_t lhsize);

/*lhput - adds an element to the hashtabe using the key given
 * return 0 if sucessfull or 1 otherwise 
*/
int32_t lhput(lhash_t* htp, void *ep, const char *key, int keylen);

/*apply something to an element of the hashtable*/
void lhapply(lhash_t* htp, void(*fn)(void *ep));

/*searches for element using keyword given
 * return the queue-search result
*/
void* lhsearch(lhash_t* htp, bool(*searchfn)(void *ep, const void *searchkeyp), 
                const char *key, int keylen);

/*hclose - closes the hastable and clears up memory
*/
void lhclose(lhash_t* htp);

void* lhremove(lhash_t *lhtp, bool(*searchfn)(void *ep, const void *searchkeyp), 
                const char *key, int keylen);