/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_nodes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 21:02:27 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 13:44:04 by nkunnath         ###   ########.fr       */
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

char	**free_build_args(char	**args, int i)
{
	if (args)
	{
		if (i != 0)
			free_result(args, i);
		else
			free(args);
	}
	return (NULL);
}

int	find_num_arguments(t_tree *node)
{
	int		count;
	t_tree	*current;

	count = 0;
	current = node;
	while (current)
	{
		count += 1;
		current = current->right;
	}
	return (count);
}
