/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:10:51 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 13:40:41 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree	*build_ast(char *str, t_shell *shell)
{
	t_tree	*node;

	node = create_node(str, NODE_COMMAND);
	if (!node)
		return (NULL);
	if (!handle_pipes(node))
		return (ft_putstr_fd("pipes failed\n", 2),
			free_ast(node), NULL);
	if (!handle_redirection(node))
		return (ft_putstr_fd("redirects failed\n", 2),
			free_ast(node), NULL);
	if (!perform_exp(node, shell))
		return (ft_putstr_fd("expansion failed\n", 2),
			free_ast(node), NULL);
	if (!split_args(node))
		return (ft_putstr_fd("split arguments failed\n", 2),
			free_ast(node), NULL);
	if (!handle_quotes(node))
		return (ft_putstr_fd("split quotes failed\n", 2),
			free_ast(node), NULL);
	return (node);
}

char	*ft_strappend(char *str1, char *str2)
{
	char	*temp;

	if (!str1 || !str2)
		return (NULL);
	temp = ft_strjoin(str1, str2);
	free(str1);
	return (temp);
}

char	*get_word(char *str)
{
	while (*str == ' ')
		str++;
	while (*str != '\0')
	{
		if (*str == '"')
			str = ft_strchr(str + 1, '"');
		else if (*str == '\'')
			str = ft_strchr(str + 1, '\'');
		else if (ft_strchr(" |<>\t", *str) != NULL)
			return (str);
		str++;
	}
	return (str);
}

bool	perform_exp(t_tree *node, t_shell *shell)
{
	int	sq;
	int	dq;
	int	i;

	dq = 0;
	sq = 0;
	i = 0;
	if (node->type == HEREDOC)
		return (perform_exp(node->left, shell));
	if (node->type != NODE_COMMAND)
		return (perform_exp(node->left, shell)
			&& perform_exp(node->right, shell));
	while (node->cmd && node->cmd[i])
	{
		if (node->cmd[i] == '"' && !sq)
			dq = !dq;
		else if (node->cmd[i] == '\'' && !dq)
			sq = !sq;
		else if (node->cmd[i] == '$' && !sq)
		{
			if (node->cmd[i + 1] == '\0' || (!ft_isalnum(node->cmd[i + 1])
					&& node->cmd[i + 1] != '_'
					&& node->cmd[i + 1] != '?'
					&& node->cmd[i + 1] != '"'
					&& node->cmd[i + 1] != '\''))
				i++;
			else
			{
				node->cmd[i++] = '\0';
				node->cmd = expanded_str(node->cmd, &node->cmd[i--], shell);
				i--;
			}
		}
		i++;
	}
	return (node->cmd != NULL);
}
