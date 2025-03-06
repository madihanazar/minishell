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

void print_ast(t_tree *node, int depth)
{
    if (node != NULL)
    {
        // Print the left subtree first (if it exists)
        if (node->left)
        {
            print_ast(node->left, depth + 1);  // Increase depth for left child
        }

        // Print the current node's command and depth
        printf("Depth: %d, Command: %s, Type: %d\n", depth, node->cmd, node->type);

        // Print the right subtree (if it exists)
        if (node->right)
        {
            print_ast(node->right, depth + 1);  // Increase depth for right child
        }
    }
}

void	convert_to_whitespace(char	*input)
{
	while (*input)
	{
		if (*input == '"' || *input == '\'')
			input = ft_strchr(input + 1, *input);
		else if (*input > 8 && *input < 13)
			*input = ' ';
		if (input == NULL)
			return ;
		input += 1;
	}
}

int	skip_whitespaces(char	*input)
{
	int	i;

	i = 0;
	while (input[i] == ' ')
		i += 1;
	return (i);
}

bool	is_valid_string(char *input)
{
	if (!is_valid_pipe(input))
		return (false);
	if (!is_valid_redir(input))
		return (false)
	
}

char	*get_input(void)
{
	char	*input;
	int		i;

	while (1)
	{
		i = 0;
		if (isatty(0))
			input = readline("minishell>");
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
		if (add_history(input), true && is_valid_string(input))
			return (input);
		ft_putstr_fd("Input parsing error\n", 2);
		free(input);
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
		input = get_input();
		if (!input)
			return (g_status);
		shell->tree = create_tree(input, shell);
		if (shell->tree == NULL)
			return (free(input), 1);
		print_ast(shell->tree, 0);
		// g_status = execute_node(shell);
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
