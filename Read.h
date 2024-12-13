#include "My_features.h"

#define MAX_LABELS 100
#define MAX_TOKENS 5000
#define MAX_IDENTIFIER_LENGTH 20


enum Token_Type {
    TOKEN_NUMBER     ,
    TOKEN_IDENTIFIER ,
    TOKEN_KEY_WORD   ,
    TOKEN_OPERATOR   ,
    TOKEN_FUNCTION   ,
};

enum Operators { //НАВЗАНИЕ
    OP_GOAT  ,             // int
    OP_GOAL  ,             // +
    OP_FOUL,               // -
    OP_ASSIST,             // *
    OP_SUBSTITUTE,         // /
    OP_SCORE,              // =
    OP_WHISTLE,            // ;
    OP_OFFSIDE,
    OP_LSTADIUM,           // (
    OP_RSTADIUM,           // )
    OP_L_FIGURE_STADIUM ,  // {
    OP_R_FIGURE_STADIUM ,  // }
    KW_PASS ,              // if
    KW_DRIBBLE,            // while
    KW_SHOOT,              // else
    KW_START_FUNC,         // начало функции
    KW_END_FUNC,           //конец функции
    KW_CALL_FUNC,          //вызов функции
    KW_RETURN,             //return
    OP_NAN,
};

struct Main_Operators_Words {
    const char* name;
    const char* real_name;
    enum Operators op;
    int is_words_or_op;
};

enum Is_Key_Words_or_Op {
    KEY_WORDS = 1 ,
    OPER = 2 ,
};



const  Main_Operators_Words words_operators[] = {
    {"GOAT",          "int",   OP_GOAT ,             KEY_WORDS},
    {"goal",          "+",     OP_GOAL ,             OPER},
    {"foul",          "-",     OP_FOUL ,             OPER},
    {"assist",        "*",     OP_ASSIST ,           OPER},
    {"substitute",    "/",     OP_SUBSTITUTE ,       OPER},
    {"score",         "=",     OP_SCORE ,            OPER},
    {"whistle",       ";",     OP_WHISTLE ,          OPER},
    {"offside",       ",",     OP_OFFSIDE ,          OPER},
    {"stadium_open",  "(",     OP_LSTADIUM ,         OPER},
    {"stadium_close", ")",     OP_RSTADIUM ,         OPER},
    {"start_game",    "{",     OP_L_FIGURE_STADIUM , OPER},
    {"end_game",      "}",     OP_R_FIGURE_STADIUM , OPER},
    {"pass",          "if",    KW_PASS ,             KEY_WORDS},
    {"dribble",       "while", KW_DRIBBLE ,          KEY_WORDS},
    {"shoot",         "else",  KW_SHOOT ,            KEY_WORDS},
    {"/:first_half",  "start", KW_START_FUNC ,       KEY_WORDS},
    {"/:second_half", "end",   KW_END_FUNC ,         KEY_WORDS},
    {"transfer",      "call",  KW_CALL_FUNC,         KEY_WORDS},
    {"SSUUII",        "return",KW_RETURN ,           KEY_WORDS},
};

const int size_main_op = sizeof(words_operators) / sizeof (words_operators[0]);

// enum Key_Words {
//     KW_PASS = 1,  // if
//     KW_DRIBBLE,   // while
//     KW_SHOOT      // else
// };

struct Token {
    enum Token_Type token_type;
    int value;
    const char* name;
    Token* left;
    Token* right;
};

enum Namel_Label { //RENAME
    IDENTIFIER = 333,
    FUNCTION = 334,
};

struct Name_label_identifier {
    int num_letters;
    Namel_Label type_label;
    char* name_identifier;
};

struct Token_Aftomat {  //USE STACK
    Token aftomat_tokens[MAX_TOKENS];
    int token_size;
};

struct Label_Id {       // USE STACK
    Name_label_identifier table_label[MAX_LABELS];
    int table_size;
};

static const char* expr = "";




int Find_Operator (const char* name);
int Find_In_Table_Id (Label_Id* label_id , const char* name , Namel_Label type);
int Find_In_Table_Func (Label_Id* label_id , const char* name , Namel_Label type);
int Find_In_Table_Func_For_Tabel (Label_Id* label_id , const char* name);
int Find_In_Table_Id_For_Table (Label_Id* label_id , const char* name);

char* Read_File_To_Buffer (const char* filename);

void Lexical_Analyzer (const char* expr, Token_Aftomat* token_aftomat , Label_Id* label_id);
void Build_Name_Table (const char* input , Label_Id* label_id);
int Add_Identifier (Label_Id* label_id, const char* name , Namel_Label type);

void Syntax_Error ();
void Print_Name_Table (Label_Id* label_id);
void Print_Token_Array (Token_Aftomat token_aftomat, Label_Id label_id);



void export_tree_to_dot (const char* filename, Token* root);
void print_tree (Token* node, int depth);
Token* parse_expression (Token* tokens, int count, int* index);
Token* parse_term (Token* tokens, int count, int* index);
Token* parse_primary (Token* tokens, int count, int* index);
int match_operator (Token* tokens, int count, int* index, enum Operators op);
int is_end_of_tokens (Token* tokens, int count, int index);
Token* create_token (enum Token_Type type, int value, const char* name);
Token* Parse_Assign (Token* tokens , int count , int* index);
