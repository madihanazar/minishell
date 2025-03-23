/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 21:09:50 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 21:09:50 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	bubble_sort(char **env)
{
	int		i;
	int		j;
	int		size;
	char	*temp;

	i = 0;
	j = 0;
	size = 0;
	while (env[size])
		size += 1;
	while (i < size - 1)
	{
		j = 0;
		while (j < size - i - 1)
		{
			if (ft_strcmp(env[j], env[j + 1]) > 0)
			{
				temp = env[j];
				env[j] = env[j + 1];
				env[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

static void	print_export(char **env)
{
	int		i;
	char	*equals;

	i = 0;
	bubble_sort(env);
	while (env[i])
	{
		equals = ft_strchr(env[i], '=');
		if (equals)
		{
			*equals = '\0';
			printf("declare -x %s=\"%s\"\n", env[i], equals + 1);
		}
		else
			printf("declare -x %s\n", env[i]);
		i++;
	}
}

static int	check_export(char *str)
{
	if (!ft_isalpha(*str) && *str != '_')
		return (0);
	while (*str != '\0' && *str != '=')
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (0);
		str++;
	}
	return (1);
}

bool	add_to_env_list(t_shell *shell, char *str)
{
	t_list	*node;
	char	*temp;
	int		flag;

	flag = 0;
	node = find_node_from_env(str, shell->env_list, &flag);
	if (flag)
		return (false);
	if (node == NULL)
	{
		node = ft_lstnew(str);
		if (!node)
			return (false);
		ft_lstadd_back(&(shell->env_list), node);
		return (true);
	}
	else
	{
		if (ft_strchr(str, '=') != NULL)
		{
			temp = ft_strdup(str);
			if (!temp)
				return (false);
			free(node->content);
			node->content = temp;
			return (true);
		}
	}
	return (true);
}

int	builtin_export(t_shell *shell, char **env)
{
	int		i;
	int		status;
	char	**args;

	i = 1;
	status = 0;
	args = shell->context->args;
	if (!args[1])
		print_export(env);
	else
	{
		while (args[i])
		{
			if (!check_export(args[i]))
				display_export_error(args[i], &status);
			else
			{
				if (!add_to_env_list(shell, args[i]))
					return (ft_putstr_fd("An error has occurred\n", 2), 1);
			}
			i++;
		}
	}
	return (status);
}
