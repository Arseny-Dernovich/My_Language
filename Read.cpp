#include "Read.h"


int Find_In_Table_Func (Label_Id* label_id , const char* name , Namel_Label type)
{
    for (int i = 0; i < label_id->table_size; i++) {
        if (strcmp (label_id->table_label[i].name_identifier , name) == 0 && type == FUNCTION) {
            return i;  // Уже существует , возвращаем индекс
        }
    }

    return -1; // не найдено
}



int Find_In_Table_Func_For_Tabel (Label_Id* label_id , const char* name)
{
    for (int i = 0; i < label_id->table_size; i++) {
        if (strcmp (label_id->table_label[i].name_identifier , name) == 0 &&  label_id->table_label[i].type_label == FUNCTION) {
            return i;  // Уже существует , возвращаем индекс
        }
    }

    return -1; // не найдено
}


int Find_In_Table_Id (Label_Id* label_id , const char* name , Namel_Label type)
{
    for (int i = 0; i < label_id->table_size; i++) {
        if (strcmp (label_id->table_label[i].name_identifier , name) == 0 && type == IDENTIFIER) {
            return i;  // Уже существует , возвращаем индекс
        }
    }

    return -1; // не найдено
}


int Find_In_Table_Id_For_Table (Label_Id* label_id , const char* name)
{
    for (int i = 0; i < label_id->table_size; i++) {
        if (strcmp (label_id->table_label[i].name_identifier , name) == 0 && label_id->table_label[i].type_label == IDENTIFIER) {
            return i;  // Уже существует , возвращаем индекс
        }
    }

    return -1; // не найдено
}


int Add_Identifier (Label_Id* label_id , const char* name , Namel_Label type)
{

    if (label_id->table_size < MAX_LABELS) {

        label_id->table_label[label_id->table_size].name_identifier = strdup (name);
        label_id->table_label[label_id->table_size].num_letters = strlen (name);
        label_id->table_label[label_id->table_size].type_label = type;
        label_id->table_size++;
        return 999;  // Увеличиваем размер таблицы и возвращаем индекс

    } else {

        fprintf (stderr , "Error: Label table overflow.\n");
        exit (EXIT_FAILURE);  // Если таблица переполнена
    }
}



void Build_Name_Table (const char* input , Label_Id* label_id)
{
    const char* cursor = input;
    while (*cursor != '\0') {
        if (*cursor == '$') {

            cursor++;

            char buffer[MAX_IDENTIFIER_LENGTH] = "";
            int length = 0;


            while (*cursor != '\0' && *cursor != '$') {
                buffer[length++] = *cursor;
                cursor++;
            }


            if (*cursor == '$') {

                cursor++;
                buffer[length] = '\0';

                if (Find_In_Table_Func_For_Tabel (label_id , buffer) == -1 && Find_Operator (buffer) < 0)
                Add_Identifier (label_id , buffer , FUNCTION);

            } else {

                fprintf (stderr , "Error: Unmatched '$' in input.\n");
                return; // Ошибка: пропущен закрывающий символ '$'
            }
        }

        else if (isalpha (*cursor) || (*cursor == '/' && * (cursor + 1) == ':')) {

            char buffer[MAX_IDENTIFIER_LENGTH] = "";
            int length = 0;


            if (*cursor == '/' && * (cursor + 1) == ':') {
                buffer[length++] = *cursor++;
                buffer[length++] = *cursor++;
            }

            while (isalnum (*cursor) || *cursor == '_') {
                buffer[length++] = *cursor;
                cursor++;
            }
            buffer[length] = '\0';

            if (Find_In_Table_Id_For_Table (label_id , buffer) == -1 && Find_Operator (buffer) < 0) {
                Add_Identifier (label_id ,  buffer , IDENTIFIER);
            }

        } else {
            cursor++;
        }
    }
}


void Remove_Dollar_Sign (char* str)
{
    char* p = str;
    while (*p) {
        if (*p == '$') {
            memmove (p , p + 1 , strlen (p));
        } else {
            p++;
        }
    }
}



void Lexical_Analyzer (const char* expr , Token_Aftomat* token_aftomat , Label_Id* label_id)
{
    // fprintf (stderr , "\nHUUUIII\n");
    int pos = 0;  // Текущая позиция в строке
    int index = 0; // Индекс в массиве токенов

    while (expr[pos] != '\0') {

        if (isdigit (expr[pos])) {

            // Это число
            int value = 0;
            while (isdigit (expr[pos])) {

                value = value * 10 + (expr[pos] - '0');
                pos++;
            }
            token_aftomat->aftomat_tokens[index++] = (Token) {TOKEN_NUMBER , value ,  NULL , NULL};

        } else if (isalpha (expr[pos]) || (expr[pos] == '/' && expr[pos + 1] == ':') || expr[pos] == '$') {

            char name[100] = "";
            int name_pos = 0;

            if (expr[pos] == '$') {
                name[name_pos++] = expr[pos++];  // Добавляем первый символ '$'

                // Собираем слово между $$
                while (isalnum (expr[pos]) || expr[pos] == '_') {
                    name[name_pos++] = expr[pos++];
                }

                if (expr[pos] == '$') {
                    name[name_pos++] = expr[pos++];
                }
            }


            if (expr[pos] == '/' && expr[pos + 1] == ':') {

                name[name_pos++] = expr[pos++];
                name[name_pos++] = expr[pos++];
            }

            while (isalnum (expr[pos]) || expr[pos] == '_') {

                name[name_pos++] = expr[pos++];
            }
            name[name_pos] = '\0';
            // printf ("\n\n@@@@@@@@%s@@@@@@@\n\n" , name);

            int op_index = Find_Operator (name);
            // if (strcmp (name , "GOAT") == 0)
                // printf ("\n\n@@@@@@@@%d@@@@@@@\n\n" , Find_Operator (name));

            // printf ("\n\n@@@@@@@@@ SSUUII_index = %d\n\n" , Find_Operator ("SSUUII"));

            if (op_index != -1) {

                if (words_operators[op_index].is_words_or_op == KEY_WORDS) {

                    int value = op_index;
                    // const char* name_key_words = words_operators[value].name;
                    // printf ("%s = %d " , name_key_words , value);
                    // printf ("\n\n@@@@@@@@%s@@@@@@@\n\n" , name_key_words);
                    token_aftomat->aftomat_tokens[index++] = (Token) {TOKEN_KEY_WORD , op_index , NULL , NULL};
                }

                else if (words_operators[op_index].is_words_or_op == OPER) {

                    int value = op_index;
                    // const char* name_operator = words_operators[value].name;
                    token_aftomat->aftomat_tokens[index++] = (Token) {TOKEN_OPERATOR , op_index , NULL , NULL};
                }
            }

            else {

                char original_name[40] = "";
                strcpy (original_name , name);  // Сохраняем исходное имя для дальнейшей проверки на наличие $

                // Убираем символы $ из имени
                Remove_Dollar_Sign (name);

                // Если символ $ присутствует в оригинальном имени , считаем это функцией
                if (strchr (original_name , '$') != NULL) {

                    int func_index = Find_In_Table_Func (label_id , name , FUNCTION);
                    if (func_index != -1) {
                        // const char* name_func = label_id->table_label[func_index].name_identifier;
                        token_aftomat->aftomat_tokens[index++] = (Token) {TOKEN_FUNCTION , func_index , NULL , NULL};
                    }
                } else {

                    int id_index = Find_In_Table_Id (label_id , name , IDENTIFIER);
                    if (id_index != -1) {
                        // const char* name_id = label_id->table_label[id_index].name_identifier;
                        token_aftomat->aftomat_tokens[index++] = (Token) {TOKEN_IDENTIFIER , id_index , NULL , NULL};
                    }
                }
            }
        }


        else {
            pos++;
        }
    }

    token_aftomat->token_size = index;
}




char* Read_File_To_Buffer (const char* filename)
{
    struct stat st = {};
    FILE* file = fopen (filename , "r");
    if  (!file) {

        perror ("Failed to open file");
        exit (EXIT_FAILURE);
    }

    if  (fstat (fileno (file) , &st) == -1) {

        perror ("Failed to get file size");
        fclose (file);
        exit (EXIT_FAILURE);
    }

    long size = st.st_size;

    char* buffer =  (char*) calloc (1 , size + 1);
    if  (!buffer) {

        perror ("Failed to allocate memory");
        fclose (file);
        exit (EXIT_FAILURE);
    }

    int j = 0;
    for  (int i = 0; i < size; i++) {
        char ch = fgetc (file);
        if  (ch == EOF) break;

            buffer[j++] = ch;

    }

    buffer[j] = '\0';

    fclose (file);
    return buffer;
}

void Syntax_Error ()
{
    printf ("Syntax Error\n");
    exit (EXIT_FAILURE);
}

int Find_Operator (const char* name)
{
    for (int i = 0  ; i < size_main_op ; i++) {
        if (strcmp (name , words_operators[i].name) == 0) {
            // printf ("%s = %d " , name , i);
            return words_operators[i].op;
        }
    }
    return -1; // Не найдено
}

void Print_Name_Table (Label_Id* label_id)
{
    printf ("Name Table:\n");
    for (int i = 0; i < label_id->table_size; i++) {
        printf ("Identifier[%d]: Name = %s , type = %d\n" , i , label_id->table_label[i].name_identifier , label_id->table_label[i].type_label);
    }
}



void Print_Token_Array (Token_Aftomat token_aftomat , Label_Id label_id)
{
    printf ("\nToken Array:\n");

    for (int i = 0; i < token_aftomat.token_size; i++) {

        printf ("Token[%d]: " , i);

        int value        = token_aftomat.aftomat_tokens[i].value;
        int type         = token_aftomat.aftomat_tokens[i].token_type;

        if (type == TOKEN_OPERATOR) {
            // Выводим имя из words_operators для операторов и ключевых слов
            if (value >= 0 && value < size_main_op ) {

                printf ("Type=%d , Value = %d Name=%s\n" , type , value ,  words_operators[value].name);

            } else {
                printf ("Unknown operator (value: %d)\n" , value);
            }

        } else if (type == TOKEN_KEY_WORD) {

            if (value >= 0 && value <= size_main_op ) {

                printf ("Type=%d , Value = %d Name=%s\n" , type , value , words_operators[value].name);

            }

        } else if (type == TOKEN_IDENTIFIER) {
            printf ("Type=%d , Value = %d ,  Name=%s\n" , type , value , label_id.table_label[value].name_identifier);

        } else if (type == TOKEN_FUNCTION) {
            printf ("Type=%d , Name=%s\n" , type , label_id.table_label[value].name_identifier);

        } else {
            printf ("Type=%d , Value=%d\n" , type , value);
        }
    }
}
