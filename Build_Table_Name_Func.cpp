#include "Read.h"

// void Add_Local_Identifier (Token* node,  Label_Id* label_first_label_id , Label_Id* local_label_id)
// {
//
// }
//
// void Build_func_Table_Name (Token* node , Token* block , int index , Label_Id* label_id)
// {
//     Token* func_node = node->left
//     if ((node->token_type != TOKEN_KEY_WORD && node->value != OP_GOAT) && (node->left->token_type !=
//
//



void InitializeLocalNameTable(Token* new_block_node, const Label_Id* global_name_table,  Token* param_list)
{
     assert(new_block_node);
    assert(global_name_table);

    // Проверяем и инициализируем local_label_id
    if (new_block_node->local_label_id == NULL) {
        new_block_node->local_label_id = (Label_Id*)calloc(1, sizeof(Label_Id));
        if (new_block_node->local_label_id == NULL) {
            fprintf(stderr, "Error: Memory allocation for local_label_id failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Инициализация таблицы имен для узла NEW_BLOCK
    Label_Id* local_name_table = new_block_node->local_label_id;
    local_name_table->table_size = 0;

    if (param_list == NULL) {
        printf("Warning: param_list is NULL.\n");
        return;
    }

    Token* current_param = param_list;
    while (current_param != NULL) {
        if (current_param->left == NULL) {
            fprintf(stderr, "Error: current_param->left is NULL.\n");
            exit(EXIT_FAILURE);
        }

        int global_index = current_param->left->value;

        // Проверка на корректность индекса
        if (global_index < 0 || global_index >= MAX_LABELS) {
            fprintf(stderr, "Error: global_index (%d) is out of bounds.\n", global_index);
            exit(EXIT_FAILURE);
        }

        if (local_name_table->table_size < MAX_LABELS) {
            local_name_table->table_label[local_name_table->table_size] = global_name_table->table_label[global_index];
            printf("Added param: %s\n", global_name_table->table_label[global_index].name_identifier);
            local_name_table->table_size++;
        } else {
            fprintf(stderr, "Error: Local name table exceeded its maximum size.\n");
            exit(EXIT_FAILURE);
        }

        current_param = current_param->right;
    }

    if (new_block_node->right) {
        AddVariablesFromBody(new_block_node->right, local_name_table, global_name_table);
    }
}
void ProcessFunction(Token* int_node, const Label_Id* global_name_table)
{
    assert(int_node);
    assert(int_node->token_type == TOKEN_KEY_WORD);
    assert(global_name_table);

    // Узел FUNC — левый потомок узла int
    Token* func_node = int_node->left;
    assert(func_node && func_node->token_type == TOKEN_KEY_WORD && func_node->value == KW_INIT_FUNC);

    // Перечисление параметров — правое поддерево узла FUNC
    Token* param_list = func_node->right;

    // Узел NEW_BLOCK — правый потомок узла int
    Token* new_block_node = int_node->right;
    assert(new_block_node && new_block_node->token_type == TOKEN_NEW_BLOCK);

    // Инициализация локальной таблицы имен для узла NEW_BLOCK
    InitializeLocalNameTable(new_block_node, global_name_table, param_list);

    ReplaceGlobalIndicesWithLocal(new_block_node->right, new_block_node->local_label_id , global_name_table);
}

void ProcessTree(Token* root, const Label_Id* global_name_table) {
    if (!root) return; // Если узел NULL, ничего не делаем

    // Проверяем, что root имеет необходимые потомки перед доступом
    if (root->token_type == TOKEN_KEY_WORD && root->value == OP_GOAT) {
        if (root->right && root->right->token_type == TOKEN_NEW_BLOCK && root->right->value == NEW_BLOCK) {
            // Обработка функции
            ProcessFunction(root, global_name_table);
        }
    }

    // Рекурсивный обход дерева с проверкой
    if (root->left) {
        ProcessTree(root->left, global_name_table);
    }

    if (root->right) {
        ProcessTree(root->right, global_name_table);
    }
}

void PrintLocalNameTables(Token* root)
{
    if (!root) return;

    // Проверяем, является ли узел NEW_BLOCK
    if (root->token_type == TOKEN_NEW_BLOCK) {
        Label_Id* local_name_table = root->local_label_id;

        if (local_name_table) {

            printf("Local Name Table for NEW_BLOCK at %p:\n", (void*)root);
            printf("Table size: %d\n", local_name_table->table_size);

            for (int i = 0; i < local_name_table->table_size; i++) {
                Label_Id* label = local_name_table;
                printf("  [%d]: %s (type: %d)\n", i, label->table_label[i].name_identifier, label->table_label[i].type_label);
            }

        } else {
            printf("Local Name Table for NEW_BLOCK at %p is empty (NULL).\n", (void*)root);
        }
    }

    // Рекурсивно обходим дерево
    PrintLocalNameTables(root->left);
    PrintLocalNameTables(root->right);
}


void AddVariablesFromBody(Token* body_node, Label_Id* local_name_table, const Label_Id* global_name_table)
{
    assert(body_node);
    assert(local_name_table);
    assert(global_name_table);

    Token* current_statement = body_node;
    while (current_statement != NULL) {
        // Проверяем, является ли текущий узел оператором WHISTLE (;)
        if (current_statement->token_type == TOKEN_OPERATOR && current_statement->value == OP_WHISTLE) {

            Token* left_child = current_statement->left;

            // Проверяем, является ли левый потомок узлом int (инициализация переменной)
            if (left_child && left_child->token_type == TOKEN_KEY_WORD && left_child->value == OP_GOAT) {

                Token* variable_node = left_child->left;
                if (!variable_node) {

                    fprintf(stderr, "Error: Variable name is missing in int declaration.\n");
                    assert(0);
                }

                int global_index = variable_node->value;

                // Проверка на корректность индекса
                if (global_index < 0 || global_index >= MAX_LABELS) {

                    fprintf(stderr, "Error: global_index (%d) is out of bounds.\n", global_index);
                    assert(0);
                }

                // Добавляем переменную в локальную таблицу имен
                if (local_name_table->table_size < MAX_LABELS) {

                    local_name_table->table_label[local_name_table->table_size] = global_name_table->table_label[global_index];
                    printf("Added variable: %s\n", global_name_table->table_label[global_index].name_identifier);
                    local_name_table->table_size++;

                } else {

                    fprintf(stderr, "Error: Local name table exceeded its maximum size.\n");
                    assert(0);
                }
            }
        }

        // Переход к следующему оператору в правом поддереве
        current_statement = current_statement->right;
    }
}

void ReplaceGlobalIndicesWithLocal (Token* root, const Label_Id* local_name_table , const Label_Id* global_name_table)
{
    if (!root) return;

    // Проверяем, является ли узел переменной или параметром
    if (root->token_type == TOKEN_IDENTIFIER) {
        int global_index = root->value;

        // Проверяем наличие в локальной таблице
        int found = 0;
        for (int i = 0; i < local_name_table->table_size; i++) {
            if (strcmp(local_name_table->table_label[i].name_identifier,
                       global_name_table->table_label[global_index].name_identifier) == 0) {
                root->value = i; // Обновляем индекс на локальный
                found = 1;
                break;
            }
        }

        // Если переменная не найдена в локальной таблице, сообщаем об ошибке
        if (!found) {
            fprintf(stderr, "Error: Variable '%s' used but not INIT.\n",
                    global_name_table->table_label[global_index].name_identifier);
            assert(0);
        }
    }

    // Рекурсивно обрабатываем левое и правое поддерево
    ReplaceGlobalIndicesWithLocal(root->left, local_name_table , global_name_table);
    ReplaceGlobalIndicesWithLocal(root->right, local_name_table , global_name_table);
}
