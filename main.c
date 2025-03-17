/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:11:09 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/15 17:08:40 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// echo hi | echo >>./outfiles/outfile01 bye >./test_files/invalid_permission

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

char	*is_valid_pipe(char *input)
{
	input += 1;
	while (*input)
	{
		if (*input != ' ')
			break ;
		input += 1;
	}
	if (*input == '\0' || *input == '|')
		return (NULL);
	return (input - 1);
}

char	*is_valid_redir(char *str)
{
	if (*str == *(str + 1))
		str++;
	str++;
	while (*str)
	{
		if (*str != ' ')
			break ;
		str++;
	}
	if (ft_strchr("<>|&", *str) != NULL)
		return (NULL);
	while (*str)
	{
		if (*str == '"' || *str == '\'')
			str = ft_strchr(str + 1, *str);
		else if (ft_strchr(" <>|&", *str))
			break ;
		if (!str)
			return (NULL);
		str++;
	}
	return (str - 1);
}

bool	is_valid_string(char *input)
{
	int	words;

	words = 0;
	while (*input)
	{
		if ((*input == '"' || *input == '\'') && ++words)
			input = ft_strchr(input + 1, *input);
		else if ((*input == '>' || *input == '<') && ++words)
			input = is_valid_redir(input);
		else if (*input == '|')
		{
			input = is_valid_pipe(input);
			if (input == NULL || words == 0)
				return (false);
			words = 0;
		}
		else if (*input != ' ' && *input != '\t')
			words += 1;
		if (input == NULL)
			return (false);
		input += 1;
	}
	return (true);
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
