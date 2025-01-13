#include "minishell.h"

// Add this at the start of your function to debug
static void print_env_var(char **env, const char *var_name)
{
    int i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], var_name, ft_strlen(var_name)) == 0)
        {
            printf("Debug: %s\n", env[i]);
            return;
        }
        i++;
    }
    printf("Debug: %s not found\n", var_name);
}


static void	ft_putchar_fd(char c, int fd)
{
	if (!fd)
	{
		return ;
	}
	write(fd, &c, 1);
}

static void	ft_putnbr_fd(int n, int fd)
{
	if (n == -2147483648)
		ft_putstr_fd("-2147483648", fd);
	else if (n < 0)
	{
		ft_putchar_fd('-', fd);
		ft_putnbr_fd(-n, fd);
	}
	else if (n >= 10)
	{
		ft_putnbr_fd(n / 10, fd);
		ft_putchar_fd(n % 10 + 48, fd);
	}
	else
		ft_putchar_fd(n + 48, fd);
}
int is_builtin(char *cmd)
{
    return (!ft_strncmp(cmd, "cd", 3) || !ft_strncmp(cmd, "echo", 5) ||
            !ft_strncmp(cmd, "pwd", 4) || !ft_strncmp(cmd, "export", 7) ||
            !ft_strncmp(cmd, "unset", 6) || !ft_strncmp(cmd, "env", 4) ||
            !ft_strncmp(cmd, "exit", 5));
}
int builtin_cd(t_tree *node, char **args, char **env)
{
    char *home;
    char *new_path;
	char *old_path;
	char *current_path;
	int i;

	i = 0;
	old_path = getcwd(NULL, 0);
    if (args[1] == NULL)
    {
        home = getenv("HOME");
        if (home == NULL)
        {
            ft_putstr_fd("cd: HOME not set\n", 2);
            return (1);
        }
        new_path = home;
    }
    else if (ft_strncmp(args[1], "-", 2) == 0)
    {
        new_path = getenv("OLDPWD");
        if (new_path == NULL)
        {
            ft_putstr_fd("cd: OLDPWD not set\n", 2);
            return (1);
        }
        printf("%s\n", new_path);
    }
    else
        new_path = args[1];
    if (chdir(new_path) != 0)
        return (perror("cd"), 1);
	current_path = getcwd(NULL, 0);
	while (env[i])
	{
		if (!ft_strncmp(env[i], "PWD=", 4))
			env[i] = ft_strjoin("PWD=", current_path);
		else if (!ft_strncmp(env[i], "OLDPWD=", 7))
			env[i] = ft_strjoin("OLDPWD=", old_path);
		i++;
	}
	print_env_var(env, "HOME=");
	print_env_var(env, "PWD=");
	print_env_var(env, "OLDPWD=");
	free(old_path);
	free(current_path);
    return (0);
}


int builtin_echo(t_tree *node, char **argv, char **env)
{
    int i;
    int newline;
    //ft_putstr_fd("im in\n", 2);
    i = 1;
    newline = 1;
    //ft_putchar_fd(argv[1][1], 2);
    
    while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n' && (argv[i][2] == '\0' || argv[i][2] == 'n'))
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
		ft_putstr_fd(argv[i], 2);
        if (argv[i + 1])
            ft_putstr_fd(" ", 2);
        i++;
    }
    if (newline)
    {
        //ft_putstr_fd("im a newline\n", 2);
        ft_putstr_fd("\n", 2);
    }    
    
    return (0);

}

int builtin_pwd(t_tree *node, char **args, char **env)
{
    char *cwd;
    cwd = getcwd(NULL, 0);
    if (!cwd)
        return (ft_putstr_fd("An error has occured", 2), 1);
    ft_putstr_fd(cwd, 2);
    ft_putstr_fd("\n", 1);
    free(cwd);
    return (0);
}

int builtin_env(t_tree *node, char **args, char **env)
{
	int i;

	i = 0;
	while (env[i])
	{
		printf("%s\n", env[i]);
		i++;
	}
	
	return (0);
}

int execute_builtin(t_tree *node, char **args, char **env)
{
   if (!ft_strncmp(node->cmd, "cd", 2))
        return builtin_cd(node, args, env);
    if (!ft_strncmp(node->cmd, "echo", 4))
         return builtin_echo(node, args, env);
    else if (!ft_strncmp(node->cmd, "pwd", 3))
         return builtin_pwd(node, args, env);
    else if (!ft_strncmp(node->cmd, "export", 6))
        return builtin_export(node, args, &env);
    // else if (!ft_strcmp(node->cmd, "unset"))
    //     return builtin_unset(node, env);
    else if (!ft_strncmp(node->cmd, "env",3))
        return builtin_env(node, args, env);
    // else if (!ft_strcmp(node->cmd, "exit"))
    //     return builtin_exit(node);
    return (1);
}
