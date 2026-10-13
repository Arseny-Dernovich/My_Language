#include "Read.h"


static int Evaluate (Token * node) {
    if (node == NULL) return 0;

    switch (node->value) {

        case OP_GOAL:        // +

            return node->left->value + node->right->value;

        case OP_FOUL:        // -

            return node->left->value - node->right->value;

        case OP_ASSIST:      // *

            return node->left->value * node->right->value;

        case OP_SUBSTITUTE:  // /


            if (node->right->value != 0) {

                return node->left->value / node->right->value;
            } else {
                fprintf (stderr , "Ошибка: деление на ноль.\n");
                exit (EXIT_FAILURE);
            }

        default:

            fprintf (stderr , "Ошибка: неизвестная операция для свёртки.\n");
            exit (EXIT_FAILURE);
    }
}


Token * Fold_Constants (Token * node)
{
    // printf ("HUUUIII FOLD");
    if (node == NULL) return NULL;

    node->left = Fold_Constants (node->left);
    node->right = Fold_Constants (node->right);

    if (node->token_type == TOKEN_OPERATOR &&
        node->left && node->right &&
        node->left->token_type == TOKEN_NUMBER &&
        node->right->token_type == TOKEN_NUMBER) {
        if (node->value != OP_GOAL && node->value != OP_FOUL &&
            node->value != OP_ASSIST && node->value != OP_SUBSTITUTE)

            return node;

        int result = Evaluate (node);
        node->token_type = TOKEN_NUMBER;
        node->value = result;
        node->left = NULL;
        node->right = NULL;
    }

    return node;
}


void Remove_Neutral_Elements (Token * node)
{
    if (node == NULL) return;

    if (node->left != NULL) {

        Remove_Neutral_Elements (node->left);
    }

    if (node->right != NULL) {

        Remove_Neutral_Elements (node->right);
    }

    if (node->token_type == TOKEN_OPERATOR) {

        // Удаление нейтральных элементов для сложения (a + 0 = a)
        if (node->value == OP_GOAL) {

            if (node->left && node->left->token_type == TOKEN_NUMBER && node->left->value == 0) {

                Token * old_node = node->right;
                *node = *old_node;
            }

            else if (node->right && node->right->token_type == TOKEN_NUMBER && node->right->value == 0) {
                Token * old_node = node->left;
                *node = *old_node;

            }
        }

        // Удаление нейтрального элемента для умножения (a * 1 = a).
        // 0 * a не заменяем на 0: в a может быть вызов функции с побочными эффектами.


        if (node->value == OP_ASSIST) {


            if (node->left && node->left->token_type == TOKEN_NUMBER) {


                if (node->left->value == 1) {

                    Token * old_node = node->right;
                    *node = *old_node;

                }
            } else if (node->right && node->right->token_type == TOKEN_NUMBER) {


                if (node->right->value == 1) {

                    Token * old_node = node->left;
                    *node = *old_node;

                }
            }
        }
    }
}
