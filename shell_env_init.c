/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_env_init.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 14:04:26 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/22 18:21:05 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_shell	*create_shell(void)
{
	t_shell	*shell;

	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->export_list = NULL;
	shell->env_list = NULL;
	shell->tree = NULL;
	shell->context = NULL;
	return (shell);
}

void	free_shell(t_shell *shell)
{
	if (shell)
	{
		if (shell->export_list)
			free_env_list(&(shell->export_list));
		if (shell->env_list)
			free_env_list(&(shell->env_list));
		if (shell->tree)
			free_ast(shell->tree);
		if (shell->context)
			free_context_list(shell->context);
		free(shell);
	}
}

int	assign_shlvl_node(t_list **env_list)
{
	t_list	*shlvl_node;
	char	*shlvl_str;
	t_list	*node;

	shlvl_node = find_node(env_list, "SHLVL", 5);
	shlvl_str = find_shlvl_str(shlvl_node);
	if (!shlvl_str)
		return (0);
	if (shlvl_node)
	{
		free(shlvl_node->content);
		shlvl_node->content = shlvl_str;
	}
	else
	{
		node = ft_lstnew(shlvl_str);
		if (!node)
			return (free(shlvl_str), 0);
		ft_lstadd_back(env_list, node);
		free(shlvl_str);
	}
	return (1);
}

int	env_init(t_list **env_list)
{
	t_list	*oldpwd_node;
	t_list	*temp;

	if (!assign_shlvl_node(env_list))
		return (1);
	oldpwd_node = find_node(env_list, "OLDPWD", 6);
	if (oldpwd_node)
		return (0);
	else
	{
		temp = ft_lstnew("OLDPWD=");
		if (!temp)
			return (1);
		ft_lstadd_back(env_list, temp);
	}
	return (0);
}

t_list	*find_node(t_list **env_list, char *str, int len)
{
	t_list	*node;

	node = *env_list;
	while (node)
	{
		if (ft_strncmp(node->content, str, len) == 0)
			return (node);
		node = node->next;
	}
	return (NULL);
}
