#include "minishell.h"

char	*find_last_pipe(char *str)
{
	char	*begin;
	int 	single_quote;
	int		double_quote;

	begin = NULL;
	single_quote = 0;
	double_quote = 0;
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
	int 	double_quote;

	last_redir = NULL;
	single_quote = 0;
	double_quote = 0;
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
