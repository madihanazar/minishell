/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 16:16:25 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 23:58:48 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_sig(int sig)
{
	g_status = sig;
	write(1, "\n", 1);
	close(0);
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

int	new_execute(t_shell *shell)
{
	char	**env;
	int		status;
	pid_t	pid;

	status = 0;
	signal(SIGINT, SIG_IGN);
	env = list_to_env(shell->env_list);
	if (!env)
		return (ft_putstr_fd("An error has occured\n", 2), 1);
	shell->context = create_context();
	if (!(shell->context))
		return (free_env(env), ft_putstr_fd("An error has occured\n", 2), 1);
	if (!preprocess(shell, shell->context, shell->tree, env))
		return (free_env(env), 1);
	if (!traverse_tree(shell->context, shell->tree, env))
		return (free_env(env), 1);
	if (shell->context->next == NULL && is_builtin(shell->context->cmd))
	{
		status = new_execute_builtin(shell, env);
		free_env(env);
		return (status);
	}
	if (!execute_context(shell, env, &pid))
		return (ft_putstr_fd("An error has occurred: ", 2), free_env(env), 1);
	free_env(env);
	waitpid(pid, &status, 0);
	while (wait(NULL) != -1)
		;
	return (check_status(status));
}
