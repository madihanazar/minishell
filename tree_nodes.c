/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_nodes.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 21:02:27 by mnazar            #+#    #+#             */
/*   Updated: 2024/12/23 21:07:22 by mnazar           ###   ########.fr       */
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
	node->heredoc_fd = 0;
	node->cmd = ft_strdup(str);
	if (node->cmd == NULL)
		return (free(node), NULL);
	node->type = type;
	return (node);
}

void free_result(char **result, int count)
{
	if (!result)
		return ;
	int i;
	i = 0;
	while (i < count)
	{
		free(result[i]);
		i++;
	}
	free(result);
}

void free_split(char **result)
{
	if (!result)
		return ;
	int i;
	i = 0;
	while (result[i])
	{
		free(result[i]);
		i++;
	}
	free(result);
}

void free_ast(t_tree *node)
{
    if (!node)
        return ;
    free_ast(node->left);
    free_ast(node->right);
    if (node->cmd)
        free(node->cmd);
    free(node);
}