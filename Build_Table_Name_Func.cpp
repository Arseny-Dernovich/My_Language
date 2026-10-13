#include "Read.h"

static void Scope_Error (const char * message , int name_id , const Program_Context * context)
{
    fprintf (stderr , "Scope error: %s '%s'\n" , message ,
             context->names->table_label[name_id].name_identifier);
    exit (EXIT_FAILURE);
}


static Scope * New_Scope (Scope * parent)
{
    Scope * scope = (Scope*) calloc (1 , sizeof (Scope));

    if (scope == NULL) {

        perror ("Cannot allocate scope");
        exit (EXIT_FAILURE);
    }
    scope->parent = parent;

    return scope;
}


static int Declare_Name (Scope * scope , int name_id , Program_Context * context)
{

    for (int i = 0; i < scope->size; i++) {

        if (scope->names[i] == name_id)
            Scope_Error ("Repeated declaration of" , name_id , context);
    }

    if (scope->size >= MAX_LABELS || context->next_slot >= 10000) {

        fprintf (stderr , "Scope error: too many variables\n");
        exit (EXIT_FAILURE);
    }

    int slot = context->next_slot++;
    scope->names[scope->size] = name_id;
    scope->slots[scope->size] = slot;
    scope->size++;

    return slot;
}


static int Find_Name (const Scope * scope , int name_id , const Program_Context * context)
{

    for (const Scope * current = scope; current != NULL; current = current->parent) {

        for (int i = current->size - 1; i >= 0; i--) {

            if (current->names[i] == name_id) {

                return current->slots[i];

            }
        }
    }
    Scope_Error ("Unknown variable" , name_id , context);

    return - 1;
}


static const Function_Info * Find_Function (int name_id , const Program_Context * context)
{

    for (int i = 0; i < context->function_count; i++) {

        if (context->functions[i].name_id == name_id) {

            return &context->functions[i];

        }
    }
    Scope_Error ("Unknown function" , name_id , context);

    return NULL;
}


static void Bind_Expression (Token * node , Scope * scope , Program_Context * context)
{

    if (node == NULL) {

        return;
    }

    if (node->token_type == TOKEN_IDENTIFIER) {

        node->slot = Find_Name (scope , node->value , context);

        return;
    }

    if (node->token_type == TOKEN_KEY_WORD && node->value == KW_CALL_FUNC) {

        const Function_Info * function = Find_Function (node->left->value , context);
        int argument_count = 0;

        for (Token * argument = node->right; argument != NULL; argument = argument->right) {

            Bind_Expression (argument->left , scope , context);
            argument_count++;
        }

        if (argument_count != function->parameter_count) {

            fprintf (stderr , "Scope error: function '%s' needs %d arguments, got %d\n" ,
                     context->names->table_label[function->name_id].name_identifier ,
                     function->parameter_count , argument_count);
            exit (EXIT_FAILURE);
        }

        return;
    }

    Bind_Expression (node->left , scope , context);
    Bind_Expression (node->right , scope , context);
}


static void Bind_Statements (Token * node , Scope * scope , Program_Context * context)
{

    for (Token * statement = node; statement != NULL; statement = statement->right) {

        assert (statement->token_type == TOKEN_OPERATOR && statement->value == OP_WHISTLE);
        Token * operation = statement->left;
        assert (operation != NULL);

        if (operation->value == OP_GOAT && operation->token_type == TOKEN_KEY_WORD) {

            operation->left->slot = Declare_Name (scope , operation->left->value , context);
            Bind_Expression (operation->right , scope , context);

        }

        else if (operation->value == OP_SCORE && operation->token_type == TOKEN_OPERATOR) {
            Bind_Expression (operation->left , scope , context);
            Bind_Expression (operation->right , scope , context);

        }

        else if (operation->value == KW_PASS || operation->value == KW_DRIBBLE) {
            Bind_Expression (operation->left , scope , context);
            Token * block = operation->right;
            assert (block != NULL && block->token_type == TOKEN_NEW_BLOCK);
            block->scope = New_Scope (scope);
            Bind_Statements (block->right , block->scope , context);

        }

        else if (operation->value == KW_PRINT || operation->value == KW_RETURN) {
            Bind_Expression (operation->left , scope , context);

        }

        else if (operation->value == KW_CALL_FUNC) {
            Bind_Expression (operation , scope , context);

        }

        else {
            fprintf (stderr , "Scope error: unsupported statement %d\n" , operation->value);
            exit (EXIT_FAILURE);
        }
    }
}


void ProcessTree (Token * root , Program_Context * context , const Label_Id * global_name_table)
{
    assert (root != NULL && context != NULL && global_name_table != NULL);
    context->names = global_name_table;
    context->global_scope.parent = NULL;

    for (Token * item = root->left; item != NULL; item = item->right) {

        if (context->function_count >= MAX_LABELS) {

            fprintf (stderr , "Scope error: too many functions\n");
            exit (EXIT_FAILURE);
        }

        Token * function = item->left;
        int name_id = function->left->left->value;

        for (int i = 0; i < context->function_count; i++) {

            if (context->functions[i].name_id == name_id)
                Scope_Error ("Repeated function" , name_id , context);
        }

        Function_Info * info = &context->functions[context->function_count++];
        info->node = function;
        info->name_id = name_id;

        for (Token * param = function->left->right; param != NULL; param = param->right)
            info->parameter_count++;
    }

    Bind_Statements (root->right , &context->global_scope , context);

    for (int i = 0; i < context->function_count; i++) {

        Function_Info * info = &context->functions[i];
        Token * body = info->node->right;
        body->scope = New_Scope (&context->global_scope);
        info->frame_start = context->next_slot;

        for (Token * param = info->node->left->right; param != NULL; param = param->right)
            param->left->slot = Declare_Name (body->scope , param->left->value , context);

        Bind_Statements (body->right , body->scope , context);
        info->frame_size = context->next_slot - info->frame_start;
    }
}


void Free_Scopes (Token * root)
{

    if (root == NULL) {

        return;
    }
    Free_Scopes (root->left);
    Free_Scopes (root->right);
    free (root->scope);
    root->scope = NULL;
}
