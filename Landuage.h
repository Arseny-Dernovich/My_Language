
#include "My_features.h"
// #include "TXLib.h"

enum Node_Type {
    NODE_NUMBER    = 1 ,
    NODE_VARIABLE  = 2 ,
    NODE_OPERATION = 3 ,
};

enum Operation_Type {
    BINARY ,
    UNARY
};

enum Math_Operations {
    ADD = '+' ,
    SUB = '-' ,
    MUL = '*' ,
    DIV = '/' ,
    POW = '^' ,
    SIN = 's' ,
    COS = 'c' ,
    LN  = 'L' ,
    SQRT = 'S',
};

struct Tree_Node {
    enum Node_Type type;
    double num_value;
    char var_name;
    char operation;
    Tree_Node* left;
    Tree_Node* right;
};

struct Function {
    char name_in_file;
    const char* real_name;
    const char* latex_name;
    Operation_Type type_operation;
    Tree_Node* (*differentiate) (Tree_Node* node);
};

Tree_Node* Diff_Add (Tree_Node* node);
Tree_Node* Diff_Sub (Tree_Node* node);
Tree_Node* Diff_Mul (Tree_Node* node);
Tree_Node* Diff_Div (Tree_Node* node);
Tree_Node* Diff_Sin (Tree_Node* node);
Tree_Node* Diff_Cos (Tree_Node* node);
Tree_Node* Diff_Ln  (Tree_Node* node);
Tree_Node* Diff_Sqrt (Tree_Node* node);
Tree_Node* Diff_Exp (Tree_Node* node);

static Function functions[] = {
    {'s' , "sin" ,  "\\sin" ,  UNARY , Diff_Sin},
    {'S' , "sqrt" , "\\sqrt" , UNARY , Diff_Sqrt},
    {'c' , "cos" ,  "\\cos" ,  UNARY , Diff_Cos},
    {'L' , "ln" ,   "\\ln" , UNARY , Diff_Ln},
    {'+' , "+" ,    "+" , BINARY , Diff_Add},
    {'-' , "-" ,    "-", BINARY , Diff_Sub},
    {'*' , "*" ,    "\\cdot" , BINARY , Diff_Mul},
    {'/' , "/" ,    "\\frac" , BINARY , Diff_Div},
    {'^' , "^" ,    "^" , BINARY , Diff_Exp}
};

const int functions_count = sizeof (functions) / sizeof (Function);

Tree_Node* Parse_G ();
Tree_Node* Parse_E ();
Tree_Node* Parse_T ();
Tree_Node* Parse_P ();
Tree_Node* Parse_F ();
Tree_Node* Parse_S ();
void PrintTree (/*const*/ Tree_Node* node , int depth);
void Create_Graph (Tree_Node* root);
Function* Find_Function (char op);
void  Generate_Latex (Tree_Node* node , FILE* latex_file);
void Generate_Latex_File (Tree_Node* root , Tree_Node* diff_root , const char* filename);
void Compile_Latex_To_Pdf (const char* latex_filename);
void Generate_Latex_For_Graphic (Tree_Node* node, FILE* latex_file);
void Generate_Graphik(Tree_Node* root, FILE* latex_file);
void Generate_Gnuplot_File (Tree_Node* root, const char* filename);
char* Read_File_To_Buffer (const char* filename);
Tree_Node* Parse_Expression (const char* input);
Tree_Node* New_Node (enum Node_Type type , double value , Tree_Node* left , Tree_Node* right);
Tree_Node* Diff (Tree_Node* node);
void Taylor(Tree_Node* root, int n, FILE* latex_file);
Tree_Node* Copy_Subtree (Tree_Node* node);
Tree_Node* Simplify_Tree (Tree_Node* node);
Tree_Node* Remove_Neutral_Elements (Tree_Node* node);
Tree_Node* Fold_Constants (Tree_Node* node);
double Evaluate (Tree_Node* node);
void Free_Tree (Tree_Node *node);

static const char* expr = "";
static int p = 0; // in cpp
