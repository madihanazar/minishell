#include "minishell.h"

void clear_heredoc_list(t_heredoc **heredoc_list) {
    t_heredoc *current = *heredoc_list;
    t_heredoc *next;

    while (current) {
        next = current->next;
        close(current->node->heredoc_fd); // Close the file descriptor
        free(current); // Free the heredoc node
        current = next;
    }
    *heredoc_list = NULL; // Reset the list pointer
}
static void display_heredoc_content(int fd) {
    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        write(STDOUT_FILENO, buffer, bytes_read); // Write to standard output
    }
}

int process_heredocs(t_heredoc *heredoc_list, char ***env, t_shell *shell, int execute_flag) {
    t_heredoc *current;
    char *line;
    int fd[2];
    pid_t pid;
    int status;

    if (!heredoc_list)
        return 0;

    // Create a single pipe for all heredocs
    // if (pipe(fd) == -1)
    //     return (perror("pipe error"), 1);

    current = heredoc_list;
    while (current) {
        if (pipe(fd) == -1)
            return (perror("pipe error"), 1);
        pid = fork();
        if (pid == -1) {
            close(fd[0]);
            close(fd[1]);
            return (perror("fork error"), 1);
        }

        if (pid == 0) {
            close(fd[0]); // Close read end in child
            while (1) {
                line = readline("> ");
                if (!line) // EOF (Ctrl+D)
                    break;
                if (ft_strcmp(line, current->node->right->cmd) == 0) {
                    free(line);
                    break;
                }
                write(fd[1], line, ft_strlen(line));
                write(fd[1], "\n", 1);
                free(line);
            }
            close(fd[1]); // Close write end before exiting
            exit(0);
        }

        waitpid(pid, &status, 0);

        if (WIFSIGNALED(status)) {
            close(fd[0]); // Close the pipe
            close(fd[1]);
            return 1;
        }

        current->node->heredoc_fd = fd[0];
        current = current->next;
    }

    close(fd[1]); // Close write end in parent

    // Display the combined heredoc content
    // display_heredoc_content(fd[0]);
    printf("done usingheredpc\n");
    if (execute_flag) {
        t_tree *cmd_node = heredoc_list->node;
        while (cmd_node && cmd_node->type != NODE_COMMAND)
            cmd_node = cmd_node->left;

        if (cmd_node && cmd_node->type == NODE_COMMAND) {
            cmd_node->heredoc_fd = fd[0]; // Pass the combined heredoc fd to the command
            printf("cmd node %s\n", cmd_node->cmd);
            int result = execute_node(cmd_node, env, shell);
            close(cmd_node->heredoc_fd); // Close the heredoc fd after execution
            return result;
        }
    }

    close(fd[0]); // Close the read end if not executing
    return 0;
}

// int process_heredocs(t_heredoc *heredoc_list, char ***env, t_shell *shell, int execute_flag) {
//     t_heredoc *current;
//     char *line;
//     int fd[2];
//     pid_t pid;
//     int status;
//     t_heredoc *last = NULL;

//     if (!heredoc_list)
//         return 0;

//     current = heredoc_list;
//     while (current) {
//         if (pipe(fd) == -1)
//             return (perror("pipe error"), 1);

//         pid = fork();
//         if (pid == -1) {
//             close(fd[0]);
//             close(fd[1]);
//             return (perror("fork error"), 1);
//         }

//         if (pid == 0) {
//             close(fd[0]); // Close read end in child
//             while (1) {
//                 line = readline("> ");
//                 if (!line) // EOF (Ctrl+D)
//                     break;
//                 if (ft_strcmp(line, current->node->right->cmd) == 0) {
//                     free(line);
//                     break;
//                 }
//                 write(fd[1], line, ft_strlen(line));
//                 write(fd[1], "\n", 1);
//                 free(line);
//             }
//             close(fd[1]); // Close write end before exiting
//             exit(0);
//         }

//         close(fd[1]); // Close write end in parent
//         current->node->heredoc_fd = fd[0]; // Save read end for later use
//         last = current;
//         waitpid(pid, &status, 0);

//         if (WIFSIGNALED(status)) {
//             close(fd[0]); // Close the current heredoc fd
//             // Close all previous heredoc fds
//             t_heredoc *temp = heredoc_list;
//             while (temp != current) {
//                 close(temp->node->heredoc_fd);
//                 temp = temp->next;
//             }
//             return 1;
//         }

//         current = current->next;
//     }

//     if (execute_flag && last) {
//         t_tree *cmd_node = heredoc_list->node;
//         while (cmd_node && cmd_node->type != NODE_COMMAND)
//             cmd_node = cmd_node->left;

//         if (cmd_node && cmd_node->type == NODE_COMMAND) {
//             cmd_node->heredoc_fd = last->node->heredoc_fd;
//             int result = execute_node(cmd_node, env, shell);
//             close(cmd_node->heredoc_fd); // Close the heredoc fd after execution
//             return result;
//         }
//     }
//     //clear_heredoc_list(&heredoc_list);

//     return 0;
// }

// int process_heredocs(t_heredoc *heredoc_list, char ***env, t_shell *shell, int execute_flag)
// {
//     t_heredoc *current;
//     char *line;
//     int fd[2];
//     pid_t pid;
//     int status;
//     t_heredoc *last = NULL;

//     if (!heredoc_list)
//         return (0);

//     current = heredoc_list;
//     while (current)
//     {
//         if (pipe(fd) == -1)
//             return (perror("pipe error"), 1);

//         pid = fork();
//         if (pid == -1)
//             return (perror("fork error"), 1);

//         if (pid == 0)
//         {
//             close(fd[0]); // Close read end in child

//             while (1)
//             {
//                 line = readline("> ");
//                 if (!line) // EOF (Ctrl+D)
//                 {
//                     break;
//                 }
//                 if (ft_strcmp(line, current->node->right->cmd) == 0)
//                 {
//                     free(line);
//                     break;
//                 }

//                 // Write the line to pipe
//                 write(fd[1], line, ft_strlen(line));
//                 write(fd[1], "\n", 1);
//                 free(line);
//             }
//             close(fd[1]);
//             exit(0);
//         }
//         close(fd[1]);                      // Close write end in parent
//         current->node->heredoc_fd = fd[0]; // Save read end for later use
//         last = current;  
//         waitpid(pid, &status, 0);

//         // if (WIFSIGNALED(status))
//         // {
//         //     close(fd[0]);
//         //     // Close all previous heredoc fds
//         //     t_heredoc *temp = heredoc_list;
//         //     while (temp != current)
//         //     {
//         //         close(temp->node->heredoc_fd);
//         //         temp = temp->next;
//         //     }
//         //     return (1);
//         // }
//         current = current->next;
//     }

//     printf("im done\n");
//     // if (execute_flag && last)
//     // {
//     //     // Check if the heredoc is part of a redirection
//     //     // if (heredoc_list->node->right &&
//     //     //     (heredoc_list->node->right->type == REDIR_OUT ||
//     //     //      heredoc_list->node->right->type == REDIR_IN ||
//     //     //      heredoc_list->node->right->type == APPEND))
//     //     // {
//     //     //     return execute_node(heredoc_list->node->right, env, shell);
//     //     // }
//     //     // If it's a standalone heredoc
//     //     // If we should execute and we have heredocs
//     //     printf("im inside\n");
//     //     t_tree *cmd_node = heredoc_list->node; // Start at the first heredoc node (abc)
//     //     printf("node %s\n", cmd_node->left->cmd);
//     //     while (cmd_node && cmd_node->type != NODE_COMMAND)
//     //         cmd_node = cmd_node->left;

//     //     if (cmd_node && cmd_node->type == NODE_COMMAND)
//     //     {
//     //         // Set the command's heredoc_fd to the last heredoc's fd
//     //         cmd_node->heredoc_fd = last->node->heredoc_fd;
//     //         return execute_node(cmd_node, env, shell);
//     //     } // This will eventually reach 'cat'

//     //     return execute_node(cmd_node, env, shell); // Execute the cat command
//     // }
//     if (execute_flag && last)
//     {
//         // Find the command node
//         t_tree *cmd_node = heredoc_list->node;
//         while (cmd_node && cmd_node->type != NODE_COMMAND)
//             cmd_node = cmd_node->left;
            
//         if (cmd_node && cmd_node->type == NODE_COMMAND)
//         {
//             // Set the command's heredoc_fd to the last heredoc's fd
//             cmd_node->heredoc_fd = last->node->heredoc_fd;
//             return execute_node(cmd_node, env, shell);
//         }
//     }

//     return (0);
// }
// int process_heredocs(t_heredoc *list, char ***env, t_shell *shell, int flag)
// {
//     int pipefd[2];
//     pid_t pid;
//     int status;
//     t_heredoc *curr = list;
//     char *delimiter = curr->right->cmd;

//     if (pipe(pipefd) == -1)
//         return (perror("minishell: pipe"), 1);

//     pid = fork();
//     if (pid == 0)
//     {
//         close(pipefd[0]);
//         char *line;

//         while (1)
//         {
//             line = readline("> ");
//             if (!line)
//             {
//                 ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", 2);
//                 break;
//             }
//             if (ft_strcmp(line, delimiter) == 0)
//             {
//                 free(line);
//                 break;
//             }
//             write(pipefd[1], line, strlen(line));
//             write(pipefd[1], "\n", 1);
//             free(line);
//         }
//         close(pipefd[1]);
//         exit(0);
//     }

//     close(pipefd[1]);
//     // node->left->heredoc_fd = pipefd[0];
//     //node->heredoc_fd = pipefd[0];
//     //waitpid(pid, &status, 0);

//     //return execute_node(node->left, env, shell);
//     if (!flag)
//     {
//         node->heredoc_fd = pipefd[0];
//         waitpid(pid, &status, 0);
//         return (0);
//     }
//     node->left->heredoc_fd = pipefd[0];
//     waitpid(pid, &status, 0);
//     return execute_node(node->left, env, shell);
// }

// int process_heredocs(t_heredoc *heredoc_list, char ***env, t_shell *shell, int execute_flag)
// {
//     t_heredoc *current;
//     char *line;
//     int fd[2];
//     pid_t pid;
//     int status;

//     if (!heredoc_list)
//         return (0);

//     current = heredoc_list;
//     printf("current node in list %s\n", current->node->right->cmd);
//     while (current)
//     {
//         if (pipe(fd) == -1)
//             return (perror("pipe error"), 1);

//         pid = fork();
//         if (pid == -1)
//             return (perror("fork error"), 1);

//         if (pid == 0)
//         {
//             close(fd[0]);  // Close read end in child
//             //signal(SIGINT, SIG_DFL);  // Reset SIGINT handling in child

//             while (1)
//             {
//                 line = readline("> ");
//                 if (!line)  // EOF (Ctrl+D)
//                 {
//                     close(fd[1]);
//                     exit(0);
//                 }

//                 if (ft_strcmp(line, current->node->right->cmd) == 0)
//                 {
//                     free(line);
//                     close(fd[1]);
//                     exit(0);
//                 }

//                 // Write the line to pipe
//                 write(fd[1], line, ft_strlen(line));
//                 write(fd[1], "\n", 1);
//                 free(line);
//             }
//         }
//         else
//         {
//             close(fd[1]);  // Close write end in parent
//             current->node->heredoc_fd = fd[0];  // Save read end for later use
//             waitpid(pid, &status, 0);

//             // Check if heredoc was interrupted
//             if (WIFSIGNALED(status))
//             {
//                 close(fd[0]);
//                 return (1);
//             }
//         }
//         current = current->next;
//     }

//     // If execute_flag is true and all heredocs processed successfully,
//     // execute the command with first heredoc's content as input
//     if (execute_flag && heredoc_list)
//     {
//         return execute_node(heredoc_list->node->left, env, shell);
//     }

//     return (0);
// }
// int process_heredocs(t_heredoc *heredoc_list, char ***env, t_shell *shell, int flag)
// {
//     t_heredoc *current;
//     int ret;

//     current = heredoc_list;
//     // Process each heredoc in order (left to right)
//     while (current)
//     {
//         ret = process_heredoc(current->node, flag);
//         if (ret != 0)
//             return ret;
//         current = current->next;
//     }

//     // If this is a normal heredoc (not redirection), execute command
//     if (flag && heredoc_list)
//     {
//         // Get the leftmost command node
//         t_tree *cmd_node = heredoc_list->node;
//         while (cmd_node->left && cmd_node->left->type == NODE_COMMAND)
//             cmd_node = cmd_node->left;

//         return execute_node(cmd_node, env, shell);
//     }
//     return 0;
// }
// int process_heredocs(t_tree *node, char ***env, t_shell *shell, int flag)
// {
//     int pipefd[2];
//     pid_t pid;
//     int status;
//     char *delimiter = node->right->cmd;

//     if (pipe(pipefd) == -1)
//         return (perror("minishell: pipe"), 1);

//     pid = fork();
//     if (pid == 0)
//     {
//         close(pipefd[0]);
//         char *line;

//         while (1)
//         {
//             line = readline("> ");
//             if (!line)
//             {
//                 ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", 2);
//                 break;
//             }
//             if (ft_strcmp(line, delimiter) == 0)
//             {
//                 free(line);
//                 break;
//             }
//             write(pipefd[1], line, strlen(line));
//             write(pipefd[1], "\n", 1);
//             free(line);
//         }
//         close(pipefd[1]);
//         exit(0);
//     }

//     close(pipefd[1]);
//     // node->left->heredoc_fd = pipefd[0];
//     //node->heredoc_fd = pipefd[0];
//     //waitpid(pid, &status, 0);

//     //return execute_node(node->left, env, shell);
//     if (!flag)
//     {
//         node->heredoc_fd = pipefd[0];
//         waitpid(pid, &status, 0);
//         return (0);
//     }
//     node->left->heredoc_fd = pipefd[0];
//     waitpid(pid, &status, 0);
//     return execute_node(node->left, env, shell);
// }

int process_multiple_heredocs(t_tree *node, char ***env, t_shell *shell, int flag)
{
    t_tree *current = node;
    t_tree *head = node;

    while (current->left && current->left->type == HEREDOC)
        current = current->left;
    printf("%s", current->right->cmd);
    while (current && current->type == HEREDOC)
    {
        int pipefd[2];
        pid_t pid;
        int status;
        char *delimiter = current->right->cmd;

        if (pipe(pipefd) == -1)
            return (perror("minishell: pipe"), 1);

        pid = fork();
        if (pid == 0)
        {
            close(pipefd[0]);
            char *line;

            while (1)
            {
                line = readline("> ");
                if (!line)
                {
                    ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", 2);
                    break;
                }
                if (ft_strcmp(line, delimiter) == 0)
                {
                    free(line);
                    break;
                }
                write(pipefd[1], line, strlen(line));
                write(pipefd[1], "\n", 1);
                free(line);
            }
            close(pipefd[1]);
            exit(0);
        }

        close(pipefd[1]);
        if (!flag)
            current->heredoc_fd = pipefd[0];
        else
            current->left->heredoc_fd = pipefd[0];
        waitpid(pid, &status, 0);

        current = current->right;
    }
    // After processing all heredocs, execute the command if flag is set
    if (flag)
        return execute_node(head->left, env, shell);
    return 0;
}

// int process_heredocs(t_tree *node, char ***env, t_shell *shell)
// {
//     int pipefd[2];
//     char *line;
//     char *delimiter;
//     printf("im inside the heredoc\n");
//     delimiter = node->right->cmd;
//     if (pipe(pipefd) == -1)
//         return (perror("minishell: pipe"), 1);

//     while (1)
//     {
//         line = readline("> ");
//         if (!line)
//         {
//             ft_putstr_fd("minishell: warning: here-document delimited by end-of-file\n", 2);
//             break;
//         }
//         if (ft_strcmp(line, delimiter) == 0)
//         {
//             free(line);
//             break;
//         }
//         write(pipefd[1], line, strlen(line));
//         write(pipefd[1], "\n", 1);
//         free(line);
//     }
//     close(pipefd[1]); // Close write end after reading all input

//     // Assign the read end to the left node's heredoc_fd
//     node->left->heredoc_fd = pipefd[0];

//     // Execute the left node (command) with heredoc redirection
//     return (execute_node(node->left, env, shell));
// }

// void run_heredoc(int pipe_fd, char *delimiter)
// {
//     char *line;

//     while (1)
//     {
//         line = readline("> ");
//         if (!line || ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
//         {
//             free(line);
//             break;
//         }
//         //fprintf(stderr, "%s\n", line);
//         write(pipe_fd, line, ft_strlen(line));
//         write(pipe_fd, "\n", 1);  // Add newline after each line
//         free(line);
//     }
// }

// int execute_heredoc(t_tree *node, char ***env, t_shell *shell)
// {
//     int pipefd[2];
//     pid_t pid1, pid2;
//     int status;

//     if (pipe(pipefd) == -1)
//         (perror("Error opening the pipe"), exit(0));

//     // First fork for reading heredoc input
//     pid1 = fork();
//     if (pid1 == 0)
//     {
//         close(pipefd[0]);
//         run_heredoc(pipefd[1], node->right->cmd);
//         close(pipefd[1]);
//         exit(0);
//     }
//     close(pipefd[1]);
//     waitpid(pid1, &status, 0);
//     // Second fork for executing the command
//     // pid2 = fork();
//     // if (pid2 == 0)
//     // {
//     //     close(pipefd[1]);
//     //     dup2(pipefd[0], STDIN_FILENO);
//     //     close(pipefd[0]);
//     //     exit(execute_node(node->left, env, ));
//     // }

//     // node->heredoc_fd = pipefd[0];
//     dup2(pipefd[0], STDIN_FILENO);
//     close(pipefd[0]);
//     // close(pipefd[1]);
//     // waitpid(pid1, NULL, 0);
//     //waitpid(pid2, &status, 0);
//     //return (WEXITSTATUS(status));
//     return execute_node(node->left, env, shell);
// }
// int has_heredoc(t_tree *node)
// {
//     if (!node)
//         return (0);
//     if (node->type == HEREDOC)
//         return (1);
//     return (has_heredoc(node->left) || has_heredoc(node->right));
// }
// int process_heredocs(t_tree *node, char ***env)
// {
//     if (!node)
//         return (0);

//     if (node->type == HEREDOC)
//         return (execute_heredoc(node, env));

//     int left_status = process_heredocs(node->left, env);
//     if (left_status != 0)
//         return (left_status);

//     return process_heredocs(node->right, env);
// }

// int process_heredocs(t_tree *node, char ***env)
// {
//     int status;
//     if (!node)
//         return (0);

//     if (node->left && node->left->type == HEREDOC)
//         status = process_heredocs(node->left, env);

//     if (node->type == HEREDOC)
//     {
//         printf("yes im executing\n");
//         execute_heredoc(node, env);
//     }
//     //execute_node(node->left, env);

//     return (status);
// }

// int process_heredocs(t_tree *node, char ***env, t_shell *shell)
// {
//     if (!node)
//         return (0);

//     if (node->type == HEREDOC)
//         return execute_heredoc(node, env, shell);

//     int left_status = process_heredocs(node->left, env, shell);
//     if (left_status != 0)
//         return left_status;

//     return process_heredocs(node->right, env, shell);
// }

// static void run_heredoc(int pipe_fd, char *cmd, char **env)
// {
//     char *line;
//     while (1) {
//         line = readline(">");
//         while (!line && ft_strncmp(line, cmd, ft_strlen(cmd) + 1) == 0)
//         {
//             free(line);
//             break;
//         }
//         write(pipe_fd, line, ft_strlen(line));
//         write(pipe_fd, "\n", 1);
//         free(line);
//         line = readline(">");
// }
// }

// int execute_heredoc(t_tree *node, char **env)
// {
//     int pipefd[2];
//     pid_t pid1;
//     pid_t pid2;
//     int status;

//     if (pipe(pipefd) == -1)
//         (perror("Error opening the pipe"), exit(0));
//     pid1 = fork();
//     if (pid1 == 0)
//     {
//         close(pipefd[0]);
//         //dup2(pipefd[1], STDOUT_FILENO);
//         //close(pipefd[1]);
//         run_heredoc(pipefd[1], node->right->cmd, env);
//         close(pipefd[1]);
//         exit(0);
//     }
//     pid2 = fork();
//     if (pid2 == 0)
//     {
//         close(pipefd[1]);
//         dup2(pipefd[0], STDIN_FILENO);
//         close(pipefd[0]);
//         exit(execute_node(node->left, env));
//     }
//     close(pipefd[0]);
//     close(pipefd[1]);
//     waitpid(pid1, NULL, 0);
//     waitpid(pid2, &status, 0);
//     return (WEXITSTATUS(status));
// }