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

/**
 * @brief represents a ranked doc with id, rank and url
*/
typedef struct doc {
    int id;
    int rank;
    char *url;
} rankedDoc_t;

/*************************** PROTOTYPES ********************************/
/**
 * Initializes a ranked document
 * 
 * @param id the doc id obtained from the index
 * @param rank the rank of the doc as evaluated from the query
 * @return a pointer to the initialized doc
*/
static rankedDoc_t* init_doc(int id, int rank);

/**
 * get user input from standard in
 * 
 * @param buffer buffer for the input
 * @return -1 if end of file(EOF), 0 if successful
*/
static int get_input(char *buffer);

/**
 * split the query into an array of tokens
 * 
 * @param query the raw query obtained from stdin
 * @param num_tokens the number of tokens in the raw query
 * @return an array of string tokens
*/
static char** tokenize_query(char *query, int *num_tokens);

/**
 * Normalizes a word (from the query) by converting to lowercase
 * 
 * @param word the query token (string)
 * @return a boolean indicating whether the word is valid (alphabetic)
*/
static bool NormalizeWord(char *word);

/**
 * validates query for invalid syntax
 * 
 * @param query an array containing the words in the query
 * @param num_tokens the number of tokens in the query
 * @return a boolean indicating whether the query is valid
*/
static bool validate_query(char **query, int num_tokens);

/**
 * searches for a query token in the index
 * 
 * @param elementp a pointer to an element in the index (elementp contains a word and wordcount)
 * @param key the token to search for
 * @return a boolean indicating whether the element's word is the same as the search token
 */
static bool token_searchfn(void *elementp, const void *key);

/**
 * searches for doc in the queue of documents
 * 
 * @related token_searchfn
*/
static bool doc_searchfn(void *elementp, const void *id);

/**
 * sets the ranked page url
 * 
 * @param id the crawled page id
 * @param pagedir the directory containing crawled pages
 * @param url_buffer the pointer to the buffer to write the url into
*/
static void set_url(int id, char *pagedir, char **url_buffer);

/**
 * finds the intersection between the ranked queue and token's queue out of place
 * 
 * @param qp1 the ranked queue
 * @param qp2 the queue of docs containing a token
 * @return a pointer to a queue containing the intersetion
*/
static queue_t* get_intersection(queue_t *qp1, queue_t *qp2);

/**
 * finds the union between the ranked queue and token's queue in place
 * ie. the result is concatenated into qp1, qp2 is restored to original state
 * 
 * @param qp1 the ranked queue
 * @param qp2 the queue of docs containing a token
 * @param pagedir directory of crawled pages. Used to get metadata ie url etc.
 * @return a pointer to a queue containing the union
*/
static queue_t* get_union(queue_t *qp1, queue_t *qp2, char *pagedir);

/*************************** MAIN ******************************/
int main(void){

    char *index_file = "index", *pagedir = "../pages";
    const char *and = "and", *or = "or";
    hashtable_t *index = indexload(index_file);

    char query[MAX_QUERY_LEN];
    char **tokenized_query, *token;
    int num_tokens;
    entry_t *ep;
    rankedDoc_t *doc;
    queue_t *ranked_docs;


    while(1){
        ranked_docs = qopen();
		num_tokens = 0;

        /* get input from stdin */
        if(get_input(query) != 0){
            break;
        }

        /* no query is entered */
        if(!query[0]){
            continue;
        }

        /* get array of tokens from query */
        tokenized_query = tokenize_query(query, &num_tokens);

        /* validate query */
        if(!tokenized_query || !validate_query(tokenized_query,num_tokens)){
            printf("[invalid query]\n");
            query[0] = '\0';
            continue;
        }

        /* process each token */
        for (int i = 0; i < num_tokens; i++) {
            token = tokenized_query[i];
            if(strcmp(token,and)==0 || strcmp(tokenized_query[i],or)==0)
                continue;

            ep = hsearch(index, token_searchfn, token, strlen(token));
            if(ep){
                if(i==0){
                    /* first iteration - add all docs */
                    ranked_docs = get_union(ranked_docs, ep->documents, pagedir);
                } else {
                    /* find intersection between current ranked and token's queue */
                    ranked_docs = get_intersection(ranked_docs, ep->documents);
                }
            }
            else{ // token not in index
                qclose(ranked_docs);
                ranked_docs = NULL;
            }
        }

        /* print docs' rank & url */
        while((doc=qget(ranked_docs))){
            printf("rank:%d doc:%d : %s\n",doc->rank,doc->id,doc->url);
        }

        qclose(ranked_docs);
    }

    exit(EXIT_SUCCESS);
}
/****************************************************************************/

/******************************** FUNCTIONS *********************************/
static int get_input(char *buffer){
    if(!buffer)
        return 1;
    buffer[0] = '\0';
    printf("> ");
    if(scanf("%[^\n]", buffer) == EOF){
        printf("\n");
        return -1;
    }
    getchar(); // strip newline character
    return 0;
}

static char** tokenize_query(char *query, int *num_tokens){
    char **tokenized_query = NULL;
    char* token = strtok(query, " \t");
    int count = 0;
    while(token){
        if(!NormalizeWord(token))
            return NULL;
        if(strlen(token) < 3 && strcmp(token,"or")!=0){
            token = strtok(NULL, " \t");
            continue;
        }
        tokenized_query = realloc(tokenized_query, (count+1) * sizeof(char*));
        tokenized_query[count] = malloc(strlen(token) + 1);
        strcpy(tokenized_query[count], token);
        token = strtok(NULL, " \t");
        count++;
    }
    *num_tokens = count;
    return tokenized_query;
}

static rankedDoc_t* init_doc(int id, int rank){
    rankedDoc_t *doc;
    if (!(doc=(rankedDoc_t*)malloc(sizeof(rankedDoc_t)))) {
        printf("Error in allocating memory\n");
        return NULL;
    }
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

static bool validate_query(char **query, int num_tokens){
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

static queue_t* get_intersection(queue_t *qp1, queue_t *qp2){
    queue_t *intersect = qopen();
    if(!qp1 || !qp2 || !intersect)
        return NULL;
    rankedDoc_t *doc; document_t *tmp;
    int word_count, rank;
    while((doc = qget(qp1))){
        if ((tmp=qsearch(qp2, doc_searchfn, &(doc->id)))) {
            // Add doc to new queue
            word_count = tmp->word_count;
            rank = doc->rank;
            doc->rank = word_count < rank ? word_count : rank;
            qput(intersect, doc);
        } 
    }
    return intersect;
}

static queue_t* get_union(queue_t *qp1, queue_t *qp2, char *pagedir){
    queue_t *tmp = qopen();
    if(!qp1 || !qp2 || !tmp)
        return NULL;
    document_t *dp;
    rankedDoc_t *doc;
    int word_count, rank;
    while((dp = qget(qp2))){
        word_count = dp->word_count;
        if ((doc=qsearch(qp1, doc_searchfn, &(dp->id)))) {
            rank = doc->rank;
            doc->rank = rank + word_count;
        } else{
            doc = init_doc(dp->id,word_count);
            set_url(dp->id, pagedir, &(doc->url));
            qput(qp1, doc);
        }
        qput(tmp, dp);
    }
    // restore qp2
    while((dp = qget(tmp)))
        qput(qp2, dp);
    qclose(tmp);

    return qp1;
}

static void set_url(int id, char *pagedir, char **url_buffer){
    webpage_t *page;
    char *url;
    if((page = pageload(id, pagedir))){
        url = webpage_getURL(page);
        if((*url_buffer = malloc(strlen(url) + 1))){
            strcpy(*url_buffer, url);
        }
        webpage_delete(page);
    }
}

static bool token_searchfn(void *elementp, const void *key){
    entry_t *ep = (entry_t*)elementp;
    return strcmp(ep->word,(char*)key)==0;
}

static bool doc_searchfn(void *elementp, const void *id){
    rankedDoc_t *dp = (rankedDoc_t*)elementp;
    return dp->id == *((int*)id);
}
