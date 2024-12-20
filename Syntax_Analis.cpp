#include "Read.h"



#define type(index)    tokens[index].token_type
#define value(index)   tokens[index].value

void Add_Local_Identifier (Token* Tokens , int index , Label_Id* label_first_label_id , Label_Id* local_label_id)
{

}

void Build_func_Table_Name (Token* tokens , Token* block , int index , Label_Id* label_id)
{
    if (type(index) != TOKEN_KEY_WORD && value(index) != OP_GOAT) {

        fprintf (stderr , "При созданни таблицы имён для функции в начале ожилася узел с инициализацией\n");
        assert(0);
    }




}

Token* Create_Token (enum Token_Type type , int value)
{
    Token* token =  (Token*)malloc (sizeof (Token));
    token->token_type = type;
    token->value = value;
    token->local_label_id = {};
    token->left = NULL;
    token->right = NULL;
    return token;
}

int Is_End_Of_Tokens (Token* tokens , int count , int index)
{
    return index >= count;
}

int Match_Operator (Token* tokens , int count , int* index , enum Operators op)
{
    if (!Is_End_Of_Tokens (tokens , count , *index) &&
        type(*index) == TOKEN_OPERATOR &&
        strcmp (words_operators[value(*index)].name , words_operators[op].name) == 0) {

        (*index)++;
        return 1;
    }
    return 0;
}

// Primary := 'stadium_open' Expression 'stadium_close' | Id | Value
Token* Parse_Primary (Token* tokens , int count , int* index)
{
    if (Is_End_Of_Tokens (tokens , count , *index)) {
        return NULL;
    }

    Token* node = NULL;

    // printf ("HUUUIII value = %d" , value(*index));
    if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_LSTADIUM) {

        node = Parse_Expression (tokens , count , index);
        if (!Match_Operator (tokens , count , index , OP_RSTADIUM)) {

            fprintf (stderr , "Error: Ожидалось 'stadium_close'\n");
            assert (0);
        }
        return node;
    }

    if (type(*index) == TOKEN_NUMBER) {
        return &tokens[(*index)++];
    }

    if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_CALL_FUNC) {

        Token* call_func_node = Parse_CallFunc (tokens , count , index);
        return call_func_node;
    }


    if (type(*index) == TOKEN_IDENTIFIER) {
        return &tokens[(*index)++];
    }

    fprintf (stderr , "Error: Ожидалось либо число , либо имя переменной , либо выражение в скобках\n");
    assert (0);
}

// Term := Primary { [* /] Primary }*
Token* Parse_Term (Token* tokens , int count , int* index)
{
    Token* left = Parse_Primary (tokens , count , index);
    if (!left) return NULL;

    while  (!Is_End_Of_Tokens (tokens , count , *index)) {

        enum Operators op;
        if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_ASSIST) {
            op = OP_ASSIST;
            (*index)++;

        } else if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_SUBSTITUTE) {
            op = OP_SUBSTITUTE;
            (*index)++;

        } else {
            break;
        }

        Token* right = Parse_Primary (tokens , count , index);
        if (!right) {
            fprintf (stderr , "Error: Missing operand after operator\n");
            assert (0);
        }

        Token* new_node = Create_Token (TOKEN_OPERATOR , op);
        new_node->left = left;
        new_node->right = right;
        left = new_node;
    }

    return left;
}

// Expression := Term { [+ -] Term }*
Token*Parse_Expression (Token* tokens , int count , int* index)
{
    Token* left = Parse_Term (tokens , count , index);
    if (!left) return NULL;

    while  (!Is_End_Of_Tokens (tokens , count , *index)) {

        enum Operators op;
        if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_GOAL) {

            op = OP_GOAL;
            (*index)++;

        } else if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_FOUL) {

            op = OP_FOUL;
            (*index)++;

        } else {
            break;
        }



        Token* right = Parse_Term (tokens , count , index);
        if (!right) {
            fprintf (stderr , "Error: Missing operand after operator\n");
            assert (0);
        }

        Token* new_node = Create_Token (TOKEN_OPERATOR , op);
        new_node->left = left;
        new_node->right = right;
        left = new_node;
    }

    return left;
}

Token* Parse_Assign  (Token* tokens , int count , int* index)
{
    Token* name_id = NULL;

    if (type(*index) == TOKEN_IDENTIFIER) {

        name_id = &tokens[*index];
        (*index)++;

    } else {

        fprintf (stderr , "Error: Ожидалось имя переменной\n");
        assert (0);
    }


   if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_SCORE) {

        fprintf (stderr , "Error: Ожидалось 'stadium_open'\n");
        assert (0);
    }
    Token* score_node = &tokens[*index];
    (*index)++;

    Token* r_value = Parse_Expression  (tokens , count , index);
    if (r_value == NULL) {

            fprintf (stderr , "Error: Missing operand after operator\n");
            assert (0);
        }

    score_node->left = name_id;
    score_node->right = r_value;

    return score_node;
}


Token* Parse_Condition (Token* tokens , int count , int* index)
{
    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_LSTADIUM) {

        fprintf (stderr , "Error: Ожидалось 'stadium_open'\n");
        assert (0);
    }
    (*index)++;

    Token* left_expr = Parse_Expression (tokens , count , index);
    if (left_expr == NULL) {

        fprintf (stderr , "Error: Ожидалось выражение после 'stadium_open'\n");
        assert (0);
    }

    if (type(*index) != TOKEN_OPERATOR ||
         (value(*index) < OP_LEADER || value(*index) > OP_GAP)) {

        fprintf (stderr , "Error: Ожидался оператор сравнения\n");
        assert (0);
    }
    Token* comparison_operator = &tokens[*index];
    (*index)++;

    Token* right_expr = Parse_Expression (tokens , count , index);
    if (right_expr == NULL) {

        fprintf (stderr , "Error: Ожидалось выражение после оператора сравнения\n");
        assert (0);
    }


    comparison_operator->left = left_expr;
    comparison_operator->right = right_expr;

    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_RSTADIUM) {

        fprintf (stderr , "Error: Ожидалось 'stadium_close'\n");
        assert (0);
    }
    (*index)++;

    return comparison_operator;
}

Token* Parse_Print  (Token* tokens , int count , int* index)
{
     if (type(*index) != TOKEN_KEY_WORD || value(*index) != KW_PRINT) {

        fprintf (stderr , "Error: Ожидалось ключевое слово 'GOAT'\n");
        assert (0);
    }

    Token* print_node = &tokens[*index];
    (*index)++;

    Token* print_expr = Parse_Expression  (tokens , count , index);

    print_node->left  = print_expr;

    return print_node;

}

Token* Parse_Init (Token* tokens , int count , int* index)
{
    if (type(*index) != TOKEN_KEY_WORD || value(*index) != OP_GOAT) {

        fprintf (stderr , "Error: Ожидалось ключевое слово 'GOAT'\n");
        assert (0);
    }
    (*index)++;

    if (type(*index) != TOKEN_IDENTIFIER) {

        fprintf (stderr , "Error: Ожидался идентификатор после 'GOAT'\n");
        assert (0);
    }
    Token* variable_name = &tokens[*index];
    (*index)++;

    Token* init_node = Create_Token (TOKEN_KEY_WORD , OP_GOAT);
    init_node->left  = variable_name;
    init_node->right = NULL;

    if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_SCORE) {
        (*index)++;

        Token* value_expr = Parse_Expression (tokens , count , index);
        if (value_expr == NULL) {

            fprintf (stderr , "Error: Ожидалось выражение после '='\n");
            assert (0);
        }
        init_node->right = value_expr;
    }


    return init_node;
}

Token* Parse_Op (Token* tokens , int count , int* index)
{
    Token* op_node = NULL;

    if (type(*index) == TOKEN_KEY_WORD && value(*index) == OP_GOAT) {
        op_node = Parse_Init (tokens , count , index);

    } else if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_L_FIGURE_STADIUM) {
        op_node = Parse_ConditionOp (tokens , count , index); // Парсим вложенное тело

    } else if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_PRINT) {
        op_node = Parse_Print (tokens , count , index); // Парсим вложенное тело

    } else if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_RETURN) {
        op_node = Parse_Ret_Func  (tokens , count , index);

    } else if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_CALL_FUNC) {
        op_node = Parse_CallFunc  (tokens , count , index);

    } else if (type(*index) == TOKEN_IDENTIFIER) {
        op_node = Parse_Assign (tokens , count , index);

    } else {

        fprintf (stderr , "Error: Ожидался Init или Assign , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }


    if (type (*index - 1) != TOKEN_OPERATOR || value (*index - 1) != OP_RSTADIUM) {

        if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_WHISTLE) {
            // Если `whistle` отсутствует в верхнем уровне , генерируем ошибку
            fprintf (stderr , "Error: Ожидалось 'whistle' после операции %d nameop = %s\n" ,
                    value(*index) , words_operators[value(*index)].name);
            assert (0);
        }
        (*index)++;

    } else {
        // Если вложенная конструкция , автоматически добавляем `whistle` в дерево
        Token* auto_whistle_node = Create_Token (TOKEN_OPERATOR , OP_WHISTLE);
        auto_whistle_node->left = op_node;
        auto_whistle_node->right = NULL;
        return auto_whistle_node;
    }


    Token* whistle_node = Create_Token (TOKEN_OPERATOR , OP_WHISTLE);
    whistle_node->left = op_node;
    whistle_node->right = NULL;

    return whistle_node;
}

Token* Parse_Multiple_Op (Token* tokens , int count , int* index)
{
    Token* root = NULL;
    Token* current = NULL;

    while  (!Is_End_Of_Tokens (tokens , count , *index)) {

        if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_END_BODY) {
        // printf ("@@Token %d: type = %d , value = %d\n count= %d" , *index , type(*index) , value(*index) , count);
            break;  // Завершаем парсинг операций
        }

        if (type(*index) == TOKEN_OPERATOR && value(*index) == OP_R_FIGURE_STADIUM) {
            break;  // Завершаем парсинг операций
        }

        if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_START_BODY) {
            return root;
        }

        // Парсим операцию Op
        Token* op_node = Parse_Op (tokens , count , index);
        if (!op_node) {
            break;
        }

        // Формируем цепочку операций
        if (!root) {
            root = op_node;

        } else {
            current->right = op_node;
        }
        current = op_node;
    }

    if (!root) {
        fprintf (stderr , "Error: Ожидалась хотя бы одна операция в MultipleOp\n");
        assert (0);
    }

    return root;
}
Token* Parse_Body (Token* tokens , int count , int* index)
{
    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_L_FIGURE_STADIUM) {

        fprintf (stderr , "Error: Ожидалось ключевое слово 'start_game'\n");
        assert (0);
    }
    (*index)++;

    // Парсим цепочку операций MultipleOp
    Token* multiple_op = Parse_Multiple_Op (tokens , count , index);


    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_R_FIGURE_STADIUM) {

        fprintf (stderr , "Error: Ожидалось ключевое слово 'end_game'\n");
        assert (0);
    }
    (*index)++;


    return multiple_op;
}


Token* Parse_ConditionOp (Token* tokens , int count , int* index)
{

    Token* body = Parse_Body (tokens , count , index);
    if (!body) {

        fprintf (stderr , "Error: Ожидалось тело цикла или условия\n");
        assert (0);
    }


    if (type(*index) != TOKEN_KEY_WORD ||
         (value(*index) != KW_PASS && value(*index) != KW_DRIBBLE)) {

        fprintf (stderr , "Error: Ожидалось ключевое слово 'pass' или 'dribble'\n");
        assert (0);
    }


    Token* condition_op = &tokens[*index];
    (*index)++;


    Token* condition = Parse_Condition (tokens , count , index);
    if (!condition) {

        fprintf (stderr , "Error: Ожидалось условие после 'pass' или 'dribble'\n");
        assert (0);
    }

    // Создаем узел для ConditionOp
    condition_op->left = condition;
    condition_op->right = body;

    return condition_op;
}


Token* Parse_Ret_Func  (Token* tokens , int count , int* index)
{
    if (type(*index) != TOKEN_KEY_WORD || value(*index) != KW_RETURN) {

        fprintf  (stderr , "Error: Ожидалось ключевое слово возврата значения функции 'SSUUII'");
        assert (0);
    }

    Token* ret_node = &tokens[*index];
    (*index)++;

    Token* ret_value = Parse_Expression  (tokens , count , index);

    Token* ret_func = Create_Token  (TOKEN_KEY_WORD , ret_node->value);
    ret_func->left = ret_value;

    return ret_func;
}


Token* Parse_CallFunc (Token* tokens , int count , int* index)
{

    if (type(*index) != TOKEN_KEY_WORD || value(*index) != KW_CALL_FUNC) {

        fprintf (stderr , "Error: Ожидалось 'transfer' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;


    if (type(*index) != TOKEN_FUNCTION) {

        fprintf (stderr , "Error: Ожидалось имя функции после 'transfer' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    Token* func_name = &tokens[*index];
    (*index)++;


    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_LSTADIUM) {

        fprintf (stderr , "Error: Ожидалось 'stadium_open' после имени функции , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;

    // Создаём корневой узел вызова функции
    Token* call_node = Create_Token (TOKEN_KEY_WORD , KW_CALL_FUNC);
    call_node->left = func_name;
    call_node->right = NULL;


    Token* args_head = NULL;
    Token* last_arg = NULL;

    while  (type(*index) == TOKEN_OPERATOR && value(*index) == OP_OFFSIDE) {

        (*index)++;

        Token* expression = Parse_Expression (tokens , count , index);
        if (!expression) {

            fprintf (stderr , "Error: Ожидалось выражение после 'offside'\n");
            assert (0);
        }



        Token* offside_node = Create_Token (TOKEN_OPERATOR , OP_OFFSIDE);
        offside_node->left  = expression;
        offside_node->right = NULL;


        if (!args_head) {
            args_head = offside_node;

        } else {
            last_arg->right = offside_node;
        }
        last_arg = offside_node;
    }


    call_node->right = args_head;


    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_RSTADIUM) {

        fprintf (stderr , "Error: Ожидалось 'stadium_close' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;

    return call_node;
}

Token* Parse_Body_Function  (Token* tokens , int count , int* index)
{
    if (type(*index) != TOKEN_KEY_WORD || value(*index) != KW_START_BODY) {

        fprintf (stderr , "Error: Ожидалось '/:first_half' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;

    Token* body_func = Parse_Multiple_Op  (tokens , count , index);


    if (type(*index) != TOKEN_KEY_WORD || value(*index) != KW_END_BODY) {

        fprintf (stderr , "Error: Ожидалось '/:second_half' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;

    return body_func;
}

Token* Parse_Function  (Token* tokens , int count , int* index)
{
    Token* body_func = Parse_Body_Function (tokens , count , index);
    if (!body_func) {
        fprintf (stderr , "Error: Ожидалось тело функции\n");
        assert (0);
    }

    Token* new_block_function  = Create_Token (TOKEN_NEW_BLOCK , NEW_BLOCK);
    printf ("\n&&&&&&&&&&&&&&&&&Token %d: type = %d , value = %d\n count= %d" , *index , type(*index) , value(*index) , count);
    new_block_function->right = body_func;
    // Убедимся , что следующий токен — это 'GOAT'
    if (type(*index) != TOKEN_KEY_WORD || value(*index) != OP_GOAT) {
        fprintf (stderr , "Error: Ожидалось ключевое слово 'GOAT' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    Token* goat_token = &tokens[*index];
    (*index)++; // Пропускаем 'GOAT'

    // Убедимся , что следующий токен — это идентификатор  (имя функции)
    if (type(*index) != TOKEN_FUNCTION) {
        fprintf (stderr , "Error: Ожидалось имя функции после 'GOAT' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    Token* func_name = &tokens[*index];
    (*index)++; // Пропускаем имя функции

    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_LSTADIUM) {
        fprintf (stderr , "Error: Ожидалось 'stadium_open' после имени функции , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;

    Token* func_node = Create_Token (TOKEN_KEY_WORD , KW_INIT_FUNC);
    func_node->left = func_name;
    func_node->right = NULL;

    Token* args_head = NULL;
    Token* last_arg = NULL;

    while  (type(*index) == TOKEN_KEY_WORD && value(*index) == OP_GOAT) {

        (*index)++;

        Token* name_arg = &tokens[*index];
        if (name_arg->token_type != TOKEN_IDENTIFIER) {
            fprintf (stderr , "Error: Ожидалось имя аргумента  после 'GOAT'\n");
            assert (0);
        }
        (*index)++;



        Token* offside_node = Create_Token (TOKEN_KEY_WORD , OP_GOAT);
        offside_node->left = name_arg;
        offside_node->right = NULL;


        if (!args_head) {
            args_head = offside_node;

        } else {
            last_arg->right = offside_node;
        }

        last_arg = offside_node;
    }

    if (type(*index) != TOKEN_OPERATOR || value(*index) != OP_RSTADIUM) {
        fprintf (stderr , "Error: Ожидалось 'stadium_close' , но получили токен типа %d с значением %d\n" ,
                type(*index) , value(*index));
        assert (0);
    }
    (*index)++;

    func_node->right  = args_head;

    goat_token->right = new_block_function;
    goat_token->left  = func_node;
    // printf  ("\n\n@@HUUUUIIIIIII\n\n");

    return goat_token;
}

Token* Parse_Programm(Token* tokens, int count, int* index)
{
    // Создаем корень программы
    Token* root_programm = Create_Token(TOKEN_KEY_WORD, FORSA_BARSA);


    // Узел для списка функций
    Token* functions_root = NULL;
    Token* last_function_node = NULL;

    // Узел для списка глобальных переменных
    Token* global_vars_root = NULL;
    Token* last_global_var_node = NULL;

    while (*index < count) {

        if (type(*index) == TOKEN_KEY_WORD && value(*index) == KW_START_BODY) {
            printf ("\n$$$\n");
            // Парсинг функции
            Token* function_node = Parse_Function (tokens, count, index);
            if (!function_node) {

                fprintf(stderr, "Error: Ожидалась функция, но парсинг не удался.\n");
                assert(0);
            }

            // Добавление функции в цепочку
            Token* function_list_node = Create_Token(TOKEN_KEY_WORD, KW_INIT_FUNC);
            function_list_node->left = function_node;
            function_list_node->right = NULL;

            if (!functions_root) {
                functions_root = function_list_node;

            } else {
                last_function_node->right = function_list_node;
            }

            last_function_node = function_list_node;

        } else if (type(*index) == TOKEN_KEY_WORD && value(*index) == OP_GOAT) {

            Token* global_node = Parse_Multiple_Op (tokens , count , index);
            global_vars_root = global_node;

        }
    }

    // Связываем поддеревья с корнем программы
    root_programm->left = functions_root;
    root_programm->right = global_vars_root;
    // Export_Tree_To_Dot ("tree.dot" , label_id ,  functions_root);

    return root_programm;
}

