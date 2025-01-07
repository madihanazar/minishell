#include "minishell.h"

int execute_pipe(t_tree *node, char **env)
{
    pid_t pid1;
    pid_t pid2;
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("Error opening the pipe");
        return (1);
    }
    
    pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork1 failed");
        return (1);
    }
    if (pid1 == 0)
    {
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 failed in first child");
            exit(1);
        }
        close(pipefd[1]);
        int result = execute_node(node->left, env);  // Add debug print INSIDE execute_node
        exit(result);
    }
    
    pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork2 failed");
        return (1);
    }
    if (pid2 == 0)
    {
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            perror("dup2 failed in second child");
            exit(1);
        }
        close(pipefd[0]);
        int result = execute_node(node->right, env);
        exit(result);
    }
    
    //printf("Parent process closing pipe\n");
    close(pipefd[0]);
    close(pipefd[1]);
    
    int status1, status2;
    //printf("Waiting for first child\n");
    waitpid(pid1, &status1, 0);
    //printf("First child exited with status: %d\n", WEXITSTATUS(status1));
    
    //printf("Waiting for second child\n");
    waitpid(pid2, &status2, 0);
    //printf("Second child exited with status: %d\n", WEXITSTATUS(status2));
    
    return WEXITSTATUS(status2);
}

int execute_redir(t_tree *node, char **env)
{
    int fd;
    pid_t pid;

    if (!node)
        return (1);
    if (node->type == REDIR_IN)
        fd = open(node->right->cmd, O_RDONLY);
    else if (node->type == REDIR_OUT)
        fd = open(node->right->cmd, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // else if (node->type == HEREDOC)
    // {
    //     // to do 
    // }
    else if (node->type == APPEND)
     {
       fprintf(stderr, "im inside append\n");
       fprintf(stderr, "right cmd %s\n",node->right->cmd);
       fd = open(node->right->cmd, O_WRONLY | O_CREAT | O_APPEND, 0644);
     }  
    if (fd < 0)
        return (1);
    pid = fork();
    if (pid == 0)
    {
        if (node->type == REDIR_IN)
            dup2(fd, STDIN_FILENO);
        else
            dup2(fd, STDOUT_FILENO);
        close(fd);
        execute_node(node->left, env);
    }
    close(fd);
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}


char *join_path(char *path, char *args)
{
	char *full_cmd;
	char *temp;

	temp = ft_strjoin(path, "/"); // Append '/' to path
	if (temp == NULL)
		return (NULL);
    full_cmd = ft_strjoin(temp, args); 
	free(temp);
	return (full_cmd);
}

char	*extract_path(char *envp[], char *args)
{
	int		i;
	char	**paths;
	char 	*full_cmd;
	
	i = 0;
	while (envp[i] != NULL && (ft_strncmp(envp[i], "PATH=", 5) != 0))
		i++;
	if (envp[i] == NULL)
		return (NULL);
	paths = ft_split(envp[i] + 5, ':');
	if (paths == NULL)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_cmd = join_path(paths[i], args); 
		if (full_cmd == NULL)
			return (free_split(paths), NULL);
		if (access(full_cmd, X_OK) == 0)
			return (free_split(paths), full_cmd);
		free(full_cmd);
		i++;
	}
	return (free_split(paths), NULL);
}

int execute_command(t_tree *node, char **env)
{
    // if (is_builtin(node->cmd))
    //     return execute_builtin(node, env);
        
    //fprintf(stderr, "DEBUG - execute_command START - cmd: %s\n", node->cmd ? node->cmd : "NULL");
    if (!node->cmd)
        return (1);
    
    char **args = build_args(node);
    printf("Built arguments:\n");
    int i = 0;
    while (args && args[i])
    {
        fprintf(stderr, "arg[%d]: '%s'\n", i, args[i]);
        i++;
    }
    char *cmd_path;
    
    if (node->cmd[0] == '/' || (node->cmd[0] == '.' && node->cmd[1] == '/'))
        cmd_path = ft_strdup(node->cmd);
    else
        cmd_path = extract_path(env, args[0]);
    if (!cmd_path) {
        ft_putstr_fd("minishell: command not found: ", 2);
        free_split(args);
        return (127);
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        execve(cmd_path, args, env);
        ft_putstr_fd("minishell: ", 2);
        perror(node->cmd);
        exit(126);
    }
    
    free(cmd_path);
    free_split(args);
    
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}


int execute_node(t_tree *node, char **env)
{
    if (!node)
    {
        fprintf(stderr, "DEBUG - NULL node in execute_node\n");
        return (0);
    }
    fprintf(stderr, "DEBUG - execute_node received node with type: %d, cmd: %s\n", 
            node->type, node->cmd ? node->cmd : "NULL");
    if (node->type == PIPE)
        return (fprintf(stderr,"pipe\n"), execute_pipe(node, env));
    else if (node->type == REDIR_IN || node->type == HEREDOC)
        return (fprintf(stderr, "redir\n"),execute_redir(node, env));
    else if (node->type == REDIR_OUT || node->type == APPEND)
        return (fprintf(stderr, "redir\n"),execute_redir(node, env));
    else
        return (fprintf(stderr, "cmd\n"), execute_command(node, env));
}

char **build_args(t_tree *node)
{
    int arg_count = 1;  // Start with 1 for the command itself
    t_tree *current = node;
    
    // Count arguments by traversing right children
    while (current->right) {
        arg_count++;
        current = current->right;
    }
    
    // Allocate space for arguments plus NULL terminator
    char **args = malloc(sizeof(char *) * (arg_count + 1));
    if (!args)
        return NULL;
    
    // Add command as first argument
    args[0] = ft_strdup(node->cmd);
    
    // Add remaining arguments
    current = node;
    int i = 1;
    while (current->right) {
        args[i] = ft_strdup(current->right->cmd);
        current = current->right;
        i++;
    }
    args[i] = NULL;
    
    return args;
}