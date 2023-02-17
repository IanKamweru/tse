/* query.c --- reads query from user, consults an index and ranks crawled pages
 * 
 * Authors: Abdibaset Bare, Ian Kamweru and Nathaniel Mensah
 * Created: Wed Feb 15 21:57:45 2023 (-0500)
 * Version: 1.0
 * 
 * Description: program that reads a query from the user, consults the index built by the indexer, 
 * ranks documents fetched by the crawler according to their relevance, 
 * and prints a list of documents in rank order. In general, queries are a sequence of words
 * separated by spaces with optiona  boolean operators AND and OR, where AND has precedence over OR.
 * By default, all words typed in a query are implicitly connected by logical-AND. 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <hash.h>
#include <indexio.h>
#include <pageio.h>

#define MAX_QUERY_LEN 512
#define MAX_TOKENS 100

typedef struct doc {
    int id;
    int rank;
    char url[1024];
} doc_rank_t;

static doc_rank_t* init_doc(int id, int rank, char *url){
    doc_rank_t *doc;
    if (!url) {
        return NULL;
    }
    if (!(doc=(doc_rank_t*)malloc(sizeof(doc_rank_t)))) {
        printf("Error in allocating memory\n");
        return NULL;
    }
    strcpy(doc->url, url);
    doc->id = id;
    doc->rank = rank;
    return doc;
}

static bool NormalizeWord(char *word){
	if(!word)
		return false;
	int len = strlen(word);
	int i;
	for(i=0; i<len; i++){
		if(!isalpha(word[i])){
			return false;
		}
		word[i] = tolower(word[i]);
	}
    return true;
}

/* validates query for invalid syntax */
static bool validate_query(char query[][MAX_QUERY_LEN], int num_tokens){
    if(strcmp(query[0],"and") == 0 || strcmp(query[0],"or") == 0 ||
        strcmp(query[num_tokens-1],"and") == 0 ||
        strcmp(query[num_tokens-1],"or") == 0){
        return false;
    }
    for (int i = 0; i < num_tokens-1; i++) {
        if(strcmp(query[i], "and") == 0){
            if(strcmp(query[i+1], "and") == 0 || strcmp(query[i+1], "or") == 0)
                return false;  
        }
        else if(strcmp(query[i], "or") == 0){
            if(strcmp(query[i+1], "and") == 0 || strcmp(query[i+1], "or") == 0)
                return false;
        }
    }
    return true;
}

/* search for query token in index */
static bool token_searchfn(void *elementp, const void *key){
    entry_t *ep = (entry_t*)elementp;
    return strcmp(ep->word,(char*)key)==0;
}

/* search for doc in ranked docs queue */
static bool doc_searchfn(void *elementp, const void *id){
    doc_rank_t *dp = (doc_rank_t*)elementp;
    return dp->id == *((int*)id);
}

int main(void){

    char *index_file = "index";
    char *pagedir = "../pages";
    hashtable_t *index;
    queue_t *ranked_docs, *new_ranked_docs;
    webpage_t *page;

    char query[MAX_TOKENS][MAX_QUERY_LEN];
    char input[MAX_QUERY_LEN], *token;
    int num_tokens, count, rank;
    entry_t *ep;
    document_t *dp;
    doc_rank_t *doc;
    bool valid_token;

    while(1){
        index = indexload(index_file);
        ranked_docs = NULL;

        valid_token = true;
		num_tokens = 0, count = 0, rank = -1;
        printf("> ");
        if(scanf("%[^\n]", input) == EOF){
            printf("\n");
            break;
        }
        getchar(); // strip newline character

        /* query */
        if(input[0]){
            token = strtok(input, " ");
            while(token){
                if(NormalizeWord(token)){
                    strcpy(query[num_tokens++], token);
                }
                else{
                    valid_token = false;
                    break;
                }
                token = strtok(NULL, " ");
            }

            if(!valid_token || !validate_query(query,num_tokens)){
                printf("[invalid query]\n");
                input[0] = '\0';
                continue;
            }

            /* process each token */
            for (int i = 0; i < num_tokens; i++, count=0) {
                if(strlen(query[i]) < 3 || strcmp(query[i],"and")==0 || strcmp(query[i],"or")==0)
					continue;
                ep = hsearch(index, token_searchfn, query[i], strlen(query[i]));

                if(ep){
                    new_ranked_docs = qopen();
                    /* first iteration - add all docs */
                    if(!ranked_docs){
                        while((dp = qget(ep->documents))){
                            count = dp->word_count;
                            page = pageload(dp->id,pagedir);
                            doc = init_doc(dp->id,count,webpage_getURL(page));
                            qput(new_ranked_docs, doc);
                        }
                    } else {
                        // Add only documents containing current token from ranked_docs to new queue
                        document_t *tmp;
                        while ((doc = qget(ranked_docs))) {
                            if ((tmp=qsearch(ep->documents, doc_searchfn, &(doc->id)))) {
                                // Add doc to new queue
                                count = tmp->word_count;
                                rank = doc->rank;
                                doc->rank = count < rank ? count : rank;
                                qput(new_ranked_docs, doc);
                            } else {
                                // Doc in ranked queue doesn't contain current token
                            }
                        }
                    }
                    ranked_docs = new_ranked_docs; // Update ranked_docs with new queue
                } 
                else{ // token not in index
                    while((doc=qget(ranked_docs))){} // ???
                }
            }

            /* print docs' rank & url */
            while((doc=qget(ranked_docs))){
                printf("rank:%d doc:%d : %s\n",doc->rank,doc->id,doc->url);
            }
            memset(query, 0, sizeof(query));
        }
        input[0] = '\0';
        free_entries(index);
	    hclose(index);
        qclose(ranked_docs);
    }
    exit(EXIT_SUCCESS);
}
