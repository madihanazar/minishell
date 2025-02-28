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

void	free_result(char **result, int count)
{
	int	i;

	i = 0;
	if (!result)
		return ;
	while (i < count)
	{
		free(result[i]);
		i++;
	}
	free(result);
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
		i++;
	}
	free(result);
}

void	free_ast(t_tree *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->cmd)
		free(node->cmd);
	free(node);
}
