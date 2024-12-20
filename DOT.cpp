#include "Read.h"

void Print_Tree (Token* node , int depth , Label_Id* label_id)
{
    if (!node) {
     fprintf (stderr , "\nGGGGG\n");
     return;
    }
    for (int i = 0; i < depth; i++) printf ("  ");

    if (node->token_type == TOKEN_NUMBER) {
        printf ("%d\n" , node->value);


    } else if (node->token_type == TOKEN_IDENTIFIER) {
        printf ("%s\n" , label_id->table_label[node->value].name_identifier);

    } else if (node->token_type == TOKEN_OPERATOR) {
        printf ("%s\n" , words_operators[node->value].real_name);

    } else if (node->token_type == TOKEN_KEY_WORD) {
        printf ("%s\n" , words_operators[node->value].real_name);

    } else if (node->token_type == TOKEN_FUNCTION) {
        printf ("%s\n" , label_id->table_label[node->value].name_identifier);

    } else {
        printf ("Unknown");

    }

    Print_Tree (node->left , depth + 1 , label_id);
    Print_Tree (node->right , depth + 1 , label_id);
}


void Write_Tree_To_Dot (FILE* file , Token* node , Label_Id* label_id , int* node_id)
{
    if (!node) {
        // fprintf (stderr , "\nGGGGG\n");
        return;
    }

    int current_id = (*node_id)++;
    // printf ("\n\n@@HUUUUIIIIIII node_value = %d\n\n" , node->token_type);
    fprintf (file , "  node%d [label=\"" , current_id);

    const char* color = "black";

    if (node->token_type == TOKEN_NUMBER) {

        fprintf (file , "%d" , node->value);
        color = "bisque";

    } else if (node->token_type == TOKEN_IDENTIFIER) {

        fprintf (file , "%s" , label_id->table_label[node->value].name_identifier);
        color = "green";

    } else if (node->token_type == TOKEN_OPERATOR) {

        fprintf (file , "%s" , words_operators[node->value].real_name);
        fprintf (stderr , "%s" , words_operators[node->value].real_name);
        color = "aqua";

    } else if (node->token_type == TOKEN_NEW_BLOCK) {

        fprintf (file , "%s" , words_operators[node->value].real_name);
        color = "pink";

    } else if (node->token_type == TOKEN_KEY_WORD) {

        if (node->value == FORSA_BARSA) {

            fprintf (file , "%s" , words_operators[node->value].real_name);
            color = "black";
        }

        else {

            fprintf (file , "%s" , words_operators[node->value].real_name);
            color = "hotpink";
        }

    } else if (node->token_type == TOKEN_FUNCTION) {

        fprintf (file , "%s" , label_id->table_label[node->value].name_identifier);
        color = "khaki1";

    } else {

        fprintf (file , "Unknown");
        color = "gray";
    }

    if (node->token_type == TOKEN_KEY_WORD && node->value == FORSA_BARSA)
        fprintf (file , "\" , style=filled ,fontcolor=white , fillcolor=%s];\n" , color);

    else
        fprintf (file , "\" , style=filled , fillcolor=%s];\n" , color);

    if (node->left) {
        int left_id = *node_id;
        Write_Tree_To_Dot (file , node->left , label_id , node_id);
        fprintf (file , "  node%d -> node%d;\n" , current_id , left_id);
    }

    if (node->right) {
        int right_id = *node_id;
        Write_Tree_To_Dot (file , node->right ,label_id , node_id);
        fprintf (file , "  node%d -> node%d;\n" , current_id , right_id);
    }
}


void Export_Tree_To_Dot (const char* filename , Label_Id* label_id ,  Token* root)
{
    FILE* file = fopen (filename , "w");
    if (!file) {

        perror ("Error opening file");
        exit (EXIT_FAILURE);
    }

    fprintf (file , "digraph Tree {\n");
    fprintf (file , "bgcolor = \"darkblue\"\n");
    fprintf (file , "edge [color = \"white\"]\n");
    fprintf (file , "  node [shape=ellipse];\n");

    int node_id = 0;
    Write_Tree_To_Dot (file , root , label_id ,  &node_id);

    fprintf (file , "}\n");
    fclose (file);

    // Генерация изображения через Graphviz с помощью system
    char command[256] = "";
    snprintf (command , sizeof (command) , "dot -Tpng %s -o tree.png" , filename);
    system (command);

    printf ("Tree#   exported to tree.png.\n");
}
