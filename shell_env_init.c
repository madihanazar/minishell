#include "minishell.h"

t_shell	*create_shell(void)
{
	t_shell	*shell;

	shell = (t_shell *)malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->export_list = NULL;
	shell->env = NULL;
	return (shell);
}

void	free_shell(t_shell *shell)
{
	if (shell)
	{
		if (shell->export_list)
			ft_lstclear(&shell->export_list, free);
		free(shell);
	}
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

void	free_env(char **env_copy)
{
	int		i;

	i = 0;
	while (env_copy[i])
	{
		free(env_copy[i]);
		i += 1;
	}
	free(env_copy);
	return ;
}