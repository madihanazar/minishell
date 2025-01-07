#include "minishell.h"

static void run_heredoc(int pipe_fd, char *delimiter)
{
    char *line;

    while (1)
    {
        line = readline("heredoc> ");
        if (!line || ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
        {
            free(line);
            break;
        }
        fprintf(stderr, "%s\n", line);
        write(pipe_fd, line, ft_strlen(line));
        write(pipe_fd, "\n", 1);  // Add newline after each line
        free(line);
    }
}

int execute_heredoc(t_tree *node, char **env)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    if (pipe(pipefd) == -1)
        (perror("Error opening the pipe"), exit(0));

    // First fork for reading heredoc input
    pid1 = fork();
    if (pid1 == 0)
    {
        close(pipefd[0]);
        run_heredoc(pipefd[1], node->right->cmd);
        close(pipefd[1]);
        exit(0);
    }
    waitpid(pid1, NULL, 0);

    // Second fork for executing the command
    pid2 = fork();
    if (pid2 == 0)
    {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        exit(execute_node(node->left, env));
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid2, &status, 0);
    return (WEXITSTATUS(status));
}

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