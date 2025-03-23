/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_three.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 13:35:15 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 13:36:47 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	count_tokens(char *str, char split_char)
{
	int	count;

	count = 0;
	while (*str)
	{
		while (*str == split_char)
			str++;
		if (*str)
			count++;
		while (*str != '\0' && *str != split_char)
		{
			if (*str == '"' || *str == '\'')
				str = ft_strchr(str + 1, *str);
			str++;
		}
	}
	return (count);
}

char	*extract_word(char *str, char c)
{
	char	*curr;
	char	*temp;

	curr = str;
	while (*curr != '\0' && *curr != c)
	{
		if (*curr == '"' || *curr == '\'')
			curr = ft_strchr(curr + 1, *curr);
		curr++;
	}
	temp = ft_substr(str, 0, curr - str);
	if (!temp)
		return (NULL);
	return (temp);
}

char	*get_var_name(char *str)
{
	char	*word;

	word = str;
	if (ft_isdigit(*str))
	{
		word = ft_strdup(" ");
		if (word)
			word[0] = *str;
		return (word);
	}
	if (*str == '?')
		return (ft_strdup("?"));
	while (*word == '_' || ft_isalnum(*word))
		word++;
	if (word == str)
		word = ft_strdup("");
	else
		word = ft_substr(str, 0, word - str);
	return (word);
}

char	*expanded_str(char *str, char *var, t_shell *shell)
{
	char	*word;
	char	*lstr;
	char	*rstr;
	char	*mstr;	

	word = get_var_name(var);
	if (word == NULL)
		return (free(str), NULL);
	lstr = ft_strdup(str);
	mstr = get_env_value(word, shell);
	rstr = ft_strdup(var + ft_strlen(word));
	free(word);
	free(str);
	if (lstr == NULL || mstr == NULL || rstr == NULL)
		return (free(lstr), free(mstr), free(rstr), NULL);
	lstr = ft_strappend(ft_strappend(lstr, mstr), rstr);
	free(rstr);
	free(mstr);
	return (lstr);
}
