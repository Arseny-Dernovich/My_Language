#include "Read.h"



Token* parse_expression(Token* tokens, int count, int* index);

Token* create_token(enum Token_Type type, int value, const char* name) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->token_type = type;
    token->value = value;
    token->name = name ? strdup(name) : NULL;
    token->left = NULL;
    token->right = NULL;
    return token;
}

// Функция для проверки конца токенов
int is_end_of_tokens(Token* tokens, int count, int index) {
    return index >= count;
}

// Проверка текущего токена на определённый оператор
int match_operator(Token* tokens, int count, int* index, enum Operators op) {
    if (!is_end_of_tokens(tokens, count, *index) &&
        tokens[*index].token_type == TOKEN_OPERATOR &&
        strcmp(tokens[*index].name, words_operators[op].name) == 0) {
        (*index)++;
        return 1;
    }
    return 0;
}

// Primary := 'stadium_open' Expression 'stadium_close' | Id | Value
Token* parse_primary(Token* tokens, int count, int* index) {
    if (is_end_of_tokens(tokens, count, *index)) {
        return NULL;
    }

    Token* node = NULL;
    printf("Current token: %s\n", tokens[*index].name);

    // Если 'stadium_open'
    if (match_operator(tokens, count, index, OP_LSTADIUM)) {
        node = parse_expression(tokens, count, index);
        if (!match_operator(tokens, count, index, OP_RSTADIUM)) {
            fprintf(stderr, "Error: Expected 'stadium_close'\n");
            exit(EXIT_FAILURE);
        }
        return node;
    }

    // Если Value
    if (tokens[*index].token_type == TOKEN_NUMBER) {
        node = create_token(tokens[*index].token_type, tokens[*index].value, NULL);
        (*index)++;
        return node;
    }

    // Если Id
    if (tokens[*index].token_type == TOKEN_IDENTIFIER) {
        node = create_token(tokens[*index].token_type, 0, tokens[*index].name);
        (*index)++;
        return node;
    }

    fprintf(stderr, "Error: Unexpected token at primary\n");
    exit(EXIT_FAILURE);
}

// Term := Primary { [* /] Primary }*
Token* parse_term(Token* tokens, int count, int* index) {
    Token* left = parse_primary(tokens, count, index);
    if (!left) return NULL;

    while (!is_end_of_tokens(tokens, count, *index)) {
        enum Operators op;
        if (match_operator(tokens, count, index, OP_ASSIST)) {
            op = OP_ASSIST;
        } else if (match_operator(tokens, count, index, OP_SUBSTITUTE)) {
            op = OP_SUBSTITUTE;
        } else {
            break;
        }

        Token* right = parse_primary(tokens, count, index);
        if (!right) {
            fprintf(stderr, "Error: Missing operand after operator\n");
            exit(EXIT_FAILURE);
        }

        Token* new_node = create_token(TOKEN_OPERATOR, 0, words_operators[op].name);
        new_node->left = left;
        new_node->right = right;
        left = new_node;
    }

    return left;
}

// Expression := Term { [+ -] Term }*
Token* parse_expression(Token* tokens, int count, int* index) {
    Token* left = parse_term(tokens, count, index);
    if (!left) return NULL;

    while (!is_end_of_tokens(tokens, count, *index)) {
        enum Operators op;
        if (match_operator(tokens, count, index, OP_GOAL)) {
            op = OP_GOAL;
        } else if (match_operator(tokens, count, index, OP_FOUL)) {
            op = OP_FOUL;
        } else {
            break;
        }

        Token* right = parse_term(tokens, count, index);
        if (!right) {
            fprintf(stderr, "Error: Missing operand after operator\n");
            exit(EXIT_FAILURE);
        }

        Token* new_node = create_token(TOKEN_OPERATOR, 0, words_operators[op].name);
        new_node->left = left;
        new_node->right = right;
        left = new_node;
    }

    return left;
}

Token* Parse_Assign (Token* tokens , int count , int* index)
{
    Token* name_id = NULL;

    if (tokens[*index].token_type == TOKEN_IDENTIFIER) {

        name_id = &tokens[*index];
        (*index)++;
    }

    else {

        fprintf(stderr , "Error: Ожидалось имя переменной\n");
        exit(EXIT_FAILURE);
    }

    Operators op = OP_NAN;
    if (match_operator (tokens , count , index , OP_SCORE)) {
        op = OP_SCORE;
    }

    else {

        fprintf (stderr , "Error: Ожидалось присваивание");
        exit (EXIT_FAILURE);
    }

    Token* r_value = parse_expression (tokens , count , index);
    if (r_value == NULL) {
            fprintf(stderr, "Error: Missing operand after operator\n");
            exit(EXIT_FAILURE);
        }
    Token* new_token = create_token (TOKEN_OPERATOR , 0 , words_operators[op].name);
    new_token->left = name_id;
    new_token->right = r_value;

    return new_token;
}



// int main() {
//     // Пример массива токенов: 3 + 4 * (5 - 2)
//     Token tokens[] = {
//         {TOKEN_NUMBER, 3, NULL},
//         {TOKEN_OPERATOR, 0, "goal"},
//         {TOKEN_NUMBER, 4, NULL},
//         {TOKEN_OPERATOR, 0, "assist"},
//         {TOKEN_OPERATOR, 0, "stadium_open"},
//         {TOKEN_NUMBER, 5, NULL},
//         {TOKEN_OPERATOR, 0, "foul"},
//         {TOKEN_NUMBER, 2, NULL},
//         {TOKEN_OPERATOR, 0, "stadium_close"}
//     };
//
//     int index = 0;
//     Token* tree = parse_expression(tokens, sizeof(tokens) / sizeof(tokens[0]), &index);
//     print_tree(tree, 0);
//     export_tree_to_dot("tree.dot", tree);
//
//     return 0;
// }
