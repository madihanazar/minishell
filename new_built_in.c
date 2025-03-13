#include "minishell.h"

void	print_cderror(char *str)
{
	ft_putstr_fd(str, 2);
	if (errno == ENOTDIR)
		ft_putstr_fd(": Not a directory\n", 2);
	else if (errno == EACCES)
		ft_putstr_fd(": Permission denied\n", 2);
	else if (errno == ELOOP)
		ft_putstr_fd(": Too many levels of symbolic links\n", 2);
	else if (errno == ENAMETOOLONG)
		ft_putstr_fd(": File name too long\n", 2);
	else
		ft_putstr_fd(": No such file or directory\n", 2);
}

int	builtin_cd(t_shell *shell)
{
	int		status;
	char	*new_path;
	char	*old_path;

	status = 0;
	old_path = getcwd(NULL, 0);
	new_path = set_new_path(shell->context->args, shell->env_list);
	if (new_path == NULL)
	{
		free(old_path);
		return (1);
	}
	if (chdir(new_path) != 0)
		return (print_cderror(new_path), free(old_path), free(new_path), 1);
	free(new_path);
	new_path = getcwd(NULL, 0);
	if (replace_directory(shell, old_path, new_path) == 1)
	{
		ft_putstr_fd("An error has occured\n", 2);
		status = 1;
	}
	free(old_path);
	free(new_path);
	return (status);
}

int	builtin_echo(char **argv)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n' 
			&& (argv[i][2] == '\0' || argv[i][2] == 'n'))
	{
		int j = 2;
		int count = 0;
		while (argv[i][j++] == 'n')
			count++;
		if (count == (ft_strlen(argv[i]) - 2))
		{
			newline = 0;
			i++;
		}
		else
			break;
	}
	while (argv[i])
	{
		ft_putstr_fd(argv[i], 1);
		if (argv[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", 1);
	return (0);
}

int builtin_env(char **env)
{
	int 	i;
	char	*temp;

	i = 0;
	while (env[i])
	{
		temp = ft_strchr(env[i], '=');
		if (temp == NULL)
			i += 1;
		else
		{
			write(1, env[i], ft_strlen(env[i]));
			write(1, "\n", 1);
			i++;
		}
	}
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (ft_putstr_fd("An error has occured", 2), 1);
	ft_putstr_fd(cwd, 1);
	ft_putstr_fd("\n", 1);
	free(cwd);
	return (0);
}

int	builtin_exit(t_shell *shell, char **env)
{
	free_env(env);
	free_shell(shell);
	exit(g_status);
}

int	new_execute_builtin(t_shell *shell, char **env)
{
	if (!ft_strncmp(shell->context->cmd, "cd", 3))
		return (builtin_cd(shell));
	else if (!ft_strncmp(shell->context->cmd, "echo", 5))
		return (builtin_echo(shell->context->args));
	else if (!ft_strncmp(shell->context->cmd, "env", 4))
		return (builtin_env(env));
	else if (!ft_strncmp(shell->context->cmd, "pwd", 4))
		return (builtin_pwd());
	else if (!ft_strncmp(shell->context->cmd, "exit", 5))
		builtin_exit(shell, env);
	else if (!ft_strncmp(shell->context->cmd, "export", 7))
		return (builtin_export(shell, env));
	// else if (!ft_strncmp(shell->context->cmd, "unset", 6))
	// 	return builtin_unset(node, args, env);
	// ft_putstr_fd("An error has occured\n", 2);
	return (1);
}