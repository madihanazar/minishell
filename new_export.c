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
	int		len;
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

int	check_export(char *str)
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

void	display_export_error(char *str)
{
	ft_putstr_fd("bash: export: ", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd(" not a valid identifier\n", 2);
}

// void	update_env_list(char *temp, t_list *node)
// {
// 	char	*key;

// 	key = ft_strchr(temp, '=');
// 	if (*(key + 1) == '\0')
// 	{
// 		free(temp);
// 		return ;
// 	}
// 	free(node->content);
// 	node->content = temp;
// }

// bool	add_to_env_list(t_shell *shell, char *str)
// {
// 	char	*temp;
// 	t_list	*node;

// 	// temp = ft_strchr(str, '=');
// 	// if (temp == NULL)
// 	// 	temp = ft_strjoin(str, "=");
// 	// else
// 	temp = ft_strdup(str);
// 	if (!temp)
// 		return (false);
// 	node = find_node_from_env(temp, shell->env_list);
// 	if (node == NULL)
// 	{
// 		node = ft_lstnew(temp);
// 		if (!node)
// 			return (free(temp), false);
// 		ft_lstadd_back(&(shell->env_list), node);
// 		free(temp);
// 	}
// 	else
// 		update_env_list(temp, node);
// 	return (true);
// }

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
	char	**args;

	i = 1;
	args = shell->context->args;
	if (!args[1])
		print_export(env);
	else
	{
		while (args[i])
		{
			if (!check_export(args[i]))
				display_export_error(args[i]);
			else
			{
				if (!add_to_env_list(shell, args[i]))
					return (ft_putstr_fd("An error has occurred\n", 2), 1);
			}
			i++;
		}
	}
	return (0);
}
