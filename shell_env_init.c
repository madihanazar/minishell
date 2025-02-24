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

int	env_init(t_list **env_list)
{
	int		SHLVL;
	char	*SHLVL_str;
	char	*SHLVL_int;
	t_list	*SHLVL_node;
	t_list	*OLDPWD_node;
	t_list	*node;

	SHLVL = 1;
	SHLVL_str = NULL;
	SHLVL_node = find_node(env_list, "SHLVL", 5);
	if (SHLVL_node == NULL)
	{
		SHLVL_int = ft_itoa(SHLVL);
		SHLVL_str = ft_strjoin("SHLVL=", SHLVL_int);
		if (SHLVL_str == NULL)
			return (0);
		node = ft_lstnew(SHLVL_str);
		if (!node)
			return (free(SHLVL_str), 0);
		ft_lstadd_back(env_list, node);
	}
	else
	{
		SHLVL_int = ft_itoa(SHLVL);
		SHLVL_str = SHLVL_node->content;
		SHLVL = atoi(SHLVL_str + 6);
		if (SHLVL < 0)
			SHLVL = 0;
		SHLVL_str = ft_strjoin("SHLVL=", SHLVL_int);
		SHLVL_node->content = SHLVL_str;
	}
	free(SHLVL_int);
	return (1);
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
		free(temp);
	}
}