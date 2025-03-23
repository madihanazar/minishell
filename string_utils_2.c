/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 18:22:52 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 17:53:03 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_word_count(const char *s, char sep)
{
	int	i;
	int	word;

	i = 0;
	word = 0;
	while (s[i])
	{
		if (s[i] != sep && (s[i + 1] == sep || s[i + 1] == '\0'))
			word++;
		i++;
	}
	return (word);
}

static char	*ft_get_word(const char *s, int start, int end)
{
	char	*word;
	int		i;

	i = 0;
	word = (char *)malloc(sizeof(char) * ((end - start + 1) + 1));
	if (!word)
	{
		return (NULL);
	}
	while (start <= end)
		word[i++] = s[start++];
	word[i] = '\0';
	return (word);
}

static char	**malloc_free(char **array, int position)
{
	int	i;

	i = 0;
	while (i < position)
	{
		free(array[i]);
		i++;
	}
	free(array);
	return (NULL);
}

static char	**get_array(const char *s, char c, size_t i, int j)
{
	char	**ptr;
	int		k;

	ptr = (char **)malloc(sizeof(char *) * (ft_word_count(s, c) + 1));
	if (!ptr)
		return (NULL);
	k = 0;
	while (i <= (size_t) ft_strlen(s))
	{
		j = i;
		while (s[i] && s[i] != c)
		{
			if (s[++i] == c || s[i] == '\0')
			{
				ptr[k++] = ft_get_word(s, j, i - 1);
				if (!(ptr[k - 1]))
					return (malloc_free(ptr, k - 1));
			}
		}
		if (s[i++] == '\0')
			break ;
	}
	ptr[k] = NULL;
	return (ptr);
}

char	**ft_split(const char *s, char c)
{
	char	**ptr;

	if (!s)
		return (NULL);
	ptr = get_array(s, c, 0, 0);
	return (ptr);
}
