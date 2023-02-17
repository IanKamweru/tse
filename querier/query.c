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

#define MAX_QUERY_LEN 512
#define MAX_TOKENS 100

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

int main(void){

    char* index_file = "index";
    hashtable_t *index;

    char query[MAX_TOKENS][MAX_QUERY_LEN];
    char input[MAX_QUERY_LEN], *token;
    int num_tokens, count, rank;
    entry_t *ep;
    document_t *dp;
    bool valid_token;

    while(1){
        index = indexload(index_file);
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
                    dp = qget(ep->documents);
                    count = dp->word_count;
                }
                rank = count < rank || rank < 0 ? count : rank;
				printf("%s:%d ", query[i], count);
            }
            memset(query, 0, sizeof(query));
			rank = rank == -1 ? 0 : rank;
            printf("-- %d\n", rank);
        }

        input[0] = '\0';
        free_entries(index);
	    hclose(index);
    }

    exit(EXIT_SUCCESS);
}
