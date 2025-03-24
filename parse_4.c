/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 13:37:49 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/24 12:38:35 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	split_redirects(char *str, char *beg, t_tree **left, t_tree **right)
{
	char	*left_str;
	char	*right_str;

	left_str = ft_strdup(str);
	right_str = ft_substr(beg, 0, get_word(beg) - beg);
	if (!left_str || !right_str)
		return (free(left_str), free(right_str), false);
	left_str = ft_strappend(left_str, beg + ft_strlen(right_str));
	if (!left_str)
		return (free(right_str), false);
	*left = create_node(left_str, NODE_COMMAND);
	*right = create_node(right_str, NODE_COMMAND);
	if (!*left || !*right)
		return (free(left_str), free(right_str), false);
	free_strings(left_str, right_str);
	return (true);
}

bool	handle_redirection(t_tree *node)
{
	char	*redir_pos;

	if (node == NULL)
		return (true);
	if (node->type != NODE_COMMAND)
		return (handle_redirection(node->left)
			&& handle_redirection(node->right));
	redir_pos = find_first_redir(node, node->cmd);
	if (!redir_pos)
		return (true);
	*redir_pos = '\0';
	redir_pos++;
	if (node->type == HEREDOC || node->type == APPEND)
		redir_pos++;
	if (!split_redirects(node->cmd, redir_pos, &node->left, &node->right))
		return (false);
	return (handle_redirection(node->left));
}

void	free_env(char **env_copy)
{
	int		i;

	i = 0;
	if (env_copy == NULL)
		return ;
	while (env_copy[i])
	{
		free(env_copy[i]);
		i += 1;
	}
	free(env_copy);
	return ;
}

char	*get_env_value(char *name, t_shell *shell)
{
	int		i;
	char	*find;
	char	**env;
	char	*temp;

	i = 0;
	env = list_to_env(shell->env_list);
	if (!env)
		return (ft_strdup(""));
	if (ft_strncmp(name, "?", 2) == 0)
		return (free_env(env), ft_itoa(g_status));
	find = ft_strjoin(name, "=");
	if (find == NULL)
		return (free_env(env), NULL);
	while (env[i] && ft_strncmp(env[i], find, ft_strlen(find)) != 0)
		i++;
	free(find);
	if (!env[i])
		return (free_env(env), ft_strdup(""));
	temp = ft_strdup(env[i] + ft_strlen(name) + 1);
	return (free_env(env), temp);
}
