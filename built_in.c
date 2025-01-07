#include "minishell.h"

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
// int builtin_cd(t_tree *node, char **args, char **env)
// {
//     char *home;
//     char *new_path;

//     if (args[1] == NULL)
//     {
//         home = getenv("HOME");
//         if (home == NULL)
//         {
//             ft_putstr_fd("cd: HOME not set\n", 2);
//             return (1);
//         }
//         new_path = home;
//     }
//     else if (strncmp(args[1], "-", 2) == 0)
//     {
//         new_path = getenv("OLDPWD");
//         if (new_path == NULL)
//         {
//             ft_putstr_fd("cd: OLDPWD not set\n", 2);
//             return (1);
//         }
//         printf("%s\n", new_path);
//     }
//     else
//         new_path = args[1];
//     if (chdir(new_path) != 0)
//         return (perror("cd"), 1);
//     return (0);
// }


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
    // while (argv[i] && argv[i][0] == '-' && argv[i][1] == 'n' && argv[i][2] == '\0')
    // {
    //     newline = 0;
    //     i++;
    // }
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
int execute_builtin(t_tree *node, char **args, char **env)
{
   //if (!ft_strncmp(node->cmd, "cd", 3))
        // return builtin_cd(node, args, env);
    if (!ft_strncmp(node->cmd, "echo", 5))
         return builtin_echo(node, args, env);
    else if (!ft_strncmp(node->cmd, "pwd", 4))
         return builtin_pwd(node, args, env);
    // else if (!ft_strcmp(node->cmd, "export"))
    //     return builtin_export(node, env);
    // else if (!ft_strcmp(node->cmd, "unset"))
    //     return builtin_unset(node, env);
    // else if (!ft_strcmp(node->cmd, "env"))
    //     return builtin_env(env);
    // else if (!ft_strcmp(node->cmd, "exit"))
    //     return builtin_exit(node);
    return (1);
}
