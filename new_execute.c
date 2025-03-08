#include "minishell.h"

t_context	*create_context(void)
{
	t_context	*context;

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

void	free_context(t_context *context)
{
	if (!context)
		return ;
	free_split(context->args);
	context->args = NULL;
	free(context->cmd);
	context->cmd = NULL;
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

void	free_context_list(t_context *context)
{
	t_context	*curr;
	t_context	*temp;

	curr = context;
	temp = curr;
	while (curr)
	{
		temp = curr->next;
		free_context(curr);
		curr = temp;
	}
}

char	*expand_heredocs(char *str, t_shell *shell)
{
	int		i;
	int		j;

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

void	ft_putendl_fd(char *s, int fd)
{
	int	size;

	size = ft_strlen(s);
	write(fd, s, size);
	write(fd, "\n", 1);
}

bool	process_heredocs(t_shell *shell, t_tree *node, char *delim, char **env)
{
	int		pid;
	int		status;
	int		fd[2];

	if (pipe(fd) == -1)
		return (false);
	pid = fork();
	if (pid == 0)
		child_heredoc(fd, shell, node, delim, env);
	waitpid(pid, &status, 0);
	if (shell->context->input >= 0)
		close(shell->context->input);
	shell->context->input = fd[0];
	close(fd[0]);
	close(fd[1]);
	return (WEXITSTATUS(status) == 0);
}

void	child_heredoc(int *fd, t_shell *shell, t_tree *node, char *delim, char **env)
{
	char	*str;
	
	close(fd[0]);
	str = readline(">");
	while (str && (ft_strcmp(str, delim) != 0))
	{
		str = expand_heredocs(str, shell);
		ft_putendl_fd(str, fd[1]);
		free(str);
		str = readline(">");
	}
	if (str)
		free(str);
	free_env(env);
	free_shell(shell);
	close(fd[1]);
	exit(0);  //modify the exit value???
}

bool process_pipes(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	int		fd[2];

	if (pipe(fd) == -1)
		return (false);
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

bool	preprocess(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	if (node == NULL)
		return (true);
	if (node->type == HEREDOC)
		if (process_heredocs(shell, node, node->right->args[0], env) == false)
			return (false);
	if (!preprocess(shell, context, node->left, env))
		return (false);
	if (node->type == PIPE)
		if (process_pipes(shell, context, node, env) == false)
			return (false);
	return (true);
}

int	new_execute(t_shell *shell)
{
	char	**env;

	// Add signal handler
	if (!(env = list_to_env(shell->env_list)))
		return (ft_putstr_fd("An error has occured\n", 2), 1);
	if (!(shell->context = create_context()))
		return (free_env(env), ft_putstr_fd("An error has occured\n", 2), 1);
	if (!preprocess(shell, shell->context, shell->tree, env))
		return (free_env(env), ft_putstr_fd("An error has occured\n", 2), 1);
	free_env(env);
	return (0);
}