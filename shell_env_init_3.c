/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_env_init_three.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 18:21:23 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/22 18:22:14 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*assign_pwds(char *str)
{
	t_list	*new_node;
	char	*temp;
	char	*final_string;

	temp = getcwd(NULL, 0);
	if (!temp)
		return (NULL);
	final_string = ft_strjoin(str, temp);
	if (!final_string)
		return (free(temp), NULL);
	new_node = ft_lstnew(final_string);
	if (!new_node)
		return (free(temp), free(final_string), NULL);
	free(temp);
	free(final_string);
	return (new_node);
}

t_list	*handle_empty_env(void)
{
	t_list	*head;

	head = ft_lstnew("SHLVL=1");
	if (!head)
		return (NULL);
	head->next = assign_pwds("OLDPWD=");
	if (!head->next)
		return (ft_lstclear(&head, free), NULL);
	head->next->next = assign_pwds("PWD=");
	if (!head->next->next)
		return (ft_lstclear(&head, free), NULL);
	return (head);
}

t_list	*env_to_list(char **env)
{
	t_list	*head;
	t_list	*new_node;
	int		i;

	if (!env[0])
		return (handle_empty_env());
	head = create_env_node(env[0]);
	if (!head)
		return (NULL);
	i = 1;
	while (env[i])
	{
		new_node = create_env_node(env[i]);
		if (!new_node)
		{
			ft_lstclear(&head, free);
			return (NULL);
		}
		ft_lstadd_back(&head, new_node);
		i++;
	}
	return (head);
}

char	**list_to_env(t_list *list)
{
	int		i;
	int		count;
	t_list	*current;
	char	**new_env;

	count = ft_lstsize(list);
	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);
	current = list;
	i = 0;
	while (current)
	{
		new_env[i] = ft_strdup(current->content);
		if (!new_env[i])
		{
			while (--i >= 0)
				free(new_env[i]);
			return (free(new_env), NULL);
		}
		current = current->next;
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}
