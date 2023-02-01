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
#include "webpage.h"

int main(void){

    char *seed_url = "https://thayer.github.io/engs50/";
    int depth = 0;
    webpage_t *page;
    
    /* initialize webpage */
    if(!(page=webpage_new(seed_url,depth,NULL))){
        printf("Error! Failed to initialize webpage.");
        exit(EXIT_FAILURE);
    }

    /* fetch html */
    if(!webpage_fetch(page)) {
        printf("Error! Failed to fetch html.");
        exit(EXIT_FAILURE);
    }
    
    int pos = 0;
    char *url;
    
    /* scan page and retrieve all urls */
    while ((pos = webpage_getNextURL(page, pos, &url)) > 0) {
        printf("Found url: %s ", url);
        if(IsInternalURL(url))
            printf("[internal]\n");
        else
            printf("[external]\n");
        free(url);
    }

    webpage_delete(page);
    exit(EXIT_SUCCESS);
}
