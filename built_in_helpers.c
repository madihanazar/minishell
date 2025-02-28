#include "minishell.h"

char	*set_new_path(char **args)
{
	char	*new_path;

	if (args[1] == NULL)
	{
		new_path = getenv("HOME");
		if (new_path == NULL)
			ft_putstr_fd("cd: HOME not set\n", 2);
	}
	else if (ft_strncmp(args[1], "-", 2) == 0)
	{
		new_path = getenv("OLDPWD");
		if (new_path == NULL)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", 2);
			return (NULL);
		}
		printf("%s\n", new_path);
	}
	else
		new_path = args[1];
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
}
