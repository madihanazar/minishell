/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:11:09 by nkunnath          #+#    #+#             */
/*   Updated: 2025/01/31 17:11:11 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_status = 0;

void handle_sigint(int sig)
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

int	main(int argc, char **argv, char **env)
{
	char	*input;
	t_shell	*shell;
	t_tree	*ast;
	char	**env_copy;

	(void)argc;
	(void)argv;
	input = NULL;
	ast = NULL;
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	shell = create_shell();
	if (shell == NULL)
	{
		ft_putstr_fd("An error has occured\n", 2);
		return (1);
	}
	env_copy = create_env_copy(env);
	if (env_copy == NULL)
	{
		free_shell(shell);
		ft_putstr_fd("An error has occured\n", 2);
		return (1);
	}
	// create the shell - Done
	// create the env linked list - To do
	// initialize the env linked list
	// attach it to the shell
	while (1)
	{
		input = readline("minishell>");
		if (!input)
			return (free_shell(shell), free_env(env_copy), g_status);
		add_history(input);
		ast = create_tree(input, env_copy); 
		if (ast == NULL)
			return (free(input), free_shell(shell), 2); // If tree creation fails, there are no memory leaks (from our side).
		g_status = execute_node(ast, &env_copy, shell);
		free(input);
		free_ast(ast);
		input = NULL;
		ast = NULL;
	}
	return (g_status);
}
