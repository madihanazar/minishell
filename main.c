/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:11:09 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 22:39:42 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_status = 0;

static void	handle_sigint(int sig)
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

static int	skip_whitespaces(char	*input)
{
	int	i;

	i = 0;
	while (input[i] == ' ')
		i += 1;
	return (i);
}

char	*get_input(void)
{
	char	*input;
	int		i;

	while (1)
	{
		i = 0;
		if (isatty(0))
			input = readline("minishell> ");
		else
			input = readline("");
		if (!input)
			return (NULL);
		convert_to_whitespace(input);
		i = skip_whitespaces(input);
		if (input[i] == '\0')
		{
			free(input);
			continue ;
		}
		if ((add_history(input), true) && is_valid_string(input))
			return (input);
		g_status = 258;
		ft_putstr_fd("Input parsing error\n", 2);
		free(input);
	}
}

int	main_loop(t_shell *shell)
{
	char	*input;

	if (env_init(&(shell->env_list)))
		return (ft_putstr_fd("An error has occured\n", 2), 1);
	while (1)
	{
		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);
		input = get_input();
		if (!input)
			return (g_status);
		shell->tree = create_tree(input, shell);
		if (shell->tree == NULL)
			return (free(input), 1);
		g_status = new_execute(shell);
		free(input);
		free_ast(shell->tree);
		free_context_list(shell->context);
		input = NULL;
		shell->tree = NULL;
		shell->context = NULL;
	}
	return (g_status);
}

int	main(int argc, char **argv, char **env)
{
	t_shell	*shell;

	(void)argc;
	(void)argv;
	shell = create_shell();
	if (shell == NULL)
		return (ft_putstr_fd("An error has occured\n", 2), 1);
	if (!isatty(0))
		rl_outstream = stdin;
	shell->env_list = env_to_list(env);
	if (shell->env_list == NULL)
	{
		free_shell(shell);
		return (ft_putstr_fd("An error has occured\n", 2), 1);
	}
	g_status = main_loop(shell);
	free_shell(shell);
	return (g_status);
}
