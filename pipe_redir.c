#include "minishell.h"

char	*find_last_pipe(char *str)
{
	char	*begin;
	int		single_quote;
	int		double_quote;

	begin = NULL;
	single_quote = 0;
	double_quote = 0;
	if (str == NULL || *str == '\0')
		return (NULL);
	while (*str)
	{
		if (*str == '"')
			double_quote = !double_quote;
		else if (*str == '\'')
			single_quote = !single_quote;
		if (*str == '|' && !double_quote && !single_quote)
			begin = str;
		str++;
	}
	return (begin);
}

char	*find_last_redir(char *str)
{
	char	*last_redir;
	int		single_quote;
	int		double_quote;

	last_redir = NULL;
	single_quote = 0;
	double_quote = 0;
	if (str == NULL || *str == '\0')
		return (NULL);
	while (*str)
	{
		if (*str == '"')
			double_quote = !double_quote;
		else if (*str == '\'')
			single_quote = !single_quote;
		if ((*str == '<' || *str == '>') && !double_quote && !single_quote)
		{
			last_redir = str;
			if (*(str + 1) == *str)
				str++;
		}
		str++;
	}
	return (last_redir);
}

t_tree	*free_handle_pipe_redir(int *flag, t_tree *node, char *str)
{
	*flag = 1;
	if (node)
		free_ast(node);
	if (str)
		free(str);
	return (NULL);
}

t_tree	*free_commands(int *flag, char **cmd_tokens, t_tree *node, char *str)
{
	if (cmd_tokens)
		free_split(cmd_tokens);
	return (free_handle_pipe_redir(flag, node, str));
}

void	free_strings(char *str1, char *str2)
{
	if (str1)
		free(str1);
	if (str2)
		free(str2);
}
