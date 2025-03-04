/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 14:10:51 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/04 15:55:22 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

bool	handle_pipes(t_tree *node)
{
	char	*curr;

	if (node == NULL)
		return (1);
	if (node->type != NODE_COMMAND)
		return (handle_pipes(node->left)
			&& handle_pipes(node->right));
	curr = find_first_pipe(node->cmd);
	if (!curr)		// if pipe is not found
		return (true);
	if (!split_evenly(node->cmd, curr, &node->left, &node->right))
		return (ft_putstr_fd("split pipe failed\node", 2), false);
	// free_null((void **)&node->cmd);
	node->type = PIPE;
	return (handle_pipes(node->left) && handle_pipes(node->right));
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

	left_str = ft_strdup(str);
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
	redir_pos = find_first_redir(node->cmd);
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
	while (env_copy[i])
	{
		free(env_copy[i]);
		i += 1;
	}
	free(env_copy);
	return ;
}
char	*get_env_value_helper(char *name, t_shell *shell)
{
	int		i;
	int		len;
	char	*value;
	char	**env_array;

	i = 0;
	env_array = list_to_env(shell->env_list);
	if (!name || !env_array)
		return (NULL); 
	len = ft_strlen(name);
	while (env_array[i])
	{
		if (ft_strncmp(env_array[i], name, len) == 0 && env_array[i][len] == '=')
		{
			value = ft_strdup(env_array[i] + len + 1); // Automatically, NULL is taken care of
			free_env(env_array);
			return (value);
		}
		i++;
	}
	free_env(env_array);
	return (NULL);
}

char	*get_env_value(char *str, int **i, int len, t_shell *shell)
{
	char	*value;
	char	*var_name;

	var_name = NULL;
	value = NULL;
	if (len == 0)
	{
		value = ft_strdup("$"); // Automatically, NULL is taken care of
		return (value);
	}
	var_name = ft_substr(str, **i, len);
	if (!var_name)
        return (NULL);
	value = get_env_value_helper(var_name, shell);
	free(var_name);
	if (value)
		return (value);
	value = ft_strdup(""); // Automatically, NULL is taken care of
	return (value);
}
char	*expand_var(char *str, int *i, t_shell *shell)
{
	int		len;
	char	*value;

	len = 0;
	value = NULL;
	(*i)++;
	if (str[*i] == '?')
	{
		value = ft_itoa(g_status);
		return (value);
	}
	while (str[*i + len] && (str[*i + len] == '_' || ft_isalnum(str[*i + len])))
		len++;
	value = get_env_value(str, &i, len, shell);
	return (value);
}

bool	perform_exp(t_tree *node, t_shell *shell)
{
	int	sq;
	int	dq;
	int i;

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
			if (expand_var(node->cmd, &i, shell) == NULL)
				return (false);
			node->cmd = expand_var(node->cmd, &i, shell);
		}	
		i++;
	}
	return (true);
}

int	get_token_len(char *str, char split_char)
{
	int	len;
	int	double_quote;
	int	single_quote;

	len = 0;
	double_quote = 0;
	single_quote = 0;
	while (str[len])
	{
		if (str[len] == '"' && !single_quote)
			double_quote = !double_quote;
		else if (str[len] == '\'' && !double_quote)
			single_quote = !single_quote;
		else if (str[len] == split_char && !double_quote && !single_quote)
			break ;
		len++;
	}
	return (len);
}

int	count_tokens(char *str, char split_char, int sq, int dq)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (str[i])
	{
		if (str[i] == '"' && !sq)
			dq = !dq;
		else if (str[i] == '\'' && !dq)
			sq = !sq;
		if (str[i] == split_char && !dq && !sq)
		{
			count++;
			while (str[i + 1] == split_char)
				i++;
		}
		i++;
	}
	return (count + 1);
}
char	*extract_word(char *str, char c)
{
	char	*curr;

	curr = str;
	while (*curr != '\0' && *curr != c)
	{
		if (*curr == '"' || *curr == '\'')
			curr = ft_strchr(curr + 1, *curr);
		curr++;
	}
	return (ft_substr(str, 0, curr - str));
}
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

char	**quote_split(char *str, char split_char)
{
	char	**result;
	int		token_count;

	if (!str)
		return (NULL);
	token_count = count_tokens(str, split_char, 0, 0);
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

bool	split_args(t_tree *node)
{
	if (node->type != NODE_COMMAND)
		return (split_args(node->left) && split_args(node->right));
	node->args = quote_split(node->cmd, ' ');
	if (!node->args)
		return (false);
	return (true);
}
// bool	handle_quotes(t_tree *node)
// {
// 	int		i;
// 	char	**strs;

// 	if (node->type != NODE_COMMAND)
// 		return (handle_quotes(node->left)
// 			&& handle_quotes(node->right));
// 	strs = node->args;
// 	while (*strs)
// 	{
// 		i = 0;
// 		while ((*strs)[i])
// 		{
// 			if ((*strs)[i] == '"' || (*strs)[i] == '\'')
// 				(*strs) = snip_snip((*strs), &i,
// 						ft_strchr(&((*strs)[i + 1]), (*strs)[i]));
// 			if ((*strs) == NULL)
// 				return (false);
// 			i++;
// 		}
// 		strs++;
// 	}
// 	return (true);
// }
t_tree	*build_ast(char *str, t_shell *shell, int *flag)
{
	t_tree	*node;

	node = create_node(str, NODE_COMMAND); //by default type = command
	if (!node)
		return (NULL);
	if (!handle_pipes(node))
		return (ft_putstr_fd("split pipes failed\node", 2),
			free_ast(node), NULL);
	if (!handle_redirection(node))
		return (ft_putstr_fd("split redirects failed\node", 2),
			free_ast(node), NULL);
	if (!perform_exp(node, shell))
		return (ft_putstr_fd("split redirects failed\node", 2),
			free_ast(node), NULL);
	if (!split_args(node))
		return (ft_putstr_fd("split redirects failed\node", 2),
			free_ast(node), NULL);
	return (node);
	// if (!handle_quotes(node))
	// 	return (ft_putstr_fd("split redirects failed\node", 2),
	// 		free_ast(&node), NULL);
}
