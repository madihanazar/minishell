/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 13:47:31 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 13:48:07 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*find_from_env_list(char *to_find, t_list *env_list)
{
	char	*temp;
	t_list	*node;
	int		length;

	temp = NULL;
	node = env_list;
	length = ft_strlen(to_find);
	while (node)
	{
		if (!ft_strncmp(to_find, node->content, length))
		{
			temp = ft_substr(node->content, length + 1, -1);
			break ;
		}
		node = node->next;
	}
	return (temp);
}

t_list	*find_node_from_env(char *to_find, t_list *env_list, int *flag)
{
	t_list	*node;
	char	*temp;
	char	*key;

	temp = to_find;
	node = env_list;
	while (*temp && *temp != '=')
		temp ++;
	if (*temp == '\0')
		key = ft_strdup(to_find);
	else
		key = ft_substr(to_find, 0, temp - to_find);
	if (!key)
	{
		*flag = 1;
		return (NULL);
	}
	while (node)
	{
		if (!ft_strncmp(key, node->content, ft_strlen(key)))
			return (free(key), node);
		node = node->next;
	}
	return (free(key), NULL);
}

char	*set_new_path(char **args, t_list *env_list, int fd)
{
	char	*new_path;

	if (fd == -1)
		fd = 1;
	if (args[1] == NULL || (ft_strncmp(args[1], "~", 2) == 0))
	{
		new_path = find_from_env_list("HOME", env_list);
		if (new_path == NULL)
			ft_putstr_fd("cd: HOME not set\n", 2);
	}
	else if (ft_strncmp(args[1], "-", 2) == 0)
	{
		new_path = find_from_env_list("OLDPWD", env_list);
		if (new_path == NULL)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", 2);
			return (NULL);
		}
		ft_putendl_fd(new_path, fd);
	}
	else
		new_path = ft_strdup(args[1]);
	return (new_path);
}

int	replace_directory(t_shell *shell, char *old_path, char *new_path)
{
	char	*temp;
	t_list	*node;

	node = shell->env_list;
	while (node)
	{
		if (!ft_strncmp(node->content, "PWD=", 4))
		{
			temp = ft_strjoin("PWD=", new_path);
			if (!temp)
				return (1);
			free(node->content);
			node->content = temp;
		}
		else if (!ft_strncmp(node->content, "OLDPWD=", 7))
		{
			temp = ft_strjoin("OLDPWD=", old_path);
			if (!temp)
				return (1);
			free(node->content);
			node->content = temp;
		}
		node = node->next;
	}
	return (0);
}
