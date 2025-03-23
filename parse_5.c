/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 13:40:11 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 15:03:24 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	handle_pipes(t_tree *node)
{
	char	*curr;

	if (node == NULL)
		return (1);
	if (node->type != NODE_COMMAND)
		return (handle_pipes(node->left)
			&& handle_pipes(node->right));
	curr = find_first_pipe(node->cmd);
	if (!curr)
		return (true);
	if (!split_evenly(node->cmd, curr, &node->left, &node->right))
		return (ft_putstr_fd("split pipe failed\node", 2), false);
	node->type = PIPE;
	return (handle_pipes(node->left) && handle_pipes(node->right));
}

bool	split_evenly(char *str, char *curr, t_tree **left, t_tree **right)
{
	char	*lstr;
	char	*rstr;

	lstr = ft_substr(str, 0, curr - str);
	rstr = ft_substr(str, curr - str + 1, -1);
	if (!lstr || !rstr)
		return (free_strings(lstr, rstr), false);
	*left = create_node(lstr, NODE_COMMAND);
	*right = create_node(rstr, NODE_COMMAND);
	free_strings(lstr, rstr);
	if (!*left || !*right)
		return (false);
	else
		return (true);
}

int	get_len_separator(char *str)
{
	int	i;

	i = 1;
	while (*str == ' ')
		str++;
	while (*str)
	{
		if (*str != ' ' && *str != '\0' && *str != '|' && *str != '>'
			&& *str != '<' && *str != '\t')
		{
			i += 1;
			str++;
		}
		else
			return (i);
	}
	return (i);
}

int	check_symbol(int symbol, char c, int neg, int flag)
{
	if (flag == 2)
	{
		if (c == '"' && !neg)
			return (!symbol);
		else
			return (symbol);
	}
	else
	{
		if (c == '\'' && !neg)
			return (!symbol);
	}
	return (symbol);
}
