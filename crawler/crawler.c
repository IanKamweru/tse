/* crawler.c --- web crawler
 * 
 * 
 * Authors: Abdibaset Bare, Ian Kamweru and Nathaniel Mensah
 * Created: Tue Jan 31 23:57:45 2023 (-0500)
 * Version: 1.0
 * 
 * Description: crawls a website and extracts embedded urls for its webpages
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"

#define hsize 100    //declaring hashtable size

bool searchfn(void* elementp, const void* searchkeyp){
    char *p = (char*)elementp;
    return strcmp(p,(char*)searchkeyp) == 0;
}

int main(void){

    char *seed_url = "https://thayer.github.io/engs50/";
    int depth = 0;
    webpage_t *homepage;
    
    /* initialize webpage */
    if(!(homepage=webpage_new(seed_url,depth,NULL))){
        printf("Error! Failed to initialize webpage.");
        exit(EXIT_FAILURE);
    }

    /* fetch html */
    if(!webpage_fetch(homepage)) {
        printf("Error! Failed to fetch html.");
        exit(EXIT_FAILURE);
    }
    
    queue_t *qp = qopen();
    hashtable_t *hp = hopen(hsize);
    webpage_t *page;
    char *url;
    int pos = 0;
    
    /* scan page and retrieve all urls */
    while ((pos = webpage_getNextURL(homepage, pos, &url)) > 0) {
        printf("Found url: %s ", url);
        if(IsInternalURL(url)) {
            printf("[internal]\n");
            
            if (hsearch(hp, searchfn, url, strlen(url)) == NULL){
                if(!(page=webpage_new(url,0,NULL))) {
                    printf("Error! Failed to initialize internal webpage.");
                    exit(EXIT_FAILURE);
                }

                if(!webpage_fetch(page)) {
                    printf("Error! Failed to fetch html from internal page.");
                    exit(EXIT_FAILURE);
                }
                qput(qp, page);
                hput(hp,url,url,strlen(url));
            }
            else{
                printf("[url: %s already in queue]\n",url);
                free(url);
            }
        }
        else{
            printf("[external]\n");
            free(url);
        }
    }

    /* print queue */
    printf("******************\n\n");
    printf("Internal Page Queue:\n");
    page = qget(qp);
    for(; page!=NULL; page=qget(qp)){
        printf("%s\n",webpage_getURL(page));
        webpage_delete(page);
    }

    qclose(qp);
    hclose(hp);
    webpage_delete(homepage);
    exit(EXIT_SUCCESS);
}
