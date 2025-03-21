/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_tree.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 14:45:58 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/21 14:45:58 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree	*create_tree(char *str, t_shell *shell)
{
	t_tree	*root;
	char	*trimmed;

	root = NULL;
	trimmed = NULL;
	trimmed = ft_strtrim(str, " ");
	if (trimmed == NULL)
		return (NULL);
	root = build_ast(str, shell);
	free(trimmed);
	return (root);
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
		input++;
	}
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
