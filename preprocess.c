/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preprocess.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 17:13:46 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 17:13:46 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_heredocs(char *str, t_shell *shell)
{
	int	i;

	i = 0;
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

static void	child_heredoc(int *fd, t_shell *shell, t_tree *node, char **env)
{
	char	*str;
	char	*delim;

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

bool	proc_hdocs(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	int	pid;
	int	status;
	int	fd[2];

	if (pipe(fd) == -1)
		return (ft_putstr_fd("An error has occured\n", 2), false);
	pid = fork();
	if (pid == 0)
		child_heredoc(fd, shell, node, env);
	waitpid(pid, &status, 0);
	if (context->input >= 0)
		close(context->input);
	context->input = fd[0];
	close(fd[1]);
	return (WEXITSTATUS(status) == 0);
}

bool	proc_pipes(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	int	fd[2];

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

bool	preprocess(t_shell *shell, t_context *context, t_tree *node, char **env)
{
	if (node == NULL)
		return (true);
	if (node->type == HEREDOC)
		if (proc_hdocs(shell, context, node, env) == false)
			return (false);
	if (!preprocess(shell, context, node->left, env))
		return (false);
	if (node->type == PIPE)
		if (proc_pipes(shell, context, node, env) == false)
			return (false);
	return (true);
}
