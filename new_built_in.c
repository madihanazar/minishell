/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_built_in.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 18:23:28 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/24 14:55:18 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_env(char **env, int fd)
{
	int		i;
	char	*temp;

	i = 0;
	if (fd == -1)
		fd = 1;
	while (env[i])
	{
		temp = ft_strchr(env[i], '=');
		if (temp == NULL)
			i += 1;
		else
		{
			ft_putendl_fd(env[i], fd);
			i++;
		}
	}
	return (0);
}

int	builtin_pwd(int fd)
{
	char	*cwd;

	if (fd == -1)
		fd = 1;
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (ft_putstr_fd("An error has occured", 2), 1);
	ft_putendl_fd(cwd, fd);
	free(cwd);
	return (0);
}

static bool	exit_atoi(char *str)
{
	int		i;
	int		sign;
	int		answer;

	i = 0;
	answer = 0;
	sign = 1;
	i = atoi_skip(str);
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i++] == '-')
			sign = -1;
	}
	while (ft_isdigit(str[i]))
		answer = ((answer * 10) + (str[i++] - 48)) % 256;
	i += atoi_skip(str + i);
	if (str[i])
	{
		g_status = 255;
		return (false);
	}
	if (sign == -1)
		answer = 256 - answer;
	g_status = answer;
	return (true);
}

int	builtin_exit(t_shell *shell, char **env)
{
	int	fd;

	fd = shell->context->output;
	if (fd == -1)
		fd = 1;
	if (shell->context->args[1] == NULL)
	{
		free_env_and_shell(env, shell, fd, 0);
		exit(g_status);
	}
	if (exit_atoi(shell->context->args[1]) == false)
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(shell->context->args[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		free_env_and_shell(env, shell, fd, 1);
		exit(g_status);
	}
	if (shell->context->args[2] != NULL)
	{
		ft_putstr_fd("exit\n", 2);
		return (ft_putstr_fd("minishell: exit: too many arguments\n", 2), 1);
	}
	free_env_and_shell(env, shell, fd, 0);
	exit(g_status);
}

int	new_execute_builtin(t_shell *shell, char **env)
{
	signal(SIGPIPE, SIG_IGN);
	if (!ft_strncmp(shell->context->cmd, "cd", 3))
		return (builtin_cd(shell, shell->context));
	else if (!ft_strncmp(shell->context->cmd, "echo", 5))
		return (builtin_echo(shell->context->args, shell->context->output));
	else if (!ft_strncmp(shell->context->cmd, "env", 4))
		return (builtin_env(env, shell->context->output));
	else if (!ft_strncmp(shell->context->cmd, "pwd", 4))
		return (builtin_pwd(shell->context->output));
	else if (!ft_strncmp(shell->context->cmd, "exit", 5))
		return (builtin_exit(shell, env));
	else if (!ft_strncmp(shell->context->cmd, "export", 7))
		return (builtin_export(shell, env));
	else if (!ft_strncmp(shell->context->cmd, "unset", 6))
		return (builtin_unset(shell));
	signal(SIGPIPE, SIG_DFL);
	return (1);
}
