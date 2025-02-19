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

static t_shell	*create_shell(void)
{
	t_shell	*shell;

	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->export_list = NULL;
	return (shell);
}

void	free_shell(t_shell *shell)
{
	if (shell)
	{
		if (shell->export_list)
			ft_lstclear(&shell->export_list, free);
		free(shell);
	}
}

char	**create_env_copy(char **env)
{
	int		i;
	int		total;
	char	**env_copy;
	char	*test_str;

	i = 0;
	total = 0;
	test_str = NULL;
	while (env[total])
		total += 1;
	env_copy = (char **)malloc(sizeof(char *) * (total + 1));
	if (env_copy == NULL)
		return (NULL);
	i = 0;
	while (env[i])
	{
		test_str = ft_strdup(env[i]);
		if (test_str == NULL)
			return (free_result(env_copy, total), NULL);
		env_copy[i] = test_str;
		i += 1;
	}
	env_copy[i] = NULL;
	return (env_copy);
}

void	free_env(char **env_copy)
{
	int		i;

	i = 0;
	while (env_copy[i])
	{
		free(env_copy[i]);
		i += 1;
	}
	free(env_copy);
	return ;
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
		return (1);
	env_copy = create_env_copy(env);
	if (env_copy == NULL)
	{
		free_shell(shell);
		return (2);
	}
	while (1)
	{
		input = readline("minishell>");
		if (!input)
			return (free_shell(shell), free_env(env_copy), g_status);
		add_history(input);
		ast = create_tree(input, env_copy); 
		if (ast == NULL)
			return (free(input), free_shell(shell), 1); // If tree creation fails, there are no memory leaks (from our side).
		g_status = execute_node(ast, &env_copy, shell);
		free(input);
		free_ast(ast);
		input = NULL;
		ast = NULL;
	}
	return (g_status);
}
