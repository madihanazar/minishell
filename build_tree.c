#include "minishell.h"

t_tree	*create_tree(char *str, t_shell *shell)
{
	int		flag;
	t_tree	*root;
	char	*trimmed;

	flag = 0;
	root = NULL;
	trimmed = NULL;
	trimmed = ft_strtrim(str, " ");
	if (trimmed == NULL)
		return (NULL);
	root = build_ast(trimmed, shell, &flag);
	free(trimmed);
	return (root);
}

t_tree	*build_ast(char *str, t_shell *shell, int *flag)
{
	t_tree	*node;

	if (*flag)
		return (NULL);
	node = handle_pipe(str, shell, flag);
	if (*flag)
		return (NULL);
	if (node)
		return (node);
	node = handle_redirection(str, shell, flag);
	if (*flag)
		return (NULL);
	if (node)
		return (node);
	node = handle_command(str, shell, flag);
	if (*flag)
		return (NULL);
	return (node);
}

t_tree	*handle_pipe(char *str, t_shell *shell, int *flag)
{
	t_tree	*node;
	char	*pipe_pos;
	char	*left_str;
	char	*right_str;

	node = NULL;
	pipe_pos = find_last_pipe(str);
	if (pipe_pos)
	{
		node = create_node("|", PIPE);
		if (!node)
			return (free_handle_pipe_redir(flag, NULL, NULL));
		left_str = ft_substr(str, 0, pipe_pos - str);
		if (!left_str)
			return (free_handle_pipe_redir(flag, node, NULL));
		right_str = ft_strdup(pipe_pos + 1);
		if (!right_str)
			return (free_handle_pipe_redir(flag, node, left_str));
		node->left = build_ast(left_str, shell, flag);
		node->right = build_ast(right_str, shell, flag);
		free_strings(left_str, right_str);
		if (*flag)
			return (free_handle_pipe_redir(flag, node, NULL));
	}
	return (node);
}

t_tree	*create_redir_node(char *redir_pos)
{
	t_tree	*node;

	if (*redir_pos == '>' && *(redir_pos + 1) == '>')
		node = create_node(">>", APPEND);
	else if (*redir_pos == '<' && *(redir_pos + 1) == '<')
		node = create_node("<<", HEREDOC);
	else if (*redir_pos == '>')
		node = create_node(">", REDIR_OUT);
	else if (*redir_pos == '<')
		node = create_node("<", REDIR_IN);
	return (node);
}

int	calculate_length(char *redir_pos)
{
	int	len;

	len = 1;
	if (*(redir_pos + 1) == *redir_pos)
		len = 2;
	return (len);
}

t_tree	*handle_redirection(char *str, t_shell *shell, int *flag)
{
	int		len;
	t_tree	*node;
	char	*left_str;
	char	*right_str;
	char	*redir_pos;

	node = NULL;
	redir_pos = find_last_redir(str);
	if (redir_pos)
	{
		len = calculate_length(redir_pos);
		node = create_redir_node(redir_pos);
		if (!node)
			return (free_handle_pipe_redir(flag, NULL, NULL));
		left_str = ft_substr(str, 0, redir_pos - str);
		if (!left_str)
			return (free_handle_pipe_redir(flag, node, NULL));
		right_str = ft_strdup(redir_pos + len);
		if (!right_str)
			return (free_handle_pipe_redir(flag, node, left_str));
		node->left = build_ast(left_str, shell, flag);
		node->right = build_ast(right_str, shell, flag);
		free_strings(left_str, right_str);
		if (*flag)
			return (free_handle_pipe_redir(flag, node, right_str));
	}
	return (node);
}

t_tree	*handle_command(char *str, t_shell *shell, int *flag)
{
	int		i;
	t_tree	*node;
	t_tree	*current;
	char	**cmd_tokens;

	i = 1;
	cmd_tokens = NULL;
	node = NULL;
	cmd_tokens = split_cmd(str, ' ', shell);
	if (!cmd_tokens || !cmd_tokens[0])
		return (free_commands(flag, NULL, NULL, NULL));
	node = create_node(cmd_tokens[0], NODE_COMMAND);
	if (!node)
		return (free_commands(flag, cmd_tokens, NULL, NULL));
	current = node;
	while (cmd_tokens[i])
	{
		current->right = create_node(cmd_tokens[i], NODE_ARG);
		if (!current->right)
			return (free_commands(flag, cmd_tokens, node, NULL));
		current = current->right;
		i++;
	}
	free_split(cmd_tokens);
	return (node);
}

char	**split_cmd(char *str, char split_char, t_shell *shell)
{
	char	**result;
	int		token_count;

	if (!str)
		return (NULL);
	token_count = count_tokens(str, split_char, 0, 0);
	result = malloc((token_count + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	if (fill_tokens(result, str, split_char, shell) == -1)
	{
		free_result(result, token_count + 1);
		return (NULL);
	}
	return (result);
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

int	fill_tokens(char **result, char *str, char split_char, t_shell *shell)
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
		result[j] = extract_token(str + i, token_len, shell);
		if (!result[j])
			return (-1);
		i += token_len;
		j++;
	}
	result[j] = NULL;
	return (0);
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

char	*extract_token(char *str, int len, t_shell *shell)
{
	t_exp	exp;

	if (!init_extract(len, &exp))
		return (NULL);
	while (exp.i < len)
	{
		if (str[exp.i] == '"' && !exp.sq)
			handle_quotes(&exp.i, &exp.dq);
		else if (str[exp.i] == '\'' && !exp.dq)
			handle_quotes(&exp.i, &exp.sq);
		else if (str[exp.i] == '$' && !exp.sq)
		{
			if (!handle_expansion(str, &exp, shell))
				return (NULL);
			else
				break ;
		}
		else
			exp.token[exp.j++] = str[exp.i++];
	}
	exp.token[exp.j] = '\0';
	return (exp.token);
}

int	init_extract(int len, t_exp *exp)
{
	(*exp).i = 0;
	(*exp).j = 0;
	(*exp).sq = 0;
	(*exp).dq = 0;
	(*exp).len = len;
	(*exp).token = malloc(sizeof(char) * (len + 1));
	if (!((*exp).token))
		return (0);
	ft_bzero(((*exp).token), len + 1);
	return (1);
}

void	handle_quotes(int *i, int *quote_flag)
{
	*quote_flag = !(*quote_flag);
	(*i)++;
}

int	handle_expansion(char *str, t_exp *exp, t_shell *shell)
{
	char	*val;
	char	*new_tok;
	int		val_len;

	val = expand_var(str, &exp->i, shell);
	if (!val)
		return (free(exp->token), 0);
	val_len = ft_strlen(val);
	if (exp->j + val_len + (exp->len - exp->i) >= exp->len + 1)
	{
		new_tok = malloc(exp->j + val_len + (exp->len - exp->i) + 1);
		if (!new_tok)
		{
			free(val);
			free(exp->token);
			return (0);
		}
		ft_strcpy(new_tok, exp->token);
		free(exp->token);
		exp->token = new_tok;
	}
	ft_strcpy(exp->token + exp->j, val);
	exp->j += val_len;
	free(val);
	return (1);
}
