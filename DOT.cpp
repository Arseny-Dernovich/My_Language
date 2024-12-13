#include "Read.h"

void print_tree(Token* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    if (node->name) printf("%s\n", node->name);
    else printf("%d\n", node->value);
    print_tree(node->left, depth + 1);
    print_tree(node->right, depth + 1);
}


void write_tree_to_dot(FILE* file, Token* node, int* node_id) {
    if (!node) return;

    int current_id = (*node_id)++;
    fprintf(file, "  node%d [label=\"", current_id);

    if (node->token_type == TOKEN_NUMBER) {
        fprintf(file, "%d", node->value);
    } else if (node->token_type == TOKEN_IDENTIFIER) {
        fprintf(file, "%s", node->name);
    } else if (node->token_type == TOKEN_OPERATOR) {
        fprintf(file, "%s", node->name);
    } else {
        fprintf(file, "Unknown");
    }

    fprintf(file, "\"];\n");

    if (node->left) {
        int left_id = *node_id;
        write_tree_to_dot(file, node->left, node_id);
        fprintf(file, "  node%d -> node%d;\n", current_id, left_id);
    }

    if (node->right) {
        int right_id = *node_id;
        write_tree_to_dot(file, node->right, node_id);
        fprintf(file, "  node%d -> node%d;\n", current_id, right_id);
    }
}

// Функция для создания файла .dot и генерации изображения
void export_tree_to_dot(const char* filename, Token* root) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "digraph Tree {\n");
    fprintf(file, "  node [shape=ellipse];\n");

    int node_id = 0;
    write_tree_to_dot(file, root, &node_id);

    fprintf(file, "}\n");
    fclose(file);

    // Генерация изображения через Graphviz с помощью system
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng %s -o tree.png", filename);
    system(command);

    printf("Tree exported to tree.png.\n");
}
