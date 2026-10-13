#include "Read.h"
#include "Back_end.h"

static int next_label = 1;

static const Function_Info * Find_Function_Info (int name_id , const Program_Context * context)
{

    for (int i = 0; i < context->function_count; i++) {

        if (context->functions[i].name_id == name_id) {

            return &context->functions[i];

        }
    }
    fprintf (stderr , "Backend error: function %d not found\n" , name_id);
    exit (EXIT_FAILURE);
}


static const char * Function_Name (const Function_Info * function , const Program_Context * context)
{

    return context->names->table_label[function->name_id].name_identifier;
}

static void Generate_Asm_Expression (Token * node , FILE * asm_file ,
                                     const Program_Context * context , const Function_Info * current);

static void Generate_Asm_Call_Func (Token * node , FILE * asm_file ,
                                   const Program_Context * context , const Function_Info * current)
{
    const Function_Info * target = Find_Function_Info (node->left->value , context);

    fprintf (asm_file , "; Вызов функции %s\n" , Function_Name (target , context));

    if (current != NULL) {

        fprintf (asm_file , "; Сохранение кадра вызывающей функции\n");

        for (int i = 0; i < current->frame_size; i++) {
            int slot = current->frame_start + i;
            fprintf (asm_file , "push [%d] ; frame slot\n" , slot);
        }
    }

    for (Token * argument = node->right; argument != NULL; argument = argument->right)
        Generate_Asm_Expression (argument->left , asm_file , context , current);

    fprintf (asm_file , "call func_%s\n" , Function_Name (target , context));

    if (current != NULL) {

        fprintf (asm_file , "; Восстановление кадра вызывающей функции\n");

        for (int i = current->frame_size - 1; i >= 0; i--) {
            int slot = current->frame_start + i;
            fprintf (asm_file , "pop [%d] ; frame slot\n" , slot);
        }
    }

    fprintf (asm_file , "; Результат вызова из ax помещается на стек выражений\n");
    fprintf (asm_file , "push ax\n");
}

static void Generate_Asm_Expression (Token * node , FILE * asm_file ,
                                     const Program_Context * context , const Function_Info * current)
{

    if (node == NULL) {

        return;
    }

    if (node->token_type == TOKEN_IDENTIFIER) {

        assert (node->slot >= 0);
        fprintf (asm_file , "push [%d] ; id =%s\n" , node->slot ,
                 context->names->table_label[node->value].name_identifier);

        return;
    }

    if (node->token_type == TOKEN_NUMBER) {

        fprintf (asm_file , "push %d\n" , node->value);

        return;
    }

    if (node->token_type == TOKEN_KEY_WORD && node->value == KW_CALL_FUNC) {

        Generate_Asm_Call_Func (node , asm_file , context , current);

        return;
    }

    assert (node->token_type == TOKEN_OPERATOR);
    Generate_Asm_Expression (node->left , asm_file , context , current);
    Generate_Asm_Expression (node->right , asm_file , context , current);

    switch (node->value) {

        case OP_GOAL:       fprintf (asm_file , "add\n"); break;

        case OP_FOUL:       fprintf (asm_file , "sub\n"); break;

        case OP_ASSIST:     fprintf (asm_file , "mul\n"); break;

        case OP_SUBSTITUTE: fprintf (asm_file , "div\n"); break;

        default:

            fprintf (stderr , "Backend error: unknown arithmetic operator %d\n" , node->value);
            exit (EXIT_FAILURE);
    }
}

static void Generate_Asm_Condition (Token * node , FILE * asm_file ,
                                    const Program_Context * context , const Function_Info * current ,
                                    const char * end_label , int label_id)
{
    fprintf (asm_file , "; Я в начале условия\n");
    Generate_Asm_Expression (node->left , asm_file , context , current);
    Generate_Asm_Expression (node->right , asm_file , context , current);

    const char * jump = NULL;

    switch (node->value) {

        case OP_LEADER:    jump = "jbe"; break;

        case OP_UNDERDOG:  jump = "jae"; break;

        case OP_ADVANTAGE: jump = "jb";  break;

        case OP_PRESSING:  jump = "ja";  break;

        case OP_DRAW:      jump = "jne"; break;

        case OP_GAP:       jump = "je";  break;

        default:

            fprintf (stderr , "Backend error: unknown comparison %d\n" , node->value);
            exit (EXIT_FAILURE);
    }
    fprintf (asm_file , "%s %s_%d\n" , jump , end_label , label_id);
    fprintf (asm_file , "; Я в конце условия\n");
}

static void Generate_Asm_Statements (Token * node , FILE * asm_file ,
                                     const Program_Context * context , const Function_Info * current)
{
    if (node == NULL)
        return;

    fprintf (asm_file , "; Я в Multiple_Op\n");

    for (Token * statement = node; statement != NULL; statement = statement->right) {

        Token * operation = statement->left;
        assert (operation != NULL);

        if (operation->value == OP_GOAT && operation->token_type == TOKEN_KEY_WORD) {

            if (operation->right != NULL)
                Generate_Asm_Expression (operation->right , asm_file , context , current);

            else
                fprintf (asm_file , "push 0\n");
            fprintf (asm_file , "pop [%d] ; id =%s\n" , operation->left->slot ,
                     context->names->table_label[operation->left->value].name_identifier);

        }

        else if (operation->value == OP_SCORE && operation->token_type == TOKEN_OPERATOR) {
            Generate_Asm_Expression (operation->right , asm_file , context , current);
            fprintf (asm_file , "pop [%d] ; id =%s\n" , operation->left->slot ,
                     context->names->table_label[operation->left->value].name_identifier);

        }

        else if (operation->value == KW_PRINT) {
            Generate_Asm_Expression (operation->left , asm_file , context , current);
            fprintf (asm_file , "out\n");

        }

        else if (operation->value == KW_RETURN) {
            fprintf (asm_file , "; Генерация кода для возвращаемого значения функции\n");
            Generate_Asm_Expression (operation->left , asm_file , context , current);
            fprintf (asm_file , "pop ax ; Перенос результата в регистр ax для возврата\nret\n");
            fprintf (asm_file , "; Конец генерации кода для возвращаемого значения функции\n");

        }

        else if (operation->value == KW_CALL_FUNC) {
            Generate_Asm_Call_Func (operation , asm_file , context , current);
            fprintf (asm_file , "pop ax\n");

        }

        else if (operation->value == KW_PASS || operation->value == KW_DRIBBLE) {
            int label_id = next_label++;
            const char * label = operation->value == KW_PASS ? "if" : "while";

            fprintf (asm_file , "; Генерация кода для %s\n" , label);

            if (operation->value == KW_DRIBBLE)
                fprintf (asm_file , "%s_start_%d:\n" , label , label_id);

            Generate_Asm_Condition (operation->left , asm_file , context , current , label , label_id);
            fprintf (asm_file , "; Я в начале тела %s\n" , label);
            Generate_Asm_Statements (operation->right->right , asm_file , context , current);
            fprintf (asm_file , "; Я в конце тела %s\n" , label);

            if (operation->value == KW_DRIBBLE)
                fprintf (asm_file , "jmp %s_start_%d\n" , label , label_id);

            fprintf (asm_file , "%s_%d:\n" , label , label_id);
            fprintf (asm_file , "; Конец кода для %s\n" , label);
        }
    }

    fprintf (asm_file , "; Конец Multiple_Op\n");
}


void Generate_Asm_Program (Token * root , FILE * asm_file , const Program_Context * context)
{
    assert (root != NULL && asm_file != NULL && context != NULL);

    if (context->function_count == 0) {

        fprintf (stderr , "Backend error: no functions in program\n");
        exit (EXIT_FAILURE);
    }

    fprintf (asm_file , "; Глобальные инструкции программы\n");
    Generate_Asm_Statements (root->right , asm_file , context , NULL);

    // main — точка входа; если её нет, используем первую функцию учебной программы.
    const Function_Info * entry = &context->functions[0];

    for (int i = 0; i < context->function_count; i++) {

        if (strcmp (Function_Name (&context->functions[i] , context) , "main") == 0)
            entry = &context->functions[i];
    }

    if (entry->parameter_count != 0) {

        fprintf (stderr , "Backend error: entry function needs parameters\n");
        exit (EXIT_FAILURE);
    }
    fprintf (asm_file , "; Точка входа программы\n");
    fprintf (asm_file , "call func_%s\nhlt\n" , Function_Name (entry , context));

    for (int i = 0; i < context->function_count; i++) {

        const Function_Info * function = &context->functions[i];
        fprintf (asm_file , "; Функция %s: начало кадра RAM %d, размер %d\n" ,
                 Function_Name (function , context) , function->frame_start , function->frame_size);
        fprintf (asm_file , "func_%s:\n" , Function_Name (function , context));
        fprintf (asm_file , "; Я в теле функции %s\n" , Function_Name (function , context));

        Token * param = function->node->left->right;
        int slots[MAX_LABELS] = {};
        int count = 0;

        for (; param != NULL; param = param->right)
            slots[count++] = param->left->slot;

        fprintf (asm_file , "; Приём параметров функции\n");
        for (int j = count - 1; j >= 0; j--)
            fprintf (asm_file , "pop [%d] ; параметр кадра\n" , slots[j]);

        Generate_Asm_Statements (function->node->right->right , asm_file , context , function);
        fprintf (asm_file , "; Неявный возврат 0 из функции %s\npush 0\npop ax\nret\n" ,
                 Function_Name (function , context));
    }
}
