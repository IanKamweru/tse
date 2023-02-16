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

int main(void){

    char tokens[MAX_TOKENS][MAX_QUERY_LEN];
    char input[MAX_QUERY_LEN], *token;
    int num_tokens = 0;
    bool valid_query;
    
    while(1){
        valid_query = true;
        printf("> ");
        if(scanf("%[^\n]", input) == EOF){
            printf("\n");
            break;
        }
        getchar(); // strip newline character

        if(input[0]){
            token = strtok(input, " ");
            while(token){
                if(NormalizeWord(token)){
                    strcpy(tokens[num_tokens++], token);
                }
                else{
                    valid_query = false;
                    printf("[invalid query]\n");
                    input[0] = '\0';
                    break;
                }
                token = strtok(NULL, " ");
            }

            if(!valid_query){
                continue;
            }
            // Print query
            for (int i = 0; i < num_tokens; i++) {
                printf("%s ", tokens[i]);
            }

            num_tokens = 0;
            memset(tokens, 0, sizeof(tokens));
            printf("\n");
        }
        input[0] = '\0';
    }

    exit(EXIT_SUCCESS);
}
