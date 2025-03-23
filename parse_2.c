/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_two.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 13:33:44 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 13:34:50 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	fill_tokens(char **result, char *str, char split_char)
{
	int	i;
	int	j;
	int	token_len;

	i = 0;
	j = 0;
	while (str[i])
	{
		while (str[i] == split_char)
			i++;
		if (!str[i])
			break ;
		token_len = get_token_len(str + i, split_char);
		result[j] = extract_word(str + i, split_char);
		if (!result[j])
			return (-1);
		i += token_len;
		j++;
	}
	result[j] = NULL;
	return (0);
}

bool	split_args(t_tree *node)
{
	if (node->type != NODE_COMMAND)
		return (split_args(node->left) && split_args(node->right));
	node->args = quote_split(node->cmd, ' ');
	if (!node->args)
		return (false);
	return (true);
}

char	**quote_split(char *str, char split_char)
{
	char	**result;
	int		token_count;

	if (!str)
		return (NULL);
	token_count = count_tokens(str, split_char);
	result = malloc((token_count + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	if (fill_tokens(result, str, split_char) == -1)
	{
		free_result(result, token_count + 1);
		return (NULL);
	}
	return (result);
}

char	*remove_quotes(char *str, int *i, char *end)
{
	char	*lstr;
	char	*mstr;
	char	*rstr;

	lstr = ft_substr(str, 0, *i);
	mstr = ft_substr(str, *i + 1, end - (str + *i + 1));
	rstr = ft_strdup(end + 1);
	*i = end - str - 2;
	free(str);
	if (lstr == NULL || mstr == NULL || rstr == NULL)
		return (free(lstr), free(rstr), free(mstr), NULL);
	lstr = ft_strappend(ft_strappend(lstr, mstr), rstr);
	free(rstr);
	free(mstr);
	return (lstr);
}

bool	handle_quotes(t_tree *node)
{
	int		i;
	char	**strs;

	if (node->type != NODE_COMMAND)
		return (handle_quotes(node->left)
			&& handle_quotes(node->right));
	strs = node->args;
	while (*strs)
	{
		i = 0;
		while ((*strs)[i])
		{
			if ((*strs)[i] == '"' || (*strs)[i] == '\'')
			{
				(*strs) = remove_quotes((*strs), &i,
						ft_strchr(&((*strs)[i + 1]), (*strs)[i]));
			}
			if ((*strs) == NULL)
				return (false);
			i++;
		}
		strs++;
	}
	return (true);
}
