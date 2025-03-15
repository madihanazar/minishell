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
	new_path = set_new_path(shell->context->args, shell->env_list, shell->context->output);
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

void	print_echo(char **argv, int newline, int fd, int i)
{
	while (argv[i])
	{
		ft_putstr_fd(argv[i], fd);
		if (argv[i + 1])
			ft_putstr_fd(" ", fd);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", fd);
}

int	find_count_of_n(char **argv, int i, int j)
{
	int	count;

	count = 0;
	while (argv[i][j++] == 'n')
		count++;
	return (count);
}

int	builtin_echo(char **argv, int fd)
{
	int		i;
	int		newline;
	int		j;
	int		count;

	i = 1;
	newline = 1;
	if (fd == -1)
		fd = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n'
			&& (argv[i][2] == '\0' || argv[i][2] == 'n'))
	{
		j = 2;
		count = find_count_of_n(argv, i, j);
		if (count == (ft_strlen(argv[i]) - 2))
		{
			newline = 0;
			i++;
		}
		else
			break;
	}
	return (print_echo(argv, newline, fd, i), 0);
}

int builtin_env(char **env, int fd)
{
	int 	i;
	char	*temp;

	i = 0;
	if (fd == -1)
		fd = 1;
	while (env[i])
	{
		temp = ft_strchr(env[i], '=');
		if (temp == NULL)
			i += 1;
		else
		{
			ft_putendl_fd(env[i], fd);
			i++;
		}
	}
	return (0);
}

int	builtin_pwd(int fd)
{
	char	*cwd;

	if (fd == -1)
		fd = 1;
	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (ft_putstr_fd("An error has occured", 2), 1);
	ft_putendl_fd(cwd, fd);
	free(cwd);
	return (0);
}

bool	exit_atoi(char *str)
{
	int		i;
	int		sign;
	int		answer;

	i = 0;
	answer = 0;
	sign = 1;
	while (str[i] == ' ' || str[i] == '\t' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i++] == '-')
			sign = -1;
	}
	while (ft_isdigit(str[i]))
		answer = ((answer * 10) + (str[i++] - 48)) % 256;
	while (str[i] == ' ' || str[i] == '\t' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i])
	{
		g_status = 255;
		return (false);
	}
	if (sign == -1)
		answer = 256 - answer;
	g_status = answer;
	return (true);
}

int	builtin_exit(t_shell *shell, char **env)
{
	int	fd;

	fd = shell->context->output;
	if (fd == -1)
		fd = 1; // Might need to change
	if (shell->context->args[1] == NULL)
	{
		ft_putendl_fd("exit", fd);
		free_env(env);
		free_shell(shell);
		exit(g_status);
	}
	if (exit_atoi(shell->context->args[1]) == false)
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(shell->context->args[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		(free_env(env), free_shell(shell));
		exit(g_status);
	}
	if (shell->context->args[2] != NULL)
		return (ft_putstr_fd("exit\nminishell: exit: too many arguments\n", 2), 1);
	free_env(env);
	free_shell(shell);
	exit(g_status);
}

int	new_execute_builtin(t_shell *shell, char **env)
{
	if (!ft_strncmp(shell->context->cmd, "cd", 3))
		return (builtin_cd(shell));
	else if (!ft_strncmp(shell->context->cmd, "echo", 5))
		return (builtin_echo(shell->context->args, shell->context->output));
	else if (!ft_strncmp(shell->context->cmd, "env", 4))
		return (builtin_env(env, shell->context->output));
	else if (!ft_strncmp(shell->context->cmd, "pwd", 4))
		return (builtin_pwd(shell->context->output));
	else if (!ft_strncmp(shell->context->cmd, "exit", 5))
		return (builtin_exit(shell, env));
	else if (!ft_strncmp(shell->context->cmd, "export", 7))
		return (builtin_export(shell, env));
	else if (!ft_strncmp(shell->context->cmd, "unset", 6))
		return (builtin_unset(shell));
	// ft_putstr_fd("An error has occured\n", 2);
	return (1);
}