#include "Read.h"
#include "Back_end.h"

int n_if = 1;
int n_while = 1;

void Generate_Asm_Assign (Token* node, FILE* asm_file , Label_Id* label_id)
{
    if (!node) return;

    assert(node->token_type == TOKEN_OPERATOR && node->value == OP_SCORE);

    // Обрабатываем выражение справа
    Generate_Asm_Expression(node->right, asm_file , label_id);


    // Слева должна быть переменная
    if (node->left->token_type == TOKEN_IDENTIFIER) {
        fprintf(asm_file, "pop [%d]  ; id =%s\n", node->left->value , label_id->table_label[node->left->value].name_identifier); // Присваиваем значение переменной
    } else {
        fprintf(stderr, "ERROR: Left side of assignment must be an identifier\n");
        assert(0);
    }
}


void Generate_Asm_Expression(Token* node, FILE* asm_file , Label_Id* label_id)
{
    if (!node) return;

    // Постфиксный обход дерева
    Generate_Asm_Expression(node->left, asm_file , label_id);
    Generate_Asm_Expression(node->right, asm_file , label_id);

    // Обрабатываем текущий узел
    if (node->token_type == TOKEN_IDENTIFIER) {
        // Узел - переменная
        fprintf(asm_file, "push [%d] ; id =%s\n", node->value , label_id->table_label[node->value].name_identifier); // Значение переменной - это индекс в таблице имен
    }
    else if (node->token_type == TOKEN_NUMBER) {
        // Узел - число
        fprintf(asm_file, "push %d\n", node->value);
    }
    else if (node->token_type == TOKEN_OPERATOR) {
        // Узел - оператор
        switch (node->value) {
            case OP_GOAL: // "+"
                fprintf(asm_file, "add\n");
                break;

            case OP_FOUL: // "-"
                fprintf(asm_file, "sub\n");
                break;

            case OP_ASSIST: // "*"
                fprintf(asm_file, "mul\n");
                break;

            case OP_SUBSTITUTE: // "/"
                fprintf(asm_file, "div\n");
                break;

            default:
                fprintf(stderr, "ERROR: Unknown operator: %d\n", node->value);
                assert(0);
        }
    }

    else if (node->token_type == TOKEN_KEY_WORD) {

        Generate_Asm_Call_Func (node , asm_file , label_id);
    }
}


void Generate_Asm_Initialization(Token* node, FILE* asm_file, Label_Id* label_id)
{
    assert(node);
    my_assert(node->value != OP_GOAT); // Узел типа данных (например, "int")

    // Правый потомок - значение инициализации
    Token* value_node = node->right;
    Generate_Asm_Expression(value_node, asm_file , label_id);

    // Левый потомок - имя переменной
    Token* identifier_node = node->left;
    assert(identifier_node->token_type == TOKEN_IDENTIFIER);

    // Записываем значение в переменную
    fprintf(asm_file, "pop [%d] ; Initialize variable %s\n", identifier_node->value, label_id->table_label[identifier_node->value].name_identifier);
}


void Generate_Asm_Operations(Token* node, FILE* asm_file , Label_Id* label_id)
{
    assert(node);

//      if (node->value == OP_WHISTLE) {
//
//         return;
//     }

    switch (node->value) {

        case OP_GOAT:
                        Generate_Asm_Initialization (node, asm_file , label_id);
                        break;

        case OP_SCORE:
                        Generate_Asm_Assign (node, asm_file , label_id);
                        break;

        case KW_PASS:
                        Generate_Asm_If_Statement (node , asm_file , label_id);
                        break;

        case KW_DRIBBLE:
                        Generate_Asm_While_Statement (node , asm_file , label_id);
                        break;

        case KW_CALL_FUNC:
                        Generate_Asm_Call_Func (node , asm_file , label_id);
                        break;

        case KW_PRINT:
                        Generate_Asm_Print (node , asm_file , label_id);
                        break;

        case KW_RETURN:
                        Generate_Asm_Return_Value (node , asm_file , label_id);
                        break;


        default:
                        fprintf(stderr, "Unknown token type: %d\n", node->value);
                        assert (0);
    }
}


void Generate_Asm_Multiple_Op(Token* node, FILE* asm_file , Label_Id* label_id)
{
    if (node == NULL) {
        fprintf (asm_file , "; Я в конце  Multiple_Op\n");
        return;
    }
    fprintf (asm_file , "; Я в Multiple_Op\n");
    fprintf(stderr, "\nstart multiple: %s\n", words_operators[node->value].name);

    if (node->value == OP_WHISTLE) { // Узел ";"

        printf ("Зашёл");
        Generate_Asm_Multiple_Op(node->left, asm_file , label_id);

        // Обрабатываем правую часть (следующее выражение)
        Generate_Asm_Multiple_Op(node->right, asm_file , label_id);

    } else {
        printf ("зашёл операция : %s\n", words_operators[node->value].name);
        // Если это не ";", обрабатываем как одно выражение
        Generate_Asm_Operations(node, asm_file , label_id);
    }
}


void Generate_Asm_Condition (Token* node, FILE* asm_file , Label_Id* label_id , const char* if_or_while)
{
    fprintf (asm_file , "\n; Я в начале условия\n");
    assert (node);
    my_assert (node->token_type != TOKEN_OPERATOR ||
         (node->value < OP_LEADER || node->value > OP_GAP));

    int counter = 0;
    if (strcmp(if_or_while , "if") == 0)
        counter = n_if;

    else if (strcmp(if_or_while , "while") == 0)
        counter = n_while;

    Generate_Asm_Expression (node->left , asm_file , label_id);
    Generate_Asm_Expression (node->right , asm_file , label_id);

    switch (node->value) {

        case OP_LEADER:
                        fprintf (asm_file , "ja %s_end_%d" , if_or_while , n_if);
                        counter++;
                        break;

        case OP_UNDERDOG:
                        fprintf (asm_file , "jb %s_end_%d" , if_or_while , counter);
                        counter++;
                        break;

        case OP_ADVANTAGE:
                        fprintf (asm_file , "jae %s_end_%d" , if_or_while , counter);
                        counter++;
                        break;

        case OP_PRESSING:
                        fprintf (asm_file , "jbe %s_end_%d" , if_or_while , counter);
                        counter++;
                        break;

        case OP_DRAW:
                        fprintf (asm_file , "je %s_end_%d" , if_or_while , counter);
                        counter++;
                        break;

        case OP_GAP:
                        fprintf (asm_file , "jne %s_end_%d" , if_or_while , counter);
                        counter++;
                        break;
    }

    fprintf (asm_file , "\n; Я в конце условия\n");

}


void Generate_Asm_If_While_Body(Token* node, FILE* asm_file, Label_Id* label_id, const char* if_or_while)
{
    assert(node);
    assert(asm_file);
    assert(label_id);

    fprintf(asm_file, "\n; Я в начале тела \n");

    // // Генерируем уникальную метку для конца блока if/while
    // int end_label = label_id->n_if;
    // label_id->n_if++;

    Generate_Asm_Multiple_Op(node, asm_file, label_id);

    if (strcmp(if_or_while, "while") == 0) {
        fprintf(asm_file, "jmp %s_start_%d\n", if_or_while, n_while);
    }

    // Метка конца блока
    fprintf(asm_file, "%s_end_%d:\n", if_or_while, n_while);

    fprintf(asm_file, "\n; Я в конце тела %s\n", if_or_while);
}


void Generate_Asm_If_Statement(Token* node, FILE* asm_file, Label_Id* label_id)
{
    assert(node);
    assert(asm_file);
    assert(label_id);

    fprintf(asm_file, "\n; Генерация кода для if\n");

    // Генерация условия
    Generate_Asm_Condition(node->left, asm_file, label_id, "if");

    // Генерация тела
    Generate_Asm_If_While_Body(node->right, asm_file, label_id, "if");

    fprintf(asm_file, "\n; Конец кода для if\n");
}


void Generate_Asm_While_Statement(Token* node, FILE* asm_file, Label_Id* label_id)
{
    assert(node);
    assert(asm_file);
    assert(label_id);

    fprintf(asm_file, "\n; Генерация кода для while\n");

    // Метка начала условия

    fprintf(asm_file, "while_start_%d:\n", n_while);

    // Генерация условия
    Generate_Asm_Condition(node->left, asm_file, label_id, "while");

    // Генерация тела
    Generate_Asm_If_While_Body(node->right, asm_file, label_id, "while");

    fprintf(asm_file, "\n; Конец кода для while\n");
}


void Generate_Asm_Param_Call_Func (Token* node , FILE* asm_file , Label_Id* label_id)
{
    assert (asm_file);
    assert (label_id);

    fprintf (asm_file , "\n; Генерация параметров при вызове функции\n");

    Token* param = node;

    while (param != NULL) {

        Generate_Asm_Expression (param->left , asm_file , label_id);

        param = param->right;
    }

    fprintf (asm_file , "\n; Конец генерация параметров при вызове функции\n");
}


void Generate_Asm_Call_Func (Token* node , FILE* asm_file , Label_Id* label_id)
{
    assert (node);
    assert (asm_file);
    assert (label_id);

    fprintf (asm_file , "\n; Генерация кода для ВЫЗОВА ФУНКЦИИ\n");

    Generate_Asm_Param_Call_Func (node->right , asm_file , label_id);

    if (node->token_type == TOKEN_KEY_WORD && node->value == KW_CALL_FUNC) {
        fprintf (asm_file , "call %s:\n" , label_id->table_label[node->left->value].name_identifier);
    }


    fprintf (asm_file , "\n; Конец генерации кода для ВЫЗОВА ФУНКЦИИ\n");
}

void Generate_Asm_Return_Value (Token* node, FILE* asm_file, Label_Id* label_id)
{
    assert(asm_file);
    assert(label_id);

    fprintf(asm_file, "\n\n; Генерация кода для возвращаемого значения функции\n\n");

    // Генерация кода для выражения, возвращаемого функцией
    if (node != NULL) {
        Generate_Asm_Expression (node->left, asm_file, label_id);

        fprintf(asm_file, "pop ax ; Перенос результата в регистр ax для возврата\n");
    }
    fprintf(asm_file, "\n\n; Конец генерации кода для возвращаемого значения функции\n\n");
}


void Generate_Asm_Body_Function (Token* node , FILE* asm_file , Label_Id* label_id)
{
    fprintf (asm_file , "\n; Я в теле функции\n");

    Generate_Asm_Multiple_Op (node->right->right , asm_file , label_id);

    fprintf (asm_file , "\n; Я в конце  тела функции\n");
}

void Generate_Asm_Function (Token* node , FILE* asm_file , Label_Id* label_id)
{
    assert (asm_file);
    assert (label_id);
    my_assert (node->token_type != TOKEN_KEY_WORD && node->value != KW_INIT_FUNC);

    Token* init_func = node->left;
    Token* new_block = init_func->right;
    Label_Id* local_label = new_block->local_label_id;

    Generate_Asm_Body_Function (init_func , asm_file , local_label);
}


void Generate_Asm_Print (Token* node , FILE* asm_file , Label_Id* label_id)
{
        const char* comment = "     ;PRINT";

        if (node->left->token_type == TOKEN_IDENTIFIER)
            fprintf (asm_file , "push [%d] %s\n" , node->left->value , comment);

        else if (node->left->token_type == TOKEN_NUMBER)
            fprintf (asm_file , "push %d %s\n" , node->left->value , comment);

        else {

            fprintf (stderr , "ОШИБКА , ожидалось имя переменной или число");
            assert (0);
        }

        fprintf (asm_file , "out\n");
}


