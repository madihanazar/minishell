#include "minishell.h"

void heredoc_sig(int sig)
{
	g_status = sig;
	write(1, "\n", 1);
	close(0);
}

t_context *create_context(void)
{
	t_context *context;

	context = malloc(sizeof(t_context));
	if (!context)
		return (NULL);
	context->args = NULL;
	context->cmd = NULL;
	context->input = -1;
	context->output = -1;
	context->error = 0;
	context->next = NULL;
	return (context);
}

void free_context(t_context *context)
{
	if (!context)
		return;
	if (context->args != NULL)
	{
		free_split(context->args);
		context->args = NULL;
	}
	if (context->cmd)
	{
		free(context->cmd);
		context->cmd = NULL;
	}
	if (context->input >= 0)
		close(context->input);
	context->input = -1;
	if (context->output >= 0)
		close(context->output);
	context->output = -1;
	context->error = 0;
	context->next = NULL;
	free(context);
}

void free_context_list(t_context *context)
{
	t_context *curr;
	t_context *temp;

	curr = context;
	temp = curr;
	while (curr)
	{
		temp = curr->next;
		free_context(curr);
		curr = temp;
	}
}

char *expand_heredocs(char *str, t_shell *shell)
{
	int i;
	int j;

	i = 0;
	j = 0;
	if (ft_strchr(str, '$') == NULL)
		return (str);
	while (str[i])
	{
		if (str[i] == '$')
		{
			str[i++] = '\0';
			str = expanded_str(str, &str[i--], shell);
			i--;
		}
		i++;
	}
	return (str);
}

void ft_putendl_fd(char *s, int fd)
{
	int size;

	size = ft_strlen(s);
	write(fd, s, size);
	write(fd, "\n", 1);
}

bool process_heredocs(t_shell *shell, t_tree *node, char **env)
{
	int pid;
	int status;
	int fd[2];

	if (pipe(fd) == -1)
		return (ft_putstr_fd("An error has occured\n", 2), false);
	pid = fork();
	if (pid == 0)
		child_heredoc(fd, shell, node, env);
	waitpid(pid, &status, 0);
	if (shell->context->input >= 0)
		close(shell->context->input);
	shell->context->input = fd[0];
	// close(fd[0]);
	close(fd[1]);
	return (WEXITSTATUS(status) == 0);
}

void child_heredoc(int *fd, t_shell *shell, t_tree *node, char **env)
{
	char *str;
	char *delim;

	signal(SIGINT, heredoc_sig);
	close(fd[0]);
	str = readline(">");
	delim = node->right->args[0];
	while (str && (ft_strcmp(str, delim) != 0))
	{
		str = expand_heredocs(str, shell);
		ft_putendl_fd(str, fd[1]);
		free(str);
		str = readline(">");
	}
	if (str)
		free(str);
	close(fd[1]);
	free_env(env);
	free_shell(shell);
	exit(g_status == SIGINT);
}

bool process_pipes(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	int fd[2];

	if (pipe(fd) == -1)
		return (ft_putstr_fd("An error has occured\n", 2), false);
	context->next = create_context();
	if (!context->next)
	{
		close(fd[0]);
		close(fd[1]);
		return (false);
	}
	context->next->input = fd[0];
	context->output = fd[1];
	return (preprocess(shell, context->next, node->right, env));
}

bool preprocess(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	if (node == NULL)
		return (true);
	if (node->type == HEREDOC)
		if (process_heredocs(shell, node, env) == false)
			return (false);
	if (!preprocess(shell, context, node->left, env))
		return (false);
	if (node->type == PIPE)
		if (process_pipes(shell, context, node, env) == false)
			return (false);
	return (true);
}

int is_builtin(char *cmd)
{
	return (!ft_strncmp(cmd, "cd", 3) || !ft_strncmp(cmd, "echo", 5) || 
		!ft_strncmp(cmd, "pwd", 4) || !ft_strncmp(cmd, "export", 7) || 
		!ft_strncmp(cmd, "unset", 6) || !ft_strncmp(cmd, "env", 4) || 
		!ft_strncmp(cmd, "exit", 5)
	);
}

char	*join_path(char *path, char *args)
{
	char	*temp;
	char	*full_cmd;

	temp = ft_strjoin(path, "/");
	if (temp == NULL)
		return (NULL);
	full_cmd = ft_strjoin(temp, args);
	free(temp);
	return (full_cmd);
}

char	*extract_path(char *cmd, char **env)
{
	int		i;
	char	**paths;
	char	*full_cmd;

	i = 0;
	while (env[i] != NULL && (ft_strncmp(env[i], "PATH=", 5) != 0))
		i++;
	if (env[i] == NULL)
		return (NULL);
	paths = ft_split(env[i] + 5, ':');
	if (paths == NULL)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_cmd = join_path(paths[i], cmd);
		if (full_cmd == NULL)
			return (free_split(paths), NULL);
		if (access(full_cmd, X_OK) == 0)
			return (free_split(paths), full_cmd);
		free(full_cmd);
		i++;
	}
	return (free_split(paths), NULL);
}

bool	process_command(t_context *context, t_tree *node, char **env)
{
	context->args = node->args;
	node->args = NULL;
	if (!context->args)
		return (false);
	if (context->args[0])
	{
		if (is_builtin(context->args[0]))
			context->cmd = ft_strdup(context->args[0]);
		else
			context->cmd = extract_path(context->args[0], env);
	}
	return (true);
}

bool	check_heredoc(t_tree *node)
{
	if (node == NULL)
		return (false);
	if (node->type == HEREDOC)
		return (true);
	return (check_heredoc(node->left));
}

bool	process_input(t_context *context, t_tree *node, char **env)
{
	int	fd;

	if (!node->right->args[0])
		return (false);
	fd = open(node->right->args[0], O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd(node->right->args[0], 2);
		if (errno == EACCES)
			ft_putstr_fd(": Permission denied\n", 2);
		else
			ft_putstr_fd(": No such file or directory\n", 2);
		return (false);
	}
	if (check_heredoc(node->left))
		return (close(fd), traverse_tree(context, node->left, env));
	if (context->input >= 0)
		close(context->input);
	context->input = fd;
	return (traverse_tree(context, node->left, env));
}

bool	process_output(t_context *context, t_tree *node, char **env, int flag)
{
	if (context->output >= 0)
		close(context->output);
	if (!flag)
		context->output = open(node->right->args[0],
							   O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		context->output = open(node->right->args[0],
							   O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (context->output == -1)
	{
		ft_putstr_fd(node->right->args[0], 2);
		if (errno == EACCES)
			ft_putstr_fd(": Permission denied\n", 2);
		else
			ft_putstr_fd(": No such file or directory\n", 2);
		return (false);
	}
	return (traverse_tree(context, node->left, env));
}

bool	traverse_tree(t_context *context, t_tree *node, char **env)
{
	if (node->type == PIPE)
		(traverse_tree(context, node->left, env)),
			(traverse_tree(context->next, node->right, env));
	if (node->type == HEREDOC)
		traverse_tree(context, node->left, env);
	if (node->type == NODE_COMMAND)
		return (process_command(context, node, env));
	if (node->type == REDIR_IN)
		return (process_input(context, node, env));
	if (node->type == REDIR_OUT)
		return (process_output(context, node, env, 1));
	if (node->type == APPEND)
		return (process_output(context, node, env, 0));
	return (true);
}

int new_execute(t_shell *shell)
{
	char **env;

	signal(SIGINT, SIG_IGN);
	env = list_to_env(shell->env_list);
	if (!env)
		return (ft_putstr_fd("An error has occured\n", 2), 1);
	shell->context = create_context();
	if (!(shell->context))
		return (free_env(env), ft_putstr_fd("An error has occured\n", 2), 1);
	if (!preprocess(shell, shell->context, shell->tree, env))
		return (free_env(env), 0);
	if (!traverse_tree(shell->context, shell->tree, env))
		return (free_env(env), ft_putstr_fd("An error has occured\n", 2), 1);
	free_env(env);
	return (0);
}
