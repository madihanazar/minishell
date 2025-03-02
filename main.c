/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:11:09 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/02 14:39:23 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_status = 0;

void	handle_sigint(int sig)
{
	g_status = sig;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

int	main_loop(t_shell *shell)
{
	char	*input;

	if (env_init(&(shell->env_list)))
	{
		ft_putstr_fd("An error has occured\n", 2);
		return (1);
	}
	while (1)
	{
		input = readline("minishell>");
		if (!input)
			return (g_status);
		add_history(input);
		shell->tree = create_tree(input, shell);
		if (shell->tree == NULL)
			return (free(input), 1);
		g_status = execute_node(shell);
		free(input);
		free_ast(shell->tree);
		input = NULL;
		shell->tree = NULL;
	}
	return (g_status);
}

int	main(int argc, char **argv, char **env)
{
	t_shell	*shell;
	t_list	*env_list;

	(void)argc;
	(void)argv;
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	shell = create_shell();
	if (shell == NULL)
	{
		ft_putstr_fd("An error has occured\n", 2);
		return (1);
	}
	shell->env_list = env_to_list(env);
	if (shell->env_list == NULL)
	{
		free_shell(shell);
		ft_putstr_fd("An error has occured\n", 2);
		return (1);
	}
	g_status = main_loop(shell);
	free_shell(shell);
	return (g_status);
}
