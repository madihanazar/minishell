/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:10:51 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/02 16:39:49 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	handle_pipes(t_tree *node)
{
	char	*curr;

	if (node == NULL)
		return (1);
	if (node->type != NODE_COMMAND)
		return (split_pipes(node->left)
			&& split_pipes(node->right));
	curr = find_last_pipe(node->cmd);
	if (!curr)		// if pipe is not found
		return (true);
	if (!split_evenly(node->cmd, curr, &node->left, &node->right))
		return (ft_putstr_fd("split pipe failed\n", 2), false);
	// free_null((void **)&node->cmd);
	node->type = PIPE;
	return (split_pipes(node->left) && split_pipes(node->right));
}

bool	split_evenly(char *str, char *curr, t_tree **left, t_tree **right)
{
	char	*lstr;
	char	*rstr;

	lstr = ft_substr(str, 0, curr - str);
	rstr = ft_substr(str, curr - str + 1, -1);
	if (!lstr || !rstr)    //what if lstr is null?
		return (free(lstr), free(rstr), false);
	*left = create_node(lstr, NODE_COMMAND);
	*right = create_node(rstr, NODE_COMMAND);
	if (!*left || !*right)
		return (free(lstr), free(rstr), false);
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
				i+=1;
				str++;
			}
		else
			return (i);
	}
	return (i);
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

bool	split_redirects(char *str, char *beg, t_tree **left, t_tree **right)
{
	char	*left_str;
	char 	*right_str;

	left_str = ft_substr(str, 0, beg);
	right_str = ft_substr(beg, 0, get_len_separator(beg)); //cat << h1 - will still have spaceh1
	if (!left_str || !right_str)
		return (free(left_str), free(right_str), false);
	left_str = ft_strappend(left_str, beg + ft_strlen(right_str));
	if (!left_str)
		return (free(right_str), false);
	*left = create_node(left_str, NODE_COMMAND);
	*right = create_node(right_str, NODE_COMMAND);
	if (!*left || !*right)
		return (free(left_str), free(right_str), false);
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
	redir_pos = find_first_redir(node);
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

bool	handle_to_expand()
{
	
}

t_tree	*build_ast(char *str, t_shell *shell, int *flag)
{
	t_tree	*node;

	node = create_node(str, NODE_COMMAND); //by default type = command
	if (!node)
		return (NULL);
	if (!handle_pipes(node))
		return (ft_putstr_fd("split pipes failed\n", 2),
			free_ast(&node), NULL);
	if (!handle_redirection(str))
		return (ft_putstr_fd("split redirects failed\n", 2),
			free_ast(&node), NULL);
	node = handle_command(str, shell, flag);
	if (*flag)
		return (NULL);
	return (node);
}
