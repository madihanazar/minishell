/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_env_init_two.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 18:20:04 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/22 18:22:00 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_shlvl_str(t_list	*shlvl_node)
{
	char	*curr_str;
	int		shlvl_val;
	char	*final_str;
	char	*shlvl_itoa;

	if (!shlvl_node)
	{
		final_str = malloc(sizeof(char) * 8);
		if (!final_str)
			return (NULL);
		ft_strcpy(final_str, "SHLVL=1");
	}
	else
	{
		curr_str = (char *)shlvl_node->content;
		shlvl_val = assign_shlvl_val(ft_atoi(curr_str + 6));
		shlvl_itoa = ft_itoa(shlvl_val);
		if (!shlvl_itoa)
			return (NULL);
		final_str = ft_strjoin("SHLVL=", shlvl_itoa);
		free(shlvl_itoa);
		if (!final_str)
			return (NULL);
	}
	return (final_str);
}

int	assign_shlvl_val(int shlvl_val)
{
	if (shlvl_val < 0)
		return (0);
	else
		shlvl_val += 1;
	if (shlvl_val < 0)
		return (0);
	return (shlvl_val);
}

char	**create_env_copy(char **env)
{
	int		i;
	int		total;
	char	**env_copy;
	char	*test_str;

	i = 0;
	total = 0;
	test_str = NULL;
	while (env[total])
		total += 1;
	env_copy = (char **)malloc(sizeof(char *) * (total + 1));
	if (env_copy == NULL)
		return (NULL);
	i = 0;
	while (env[i])
	{
		test_str = ft_strdup(env[i]);
		if (test_str == NULL)
			return (free_result(env_copy, total), NULL);
		env_copy[i] = test_str;
		i += 1;
	}
	env_copy[i] = NULL;
	return (env_copy);
}

void	free_env_list(t_list **node)
{
	t_list	*start;
	t_list	*temp;

	start = *node;
	while (start)
	{
		temp = start;
		start = start->next;
		free(temp->content);
		temp->content = NULL;
		free(temp);
		temp = NULL;
	}
}

t_list	*create_env_node(void *content)
{
	t_list	*new;

	if (!content)
		return (NULL);
	new = ft_lstnew(content);
	if (!new)
		return (NULL);
	return (new);
}
