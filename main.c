#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef enum {
    INT_KEYWORD,
    RETURN_KEYWORD,
    IDENTIFIER,
    L_PARAN,
    R_PARAN,
    L_BRACE,
    R_BRACE,
    INT_LITERAL,
    SEMICOLON,
    UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    Token* tokens;
    size_t size;
    size_t capacity;
} TokenList;

TokenList* create_token_list(size_t init_capacity);
void free_token_list(TokenList* list);
void print_tokens(TokenList *list);
Token* add_token(TokenList* list, TokenType type, const char* value);
TokenList* lex(const char* filename);


int main(int argc, char** argv) {
    if (argc < 2) {
        perror("ERROR: File not provided.");
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    TokenList* tokens = lex(filename);

    if (tokens) {
        print_tokens(tokens);
        free_token_list(tokens);
    } else {
        perror("ERROR: Lexing file.");
        exit(EXIT_FAILURE); 
    }

    return 0;
}

/**
* * LEXER
* First stage.
* Performs lexical analysis on file content and generates a list of tokens.
*/

/**
 * @brief Creates a new token list with a given initial capacity.
 * @param init_capacity The initial capacity of the token list.
 * @return A pointer to the newly created TokenList.
 */
TokenList* create_token_list(size_t init_capacity) {
    TokenList* list = malloc(sizeof(TokenList));

    list -> size = 0;
    list -> capacity = init_capacity;
    list -> tokens = malloc(sizeof(Token) * init_capacity);

    return list;
}

/**
 * @brief Frees the memory used by a token list.
 * @param list A pointer to the TokenList to be freed.
 */
void free_token_list(TokenList* list) {
    for (size_t i = 0; i < (list -> size); i++) {
        free(list -> tokens[i].value);
    }

    free(list -> tokens);
    free(list);
}

/**
 * @brief Prints the tokens in a token list.
 * @param list A pointer to the TokenList to be printed.
 */
void print_tokens(TokenList *list) {
    for (size_t i = 0; i < (list -> size); i++) {
        printf("Type: %d, Value: %s\n", list -> tokens[i].type, list -> tokens[i].value);
    }
}

/**
 * @brief Adds a token to the token list.
 * @param list A pointer to the TokenList to which the token will be added.
 * @param type The type of the token to be added.
 * @param value The value of the token to be added.
 * @return A pointer to the newly added Token.
 */
Token* add_token(TokenList* list, TokenType type, const char* value) {
    if ((list -> size) >= (list -> capacity)) {
        list -> capacity *= 2;
        list -> tokens = realloc(list -> tokens, sizeof(Token) * (list -> capacity));
    
        if (!list -> tokens) {
            perror("ERROR: Failed to reallocate token list.");
            exit(EXIT_FAILURE);
        }
    }


    Token* token = &(list -> tokens[list -> size++]);

    token -> type = type;
    token -> value = strdup(value);

    return token;
}

/**
 * @brief Lexes a source file into a list of tokens.
 * @param filename The name of the file to be lexed.
 * @return A pointer to the TokenList containing the lexed tokens.
 * @note Returns NULL if the file cannot be opened.
 */
TokenList* lex(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("ERROR: Failed to open file. File may not exist.");
        return NULL;
    }

    TokenList* tokens = create_token_list(10);

    char c;
    char buffer[256]; // Buffer for identifiers or keywords
    int buffer_index = 0;

    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) {
            continue;
        } else if (isalpha(c)) {
            buffer[buffer_index++] = c;

            while (isalnum(c = fgetc(file))) {
                buffer[buffer_index++] = c;
            }

            buffer[buffer_index] = '\0';
            //ungetc(c, file);

            if (strcmp(buffer, "int") == 0) {
                add_token(tokens, INT_KEYWORD, "int");
            } else if (strcmp(buffer, "return") == 0) {
                add_token(tokens, RETURN_KEYWORD, "return");
            } else {
                add_token(tokens, IDENTIFIER, buffer);
            }

            buffer_index = 0;
            for (size_t i = 0; i < sizeof(buffer); i++) {
                buffer[i] = '\0';
            }
        } else if (isdigit(c)) {
            buffer[buffer_index++] = c;

            while (isdigit(c = fgetc(file))) {
                buffer[buffer_index++] = c;
            }

            buffer[buffer_index] = '\0';
            ungetc(c, file);

            add_token(tokens, INT_LITERAL, buffer);
            buffer_index = 0;
            for (size_t i = 0; i < sizeof(buffer); i++) {
                buffer[i] = '\0';
            }
        } else {
            switch (c) {
                case '(':
                    add_token(tokens, L_PARAN, "(");
                    break;
                case ')':
                    add_token(tokens, R_PARAN, ")");
                    break;
                case '{':
                    add_token(tokens, L_BRACE, "{");
                    break;
                case '}':
                    add_token(tokens, R_BRACE, "}");
                    break;
                case ';':
                    add_token(tokens, SEMICOLON, ";");
                    break;
                default:
                    buffer[0] = c;
                    buffer[1] = '\0';
                    add_token(tokens, UNKNOWN, buffer);
                    break;
            }
        }
    }

    fclose(file);
    return tokens;
}

/**
* * LEXER END
*/