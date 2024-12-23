/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_tree.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 20:31:33 by mnazar            #+#    #+#             */
/*   Updated: 2024/12/23 21:21:50 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	split_pipes(char *str, t_tree *tree_node)
{
	char	*begin;

	begin = str;
	while (*str)
	{
		if (*str == '|')
			check_tree(ft_substr(str, begin, begin - str + 1));
		str++;
	}
}

void	split_redirects(char *str)
{
	char	*begin;

	begin = str;
	while (*str)
	{
		if (*str == '<' || *str == '>')
		{
			//if (*str++ == '<' || *str++ == '>')
			
		}
		check_tree(ft_substr(str, begin, begin - str + 1));
		str++;
	}
}

t_tree	*create_tree(char *str)
{
	t_tree	*tree_node;

	tree_node = create_node(str);
	if (tree_node == NULL)
		return (NULL);
	while (*str)
	{
		split_pipes(str, &tree_node);
		split_redirects(str, &tree_node);

	}
	return (tree_node);
}
