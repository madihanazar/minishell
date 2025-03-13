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

int	builtin_export(t_shell *shell, char **env)
{
	int		i;
	char	**args;

	i = 1;
	args = shell->context->args;
	if (!args[1])
		print_export(env);
	// else
	// {
	// 	while (args[i])
	// 	{
	// 		if (!check_export(args[i]))
	// 		{
	// 			ft_putstr_fd("bash: export: ", 2);
	// 			ft_putstr_fd(args[i], 2);
	// 			ft_putstr_fd("not a valid identifier\n", 2);
	// 		}
	// 		if (ft_strchr(args[i], '='))
	// 		{
	// 			if (add_export(args[i], env, export_list))
	// 				printf("bash: export: failed to add `%s`\n", args[i]);
	// 		}
	// 		else
	// 		{
	// 			if (add_export_1(args[i], env, export_list))
	// 			{
	// 				printf("im isndie func\n");
	// 				printf("bash: export: failed to add `%s`\n", args[i]);
	// 			}
	// 		}
	// 		i++;
	// 	}
	// }
	return (0);
}
