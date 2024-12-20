
struct Back_end_tree {
    Token* node;
    int n_if;
    int n_while;
};










void Generate_Asm_Assign (Token* node, FILE* file , Label_Id* label_id);
void Generate_Asm_Print (Token* node , FILE* asm_file , Label_Id* label_id);
void Generate_Asm_Expression(Token* node, FILE* asm_file , Label_Id* label_id);
void Generate_Asm_Initialization(Token* node, FILE* asm_file, Label_Id* label_id);
void Generate_Asm_Multiple_Op(Token* node, FILE* asm_file , Label_Id* label_id);
void Generate_Asm_Operations(Token* node, FILE* asm_file , Label_Id* label_id);
void Generate_Asm_Condition (Token* node, FILE* asm_file , Label_Id* label_id , const char* if_or_while);
void Generate_Asm_If_While_Body(Token* node, FILE* asm_file, Label_Id* label_id, const char* if_or_while);
void Generate_Asm_If_Statement(Token* node, FILE* asm_file, Label_Id* label_id);
void Generate_Asm_While_Statement(Token* node, FILE* asm_file, Label_Id* label_id);
void Generate_Asm_Call_Func (Token* node , FILE* asm_file , Label_Id* label_id);
void Generate_Asm_Param_Call_Func (Token* node , FILE* asm_file , Label_Id* label_id);
void Generate_Asm_Return_Value(Token* node, FILE* asm_file, Label_Id* label_id);
void Generate_Asm_Function (Token* node , FILE* asm_file , Label_Id* label_id);
