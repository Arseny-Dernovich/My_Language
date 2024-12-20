#include "Read.h"
#include "Back_end.h"

void Free_Tree (Token* node)
{
    if (node == NULL) return;
    Free_Tree (node->left);
    Free_Tree (node->right);
    free (node);
}


int main (int argc , char** argv)
{
    const char* filename = "my_programm.txt";
    if (argc == 2)
        filename = argv[1];
    // Читаем входной файл
    char* input = Read_File_To_Buffer (filename);
    if (!input) {

        printf ("Failed to read file: %s\n" , filename);
        return EXIT_FAILURE;
    }
    printf ("%s\n" , input);

    Label_Id label_id = {};
// fprintf(stderr, "porno\n");
// fprintf(stderr, "label size in main = %d\n\n", label_id.table_size);
    Build_Name_Table (input , &label_id);

    Print_Name_Table (&label_id);

    Token_Aftomat token_aftomat = {};
    Lexical_Analyzer (input , &token_aftomat , &label_id);

    Print_Token_Array (token_aftomat , label_id);


    int index = 0;
    Token* tree = Parse_Programm (token_aftomat.aftomat_tokens , token_aftomat.token_size , &index);
    ProcessTree (tree , &label_id);
    Fold_Constants (tree);
    Remove_Neutral_Elements (tree);
    PrintLocalNameTables (tree);
    Export_Tree_To_Dot ("tree.dot" , &label_id ,  tree);

    // Print_Token_Array (token_aftomat , label_id);


    FILE* file = fopen("output.asm", "w");
    if (file == NULL) {
    perror("Error opening file");
    exit(1);
}
//
    Generate_Asm_Function (tree->left , file , &label_id);

    fclose (file);

//     Free_Tree(tree);         // TODO:
    free (input);
    for (int i = 0; i < label_id.table_size; i++) {

        free (label_id.table_label[i].name_identifier);
    }
    printf ("Программа завершенна");

    return 0;
}
