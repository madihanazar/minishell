#include "minishell.h"

int g_sig = 0;

// static t_tree *create_command_node(char *cmd)
// {
//     t_tree *node = malloc(sizeof(t_tree));
//     node->type = NODE_COMMAND;
//     node->cmd = ft_strdup(cmd);
//     node->left = NULL;
//     node->right = NULL;
//     return node;
// }

// static t_tree *create_pipe_node(t_tree *left, t_tree *right)
// {
//     t_tree *node = malloc(sizeof(t_tree));
//     node->type = PIPE;
//     node->cmd = NULL;
//     node->left = left;
//     node->right = right;
//     return node;
// }

// static void test_pipe(char **env, char *test_name, char *cmd1, char *cmd2)
// {
//     printf("\n=== Testing %s ===\n", test_name);
//     printf("Command: %s | %s\n", cmd1, cmd2);

//     t_tree *left = create_command_node(cmd1);
//     t_tree *right = create_command_node(cmd2);
//     t_tree *pipe_node = create_pipe_node(left, right);

//     int result = execute_node(pipe_node, env);
//     printf("Exit status: %d\n", result);

//     // Free the nodes
//     free(left->cmd);
//     free(left);
//     free(right->cmd);
//     free(right);
//     free(pipe_node);
// }

// void test_command(char **env, char *test_name, char *cmd)
// {
//     printf("\n=== Testing %s ===\n", test_name);
//     printf("Command: %s\n", cmd);

//     t_tree *node = create_command_node(cmd);
//     int result = execute_node(node, env);
//     printf("Exit status: %d\n", result);

//     // Free the node
//     free(node->cmd);
//     free(node);
// }

// int main(int argc, char **argv, char **environ)
// {
//     char *cmd;
//     t_tree *ast;
//     int status;

//     if (argc != 2)
//     {
//         printf("Usage: ./test \"command\"\n");
//         return (1);
//     }

//     cmd = argv[1];
//     printf("Executing command: %s\n", cmd);

//     // Create AST from the command using your existing parser
//     ast = create_tree(cmd, environ);
//     if (!ast)
//     {
//         printf("Failed to create AST\n");
//         return (1);
//     }
//     print_ast(ast, 0);
//     // Execute the command tree
//     //printf("node type %s\n",ast->type);
//     status = execute_node(ast, environ);

//     // Clean up
//     //free_tree(ast);

//     return (status);
// }

// t_shell *create_shell(char **env)
// {
//     t_shell *shell;
//     char *str;
//     t_list *node;

//     shell = malloc(sizeof(t_shell));
//     if (!shell)
//         return (NULL);
//     shell->env = NULL;
//     while (*env)
//     {
//         str = ft_strdup(*env);
//         if (!str)
//             return (ft_lstclear(&shell->env, free), free(shell), NULL);
//         node = ft_lstnew(str);
//         if (!node)
//             return (free(str), ft_lstclear(&shell->env, free),
//                     free(shell), NULL);
//         ft_lstadd_back(&shell->env, node);
//         env++;
//     }
//     return (shell);
// }
int main(int argc, char **argv, char **env)
{
    //t_shell *shell;
    char *input;
    int status;
    (void)argc;
    (void)argv;
    //shell = create_shell(env);
    // signal(SIGINT, handle_sigint);   // Ctrl+C
    // signal(SIGQUIT, SIG_IGN);
    char **env_copy = env;
    t_shell *shell;
    shell->export_list = NULL;
    while (1)
    {
        input = readline("minishell>");
        if (!input)
            break;
        t_tree *ast = create_tree(input, env_copy);
        print_ast(ast, 0);
        status = execute_node(ast, &env_copy, shell);
        free(input);
    }
    printf("im out\n");
    return (status);
}

// void handle_sigint(int sig)
// {
//     g_sig = sig;
//     write(1, "\n", 1);
//     rl_on_new_line();
//     rl_replace_line("", 0);
//     rl_redisplay();
// }

// void free_shell(t_shell *shell)
// {
//     if (shell)
//     {
//         if (shell->env)
//             ft_lstclear(&shell->env, free); // Changed from your previous env cleanup
//         // Rest of your cleanup
//         free(shell);
//     }
// }