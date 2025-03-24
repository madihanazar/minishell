/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_exec_context.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 13:52:44 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/24 12:25:21 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_execution_error(char *cmd)
{
	DIR	*dir;
	int	errnum;

	errnum = errno;
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	dir = opendir(cmd);
	if (dir)
	{
		closedir(dir);
		if (ft_strchr(cmd, '/'))
			return (ft_putstr_fd(": is a directory\n", 2), 126);
		return (ft_putstr_fd(": command not found\n", 2), 127);
	}
	if (errnum == EACCES)
		return (ft_putstr_fd(": Permission denied\n", 2), 126);
	if (ft_strchr(cmd, '/') != NULL)
		ft_putstr_fd(": No such file or directory\n", 2);
	else
		ft_putstr_fd(": command not found\n", 2);
	return (127);
}

int	execute_command(t_context *context, char **env)
{
	int		status;

	status = 0;
	free_context_list(context->next);
	if (context->input != -1)
	{
		dup2(context->input, 0);
		close(context->input);
	}
	if (context->output != -1)
	{
		dup2(context->output, 1);
		close(context->output);
	}
	if (!context->args || !context->args[0])
	{
		if (!context->args)
			return (1);
		return (0);
	}
	if (context->cmd)
		execve(context->cmd, context->args, env);
	status = get_execution_error(context->args[0]);
	return (status);
}

bool	execute_context(t_shell *shell, char **env, pid_t *pid)
{
	t_context	*next;
	int			status;

	while (shell->context)
	{
		next = shell->context->next;
		*pid = fork();
		if (*pid == -1)
			return (false);
		if (*pid == 0)
		{
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			if (is_builtin(shell->context->cmd))
				status = new_execute_builtin(shell, env);
			else
				status = execute_command(shell->context, env);
			free_env(env);
			free_shell(shell);
			exit(status);
		}
		free_context(shell->context);
		shell->context = next;
	}
	return (true);
}

int	atoi_skip(char	*str)
{
	int	i;

	i = 0;
	while (str[i] == ' ' || str[i] == '\t' || (str[i] >= 9 && str[i] <= 13))
		i += 1;
	return (i);
}

bool	preproc_traverse(t_shell *shell, char **env)
{
	if (!preprocess(shell, shell->context, shell->tree, env))
		return (false);
	if (!traverse_tree(shell->context, shell->tree, env))
		return (false);
}
