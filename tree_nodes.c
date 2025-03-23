/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_nodes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 21:02:27 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 15:23:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree	*create_node(char *str, t_node_type type)
{
	t_tree	*node;

	node = NULL;
	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
	node->left = NULL;
	node->right = NULL;
	node->args = NULL;
	node->cmd = ft_strdup(str);
	if (node->cmd == NULL)
		return (free(node), NULL);
	node->type = type;
	return (node);
}

void	free_result(char **result, int count)
{
	int	i;

	i = 0;
	if (!result)
		return ;
	while (i < count)
	{
		free(result[i]);
		result[i] = NULL;
		i++;
	}
	free(result);
	result = NULL;
}

void	free_split(char **result)
{
	int	i;

	i = 0;
	if (!result)
		return ;
	while (result[i])
	{
		free(result[i]);
		result[i] = NULL;
		i++;
	}
	free(result);
	result = NULL;
}

void	free_ast(t_tree *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->cmd)
	{
		free(node->cmd);
		node->cmd = NULL;
	}
	if (node->args)
	{
		free_split(node->args);
		node->args = NULL;
	}
	free(node);
	node = NULL;
}
