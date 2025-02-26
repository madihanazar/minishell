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
	return (shell);
}

void	free_shell(t_shell *shell)
{
	if (shell)
	{
		if (shell->export_list)
			ft_lstclear(&(shell->export_list), free);
		if (shell->env_list)
			free_env_list(&(shell->env_list));
		if (shell->tree)
			free_ast(shell->tree);
		free(shell);
	}
}

int	env_init(t_list **env_list)
{
	char	*shlvl_str;
	t_list	*shlvl_node;
	t_list	*node;

	shlvl_node = find_node(env_list, "SHLVL", 5);
	shlvl_str = find_shlvl_str(shlvl_node);
	if (!shlvl_str)
		return (1);
	if (shlvl_node)
	{
		free(shlvl_node->content);
		shlvl_node->content = shlvl_str;
	}
	else
	{
		node = ft_lstnew(shlvl_str);
		if (!node)
			return (free(shlvl_str), 1);
		ft_lstadd_back(env_list, node);
		free(shlvl_str);
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
		temp->content = NULL;
		free(temp);
		temp = NULL;
	}
}

t_list	*create_env_node(void *content)
{
	t_list	*new;
	char	*temp;

	if (!content)
		return (NULL);
	new = ft_lstnew(content);
	if (!new)
		return (NULL);
	return (new);
}

t_list	*env_to_list(char **env)
{
	t_list	*head;
	t_list	*new_node;
	int		i;

	if (!env)
		return (NULL);
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
