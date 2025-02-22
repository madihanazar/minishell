#include "minishell.h"

int execute_node(t_tree *node, char ***env, t_shell *shell)
{
	if (!node)
		g_status = 1;
	else if (node->type == PIPE)
		g_status = execute_pipe(node, env, shell);
	else if (node->type == REDIR_IN || node->type == REDIR_OUT || node->type == APPEND)
		g_status = execute_redir(node, env, shell);
	else if (node->type == HEREDOC)
		g_status = process_heredocs(node, env);
	else if (node->type == NODE_COMMAND)
		g_status = execute_cmd(node, env, shell);
	return (g_status);
}

int execute_pipe(t_tree *node, char ***env, t_shell *shell)
{
    pid_t pid1;
    pid_t pid2;
    int pipefd[2];
    int status1;
    int status2;
    if (has_heredoc(node->left))
    {
        if (process_heredocs(node->left, env) != 0)
            return (1);
    }
    if (has_heredoc(node->right))
    {
        if (process_heredocs(node->right, env) != 0)
            return (1);
    }
    if (pipe(pipefd) == -1)
        (perror("Error opening the pipe"), exit(0));
    pid1 = fork();
    if (pid1 == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        exit(execute_node(node->left, env, shell));
    }
    pid2 = fork();
    if (pid2 == 0)
    {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        exit(execute_node(node->right, env, shell));
    }
    (close(pipefd[0]), close(pipefd[1]));
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    return (check_status(status2));
}

int execute_redir(t_tree *node, char ***env, t_shell *shell)
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
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
        if (node->type == REDIR_IN)
            dup2(fd, STDIN_FILENO);
        else
            dup2(fd, STDOUT_FILENO);
        close(fd);
        exit(execute_node(node->left, env, shell));
        // exit(0);
    }
    close(fd);
    waitpid(pid, &status, 0);
    return (check_status(status));
}

int execute_cmd(t_tree *node, char ***env, t_shell *shell)
{
    char **args;
    char *cmd_path;
    pid_t pid;
    int status;

    args = build_args(node);
    if (!args)
        return (perror("An error has occured\n"), 1);
    if (is_builtin(args[0]))
		return (execute_builtin(node, args, env, shell));
    if (ft_strchr(node->cmd, '/'))
		cmd_path = ft_strdup(node->cmd);
    else
		cmd_path = extract_path(*env, args[0]);
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (node->heredoc_fd > 0)
		{
			dup2(node->heredoc_fd, STDIN_FILENO);
			close(node->heredoc_fd);
		}
		if (cmd_path && (!ft_strcmp(cmd_path, "./minishell")))
		{
			char *SHLVL_str = get_env_value("SHLVL", *env);
			if (SHLVL_str == NULL)
			{
				ft_putstr_fd("An error has occured\n", 2);
				g_status = 1;
				builtin_exit(node, args, *env, shell);
			}
			int SHLVL_int = atoi(SHLVL_str);
			int j = 0;
			free(SHLVL_str);
			SHLVL_str = NULL;
			SHLVL_int = SHLVL_int + 1;
			SHLVL_str = ft_itoa(SHLVL_int);
			if (SHLVL_str == NULL)
			{
				ft_putstr_fd("An error has occured\n", 2);
				g_status = 1;
				builtin_exit(node, args, *env, shell);
			}
			while ((*env)[j])
			{
				if (ft_strncmp((*env)[j], "SHLVL", 5) == 0)
				{
					char *final_str = ft_substr((*env)[j], 0, 6);
					if (final_str == NULL)
					{
						free(SHLVL_str);
						ft_putstr_fd("An error has occured\n", 2);
						g_status = 1;
						builtin_exit(node, args, *env, shell);
					}
					char *finalest_str = ft_strjoin(final_str, SHLVL_str);
					if (finalest_str == NULL)
					{
						free(final_str);
						free(SHLVL_str);
						ft_putstr_fd("An error has occured\n", 2);
						g_status = 1;
						builtin_exit(node, args, *env, shell);
					}
					free((*env)[j]);
					(*env)[j] = finalest_str;
					free(SHLVL_str);
					break ;
				}
				j += 1;
			}
		}
		if (execve(cmd_path, args, *env) == -1)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(args[0], 2);
			ft_putstr_fd(": command not found\n", 2);
			// free_split(args);
			free(cmd_path);
			// free_shell(shell);
			// free_env(*env);
			// free_ast(node);
			g_status = 127;
			// exit(g_status);
			builtin_exit(node, args, *env, shell);
		}
    }
	waitpid(pid, &status, 0);
	free_split(args);
	free(cmd_path);
	return (check_status(status));
}

char **build_args(t_tree *node)
{
    int arg_count = 1;
    t_tree *current = node;

    // Count arguments by traversing right children
    while (current->right)
    {
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
    while (current->right)
    {
        args[i] = ft_strdup(current->right->cmd);
        current = current->right;
        i++;
    }
    args[i] = NULL;
    return args;
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

char *extract_path(char *envp[], char *args)
{
    int i;
    char **paths;
    char *full_cmd;

    i = 0;
    while (envp[i] != NULL && (ft_strncmp(envp[i], "PATH=", 5) != 0)) // Consider using getenv()
        i++;
    if (envp[i] == NULL)
        return (NULL);
	// printf("evnp[i] value: %s\n", envp[i]);
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
		// printf("full_cmd value:%s\n", full_cmd);
		free(full_cmd);
		i++;
    }
    return (free_split(paths), NULL);
}
