#include "minishell.h"

static void	print_export(char **env, char **sort_env, t_list **export_list)
{
	int		len;
	int		i;
	int		env_count;
	int		export_count;
	char	*equals;

	env_count = 0;
	export_count = 0;
	while (env[env_count])
		env_count++;
	t_list *current = *export_list;
	while (current)
	{
		export_count++;
		current = current->next;
	}
	sort_env = malloc(sizeof(char *) * (env_count + export_count + 1));
	if (!sort_env)
		return;
	i = 0;
	while (env[i])
	{
		sort_env[i] = ft_strdup(env[i]);
		i++;
	}
	current = *export_list;
	while (current)
	{
		sort_env[i] = ft_strdup(current->content);
		i++;
		current = current->next;
	}
	sort_env[i] = NULL;
	bubble_sort(sort_env, env_count + export_count);
	i = 0;
	while (sort_env[i])
	{
		equals = ft_strchr(sort_env[i], '=');
		if (equals)
		{
			*equals = '\0';
			printf("declare -x %s=\"%s\"\n", sort_env[i], equals + 1);
		}
		else
		{
			printf("declare -x %s\n", sort_env[i]);
		}
		free(sort_env[i]);
		i++;
	}
	free(sort_env);
}

int check_export(char *str)
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

int	builtin_export(t_tree *node, char **args, char ***env, t_list **export_list)
{
	int		i;
	char	**sort_env;

	i = 1;
	if (!args[1])
		print_export(*env, sort_env, export_list);
	else
	{
		while (args[i])
		{
			if (!check_export(args[i]))
			{
				printf("bash: export: ");
				printf("`%s`: ", args[i]);
				printf("not a valid identifier\n");
			}
			if (ft_strchr(args[i], '='))
			{
				if (add_export(args[i], env, export_list))
				{
					printf("bash: export: failed to add `%s`\n", args[i]);
				}
			}
			else
			{
				if (add_export_1(args[i], env, export_list))
				{
					printf("im isndie func\n");
					printf("bash: export: failed to add `%s`\n", args[i]);
				}
			}
			i++;
		}
	}
	free_split(args);
	return (0);
}