#include "minishell.h"

int execute_pipe(t_tree *node, char **env)
{
    pid_t pid1;
    pid_t pid2;
    int pipefd[2];
    int status1;
    int status2;

    if (pipe(pipefd) == -1)
        (perror("Error opening the pipe"), exit(0));
    pid1 = fork();
    if (pid1 == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        exit(execute_node(node->left, env));
    }
    pid2 = fork();
    if (pid2 == 0)
    {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        exit(execute_node(node->right, env));
    }
    (close(pipefd[0]), close(pipefd[1]));
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    if (WEXITSTATUS(status1) == 127)
        return (127);
    return (WEXITSTATUS(status2));
}

int execute_redir(t_tree *node, char **env)
{
    pid_t pid;
    int fd;
    int status;

    if (node->type == REDIR_IN)
        fd = open(node->right->cmd, O_RDONLY);
    else if (node->type == REDIR_OUT)
        fd = open(node->right->cmd, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    else if (node->type == APPEND)
        fd = open(node->right->cmd, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
        perror("Error"), exit(1);
    pid = fork();
    if (pid == 0)
    {
        if (node->type == REDIR_IN)
            dup2(fd, STDIN_FILENO);
        else
            dup2(fd, STDOUT_FILENO);
        close(fd);
        exit(execute_node(node->left, env));
        //exit(0);
    }
    close(fd);
    waitpid(pid, &status, 0);
    return (WEXITSTATUS(status));
}

int execute_cmd(t_tree *node, char **env)
{
    char **args;
    char *cmd_path;
    pid_t pid;
    int status;

    args = build_args(node);
    if (!args)
        return (perror("Error"), 1);
     if (ft_strchr(node->cmd, '/'))
        cmd_path = ft_strdup(node->cmd);
    else
        cmd_path = extract_path(env, args[0]);
    if (is_builtin(args[0]))
        return (execute_builtin(node, args, env));
    pid = fork();
    if (pid == 0)
    {
        if (execve(cmd_path, args, env) == -1)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(args[0], 2);
            ft_putstr_fd(": command not found\n", 2);
            free_split(args);
            free(cmd_path);
            return(127); 
        }
    }
    free_split(args);
    free(cmd_path);
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
    
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
int execute_node(t_tree *node, char **env)
{
    if (!node)
        return (0);
    if (node->type == PIPE)
        return (execute_pipe(node, env));
    else if (node->type == REDIR_IN || node->type == REDIR_OUT || node->type == APPEND)
        return (execute_redir(node, env));
    // else if (node->type == HEREDOC)
    //     return (execute_heredoc(node, env));
    else if (node->type == NODE_COMMAND)
        return (execute_cmd(node, env));
    return (0);
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
