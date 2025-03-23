/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 18:27:38 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/22 18:27:38 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_first_pipe(char *str)
{
	int		single_quote;
	int		double_quote;

	single_quote = 0;
	double_quote = 0;
	if (str == NULL || *str == '\0')
		return (NULL);
	while (*str)
	{
		if (*str == '"')
			double_quote = !double_quote;
		else if (*str == '\'')
			single_quote = !single_quote;
		if (*str == '|' && !double_quote && !single_quote)
			return (str);
		str++;
	}
	return (NULL);
}

void	check_redir_type(t_tree *node, char *str)
{
	if (*str == '>')
	{
		if (*(str + 1) == '>')
			node->type = APPEND;
		else
			node->type = REDIR_OUT;
	}
	else if (*str == '<')
	{
		if (*(str + 1) == '<')
			node->type = HEREDOC;
		else
			node->type = REDIR_IN;
	}
}

char	*find_first_redir(t_tree *node, char *str)
{
	int		single_quote;
	int		double_quote;

	single_quote = 0;
	double_quote = 0;
	if (str == NULL)
		return (NULL);
	while (*str)
	{
		if (*str == '"')
			double_quote = !double_quote;
		else if (*str == '\'')
			single_quote = !single_quote;
		if ((*str == '<' || *str == '>') && !double_quote && !single_quote)
		{
			check_redir_type(node, str);
			return (str);
		}
		str++;
	}
	return (NULL);
}

t_tree	*free_handle_pipe_redir(int *flag, t_tree *node, char *str)
{
	*flag = 1;
	if (node)
		free_ast(node);
	if (str)
		free(str);
	return (NULL);
}

t_tree	*free_commands(int *flag, char **cmd_tokens, t_tree *node, char *str)
{
	if (cmd_tokens)
		free_split(cmd_tokens);
	return (free_handle_pipe_redir(flag, node, str));
}

void	free_strings(char *str1, char *str2)
{
	if (str1)
		free(str1);
	if (str2)
		free(str2);
}
