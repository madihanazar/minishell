#include "minishell.h"

//void add_heredoc(t_heredoc **head, char *delimiter, char *content);
//void free_heredocs(t_heredoc *head);
//char *read_heredoc_content(char *delimiter);
void add_heredoc_to_list(t_heredoc **list, t_tree *node);

void add_heredoc_to_list(t_heredoc **list, t_tree *node)
{
    t_heredoc *new;
    t_heredoc *current;

    new = malloc(sizeof(t_heredoc));
    new->node = node;
    new->next = NULL;

    if (!*list)
    {
        *list = new;
        return;
    }
    current = *list;
    while (current->next)
        current = current->next;
    current->next = new;
}
// Add a heredoc to the list
// void add_heredoc(t_heredoc **head, char *delimiter, char *content) {
//     t_heredoc *new_node = malloc(sizeof(t_heredoc));
//     new_node->delimiter = strdup(delimiter);
//     new_node->content = strdup(content);
//     new_node->next = *head;
//     *head = new_node;
// }

// Free the heredoc list
// void free_heredocs(t_heredoc *head) {
//     t_heredoc *tmp;
//     while (head) {
//         tmp = head;
//         head = head->next;
//         free(tmp->delimiter);
//         free(tmp->content);
//         free(tmp);
//     }
// }

// // Read heredoc content from user input
// char *read_heredoc_content(char *delimiter) {
//     char *content = NULL;
//     char *line = NULL;
//     size_t len = 0;
//     printf("> ");
//     while (getline(&line, &len, stdin) != -1) {
//         if (strcmp(line, delimiter) == 0) {
//             free(line);
//             break;
//         }
//         content = realloc(content, (content ? strlen(content) : 0) + strlen(line) + 1);
//         strcat(content, line);
//         printf("> ");
//     }
//     return content;
// }

// void extract_heredocs(t_tree *node, t_heredoc **heredocs) {
//     if (!node)
//         return;

//     // Check if the current node is a heredoc
//     if (node->type == HEREDOC) {
//         char *content = read_heredoc_content(node->cmd);
//         add_heredoc(heredocs, node->cmd, content);
//         free(content);
//     }

//     // Recursively process left and right subtrees
//     extract_heredocs(node->left, heredocs);
//     extract_heredocs(node->right, heredocs);
// }