/* crawler.c --- web crawler
 * 
 * 
 * Authors: Abdibaset Bare, Ian Kamweru and Nathaniel Mensah
 * Created: Tue Jan 31 23:57:45 2023 (-0500)
 * Version: 1.0
 * 
 * Description: crawls a website extracting embedded urls
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"

#define hsize 100    // hashtable size

static bool searchfn(void* elementp, const void* searchkeyp){
    char *p = (char*)elementp;
    return strcmp(p,(char*)searchkeyp) == 0;
}

int main(int argc, char *argv[]){
    if (argc != 4) {
        printf("Usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        exit(EXIT_FAILURE);
    }

    /* use case: crawler https://thayer.github.io/engs50/ ../pages 2 */
    char *seed_url = malloc(sizeof(char) * strlen(argv[1]) + 1);
    strcpy(seed_url, argv[1]);
    char *dirname = argv[2];
    int max_depth = atoi(argv[3]);
    if(!seed_url || !dirname){
        printf("Error: invalid seed_url or save directory.\n");
        exit(EXIT_FAILURE);
    }
    if(max_depth < 0){
        printf("Error: Max_depth must be 0 or greater.\n");
        exit(EXIT_FAILURE);
    }

    /* check save directory */
    struct stat st;
    if (stat(dirname, &st) != 0 || !S_ISDIR(st.st_mode)){
        if ((mkdir(dirname, 0755))!= 0){
            printf("Failed to create save directory: %s\n", dirname);
            exit(EXIT_FAILURE);
        }
    }

    /* initialize seed_page */
    webpage_t *seed_page;
    if(!(seed_page=webpage_new(seed_url,0,NULL))){
        printf("Error! Failed to initialize webpage.");
        exit(EXIT_FAILURE);
    }

    /* fetch html */
    if(!webpage_fetch(seed_page)) {
        printf("Error! Failed to fetch html.");
        exit(EXIT_FAILURE);
    }
    
    queue_t *qp = qopen();
    hashtable_t *hp = hopen(hsize);
    qput(qp,seed_page);
    hput(hp,seed_url,seed_url,strlen(seed_url));
    pagesave(seed_page,1,dirname);

    int pos = 0, depth = 0, id = 2;
    webpage_t *page, *curr;
    char *url;
    int status;

    /* BFS */
    while((curr=(webpage_t*)qget(qp))){
        depth = webpage_getDepth(curr);
        pos = 0;

        /* crawl page and retrieve all urls */
        while (depth<max_depth && (pos = webpage_getNextURL(curr, pos, &url)) > 0) {
            printf("Found url: %s ", url);
            if(IsInternalURL(url)) {
                printf("[internal]\n");
                
                if (hsearch(hp, searchfn, url, strlen(url)) == NULL){
                    if(!(page=webpage_new(url,depth+1,NULL))) {
                        printf("Error! Failed to initialize internal webpage.");
                        exit(EXIT_FAILURE);
                    }

                    if(!webpage_fetch(page)) {
                        printf("Error! Failed to fetch html from internal page.");
                        free(url);
                        webpage_delete(page);
                        continue;
                    }
                    qput(qp, page);
                    hput(hp,url,url,strlen(url));
                    status = pagesave(page, id++, dirname);
                    if (status!=0){
                        exit(EXIT_FAILURE);
                    }
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
        webpage_delete(curr);
    }

    hclose(hp);
    qclose(qp);
    exit(EXIT_SUCCESS);
}

/*
* 0 - 1
* 1 - 7
* 2 - 42
* 3 - 82
*/
