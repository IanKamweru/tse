/* 
 * pageio.c -- saving and loading crawler webpage files util
 *
 * Author: Ian Kamweru, Abdibaset, Nathaniel Mensah
 * Version: 1.0
 * 
 * Description:
 */

#include "pageio.h"
#include "webpage.h"

/*
 * pagesave -- save the page in filename id in directory dirnm
 *
 * returns: 0 for success; nonzero otherwise
 *
 * The suggested format for the file is:
 *   <url>
 *   <depth>
 *   <html-length>
 *   <html>
 */
int32_t pagesave(webpage_t *pagep, int id, char *dirnm){
    if(!pagep || !dirnm){
        return -1;
    }

    char *url = webpage_getURL(pagep);
    int depth = webpage_getDepth(pagep);
    int len = webpage_getHTMLlen(pagep);
    char *html_content = webpage_getHTML(pagep);

    /* generate path */
    char path[1024];
    sprintf(path, "%s/%d",dirnm, id);
    
    /* open file */
    FILE *file = fopen(path, "w");
    if (file == NULL || access(path, W_OK) != 0){
        printf("Failed to create file for url: %s\n",url);
        return 1;
    }
    
    /* write into the file */
    fprintf(file, "%s\n%d\n%d\n%s", url, depth, len, html_content);

    fclose(file);
    return 0;
}

/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm){
    return NULL;
}
