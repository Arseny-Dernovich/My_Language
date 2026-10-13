#include "Read.h"
#include "Back_end.h"

static int Compile_Program (const char * filename);




int main (int argc , char ** argv)
{
    const char * filename = argc > 1 ? argv[1] : "my_programm.txt";

    return Compile_Program (filename);
}




static void Free_Name_Table (Label_Id * label_id)
{
    for (int i = 0; i < label_id->table_size; i++) {

        free (label_id->table_label[i].name_identifier);
    }
}


static int Compile_Program (const char * filename)
{
    char * input = Read_File_To_Buffer (filename);

    if (input == NULL) {

        fprintf (stderr , "Failed to read file: %s\n" , filename);

        return EXIT_FAILURE;
    }

    Label_Id label_id = {};
    Build_Name_Table (input , &label_id);

    Token_Aftomat token_aftomat = {};
    Lexical_Analyzer (input , &token_aftomat , &label_id);

    int index = 0;
    Token * tree = Parse_Programm (token_aftomat.aftomat_tokens , token_aftomat.token_size , &index);

    Program_Context context = {};
    ProcessTree (tree , &context , &label_id);
    Fold_Constants (tree);
    Remove_Neutral_Elements (tree);
    Export_Tree_To_Dot ("tree.dot" , &label_id , tree);

    FILE * asm_file = fopen ("output.asm" , "w");
    int result = EXIT_SUCCESS;

    if (asm_file == NULL) {

        perror ("Error opening output.asm");
        result = EXIT_FAILURE;

    }

    else {
        Generate_Asm_Program (tree , asm_file , &context);
        fclose (asm_file);
        printf ("Программа завершена\n");
    }

    Free_Scopes (tree);
    Free_Name_Table (&label_id);
    free (input);

    return result;
}
