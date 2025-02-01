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

int	g_sig = 0;

static t_shell	*create_shell(void)
{
	t_shell	*shell;

	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->export_list = NULL;
	return (shell);
}

static void	free_shell(t_shell *shell)
{
	if (shell)
	{
		if (shell->export_list)
			ft_lstclear(&shell->export_list, free);
		free(shell);
	}
}

int	main(int argc, char **argv, char **env)
{
	char	*input;
	t_shell	*shell;
	t_tree	*ast;
	char	**env_copy;
	int		status;

	(void)argc;
	(void)argv;
	shell = create_shell();
	if (shell == NULL)
		return (1);
	// signal(SIGINT, handle_sigint);   // Ctrl+C
	// signal(SIGQUIT, SIG_IGN);
	env_copy = env;
	while (1)
	{
		input = readline("minishell>");
		if (!input)
			return (free_shell(shell), 1);
		add_history(input);
		ast = create_tree(input, env_copy);
		if (ast == NULL)
			return (free(input), free_shell(shell), 1);
		// print_ast(ast, 0);
		status = execute_node(ast, &env_copy, shell);
		free(input);
	}
	return (status);
}
