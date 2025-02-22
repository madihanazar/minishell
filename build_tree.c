#include "minishell.h"

t_tree	*create_tree(char *str, char **env)
{
	char	*trimmed;
	t_tree	*root;

	trimmed = NULL;
	root = NULL;
	trimmed = ft_strtrim(str, " ");
	if (trimmed == NULL)
		return (NULL);
	root = build_ast(trimmed, env);
	free(trimmed); // Trimmed string is freed appropriately.
	return (root);
}

t_tree *build_ast(char *str, char **env)
{
	char	*pipe_pos;
	t_tree	*node;

	if (str == NULL || *str == '\0')
		return (NULL);
	pipe_pos = find_last_pipe(str);
	if (pipe_pos)
	{
		node = create_node("|", PIPE);
		if (!node)
			return (NULL);
		char *left_str = ft_substr(str, 0, pipe_pos - str);
		char *right_str = ft_strdup(pipe_pos + 1);
		node->left = build_ast(left_str, env);
		free(left_str);
		node->right = build_ast(right_str, env);
		free(right_str);
		return (node);
	}
	node = handle_redirection(str, env);
	if (node)
		return (node);
	return (handle_command(str, env));
}

t_tree *handle_redirection(char *str, char **env)
{
	char	*redir_pos;
	t_tree	*node;
	int		len;

	redir_pos = find_last_redir(str);
	if (!redir_pos)
		return (NULL);
	len = 1;
	if (*(redir_pos + 1) == *redir_pos)
		len = 2;
	if (*redir_pos == '>' && *(redir_pos + 1) == '>')
		node = create_node(">>", APPEND);
	else if (*redir_pos == '<' && *(redir_pos + 1) == '<')
		node = create_node("<<", HEREDOC);
	else if (*redir_pos == '>')
		node = create_node(">", REDIR_OUT);
	else if (*redir_pos == '<')
		node = create_node("<", REDIR_IN);
	if (!node)
		return (NULL);
	char *left_str = ft_substr(str, 0, redir_pos - str);
	char *right_str = ft_strdup(redir_pos + 1);
	node->left = build_ast(left_str, env);
	free(left_str);
	node->right = build_ast(right_str, env);
	free(right_str);
	return (node);
}

t_tree *handle_command(char *str, char **env)
{
	char	**cmd_tokens;
	t_tree	*node;
	t_tree	*current;
	int		i;

	i = 1;
	cmd_tokens = NULL;
	node = NULL;
	cmd_tokens = split_cmd(str, ' ', env);
	if (!cmd_tokens || !cmd_tokens[0])
		return (NULL);
	node = create_node(cmd_tokens[0], NODE_COMMAND);
	if (!node)
		return (free_split(cmd_tokens), NULL);
	current = node;
	while (cmd_tokens[i])
	{
		current->right = create_node(cmd_tokens[i], NODE_ARG);
		if (!current->right)
			break ;
		current = current->right;
		i++;
	}
	free_split(cmd_tokens);
	return (node);
}

char **split_cmd(char *str, char split_char, char **env)
{
    int token_count;
    char **result;
    
    if (!str)
        return (NULL);
    token_count = count_tokens(str, split_char);
    result = malloc((token_count + 1) * sizeof(char *));
    if (!result)
        return (NULL);
    if (fill_tokens(result, str, split_char, env) == -1)
    {
        free_result(result, token_count + 1);
        return (NULL);
    }
    return result;
}

int count_tokens(char *str, char split_char)
{
	int i = 0;
	int dq = 0;
	int sq = 0;
	int count = 0;

	while (str[i]) 
	{
		if (str[i] == '"' && !sq)
			dq = !dq;
		else if (str[i] == '\'' && !dq)
			sq = !sq;
		if (str[i] == split_char && !dq && !sq) {
			count++;
			while (str[i + 1] == split_char)
				i++;
		}
		i++;
	}
	return count + 1;
}

int fill_tokens(char **result, char *str, char split_char, char **env)
{
    int i = 0;
    int j = 0;
    int token_len;

    while (str[i]) {
        while (str[i] == split_char)
            i++;
        if (!str[i])
            break;
        token_len = get_token_len(str + i, split_char);
        result[j] = extract_token(str + i, token_len, env);
		// printf("Fill tokens output %s\n", result[j]);
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
			break;
		len++;
	}
	return (len);
}

char	*extract_token(char *str, int len, char **env)
{
	t_exp	exp;

	exp.i = 0;
	exp.j = 0;
	exp.sq = 0;
	exp.dq = 0;
	exp.len = len;
	if (!init_extract(len, &exp.token))
		return (NULL);
	while (exp.i < len)
	{
		if (str[exp.i] == '"' && !exp.sq)
			handle_quotes(&exp.i, &exp.dq);
		else if (str[exp.i] == '\'' && !exp.dq)
			handle_quotes(&exp.i, &exp.sq);
		else if (str[exp.i] == '$' && !exp.sq)
		{
			if (!handle_expansion(str, &exp, env))
				return (NULL);
			else
				break ;
		}
		else
			exp.token[exp.j++] = str[exp.i++];
	}
	exp.token[exp.j] = '\0';
	// printf("handle expansion output: %s\n", exp.token);
	return (exp.token);
}

char	*init_extract(int len, char **token)
{
	*token = malloc(sizeof(char) * (len + 1));
	if (!*token)
		return (NULL);
	ft_bzero(*token, len + 1);
	return (*token);
}

void	handle_quotes(int *i, int *quote_flag)
{
	*quote_flag = !(*quote_flag);
	(*i)++;
}

int	handle_expansion(char *str, t_exp *exp, char **env)
{
	char	*val;
	int		val_len;
	char	*new_tok;

	val = expand_var(str, &exp->i, env);
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
	// printf("In handle expansion: %s\n", exp->token);
	exp->j += val_len;
	free(val);
	return (1);
}

void print_ast(t_tree *node, int depth)
{
    if (node != NULL)
    {
        // Print the left subtree first
        if (node->left)
        {
            print_ast(node->left, depth + 1);
        }

        // Print the current node with indentation based on depth
        for (int i = 0; i < depth; i++)
            printf("    "); // Indentation for visualization
        printf("Depth: %d, Command: %s\n", depth, node->cmd);

        // Print the right subtree
        if (node->right)
        {
            print_ast(node->right, depth + 1);
        }
    }
}

static void test_ast(char *command, char **env) {
    printf("\nTesting command: %s\n", command);
    t_tree *root = create_tree(command, env);
    printf("AST structure:\n");
    print_ast(root, 0);
    printf("------------------------\n");
    // Add function to free the tree
    free_ast(root);
}
