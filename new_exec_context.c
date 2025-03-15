#include "minishell.h"

int get_execution_error(char *cmd)
{
    int errnum;
    DIR *dir;

    errnum = errno;
    ft_putstr_fd("minishell: ", 2);
    ft_putstr_fd(cmd, 2);
    dir = opendir(cmd);
    if (dir)
    {
        closedir(dir);
        if (ft_strchr(cmd, '/'))
            return (ft_putstr_fd(": is a directory\n", 2), 126);
        return (ft_putstr_fd(": Command not found\n", 2), 127);
    }
    if (errnum == EACCES)
        return (ft_putstr_fd(": Permission denied\n", 2), 126);
    if (ft_strchr(cmd, '/') != NULL)
        ft_putstr_fd(": No such file or directory\n", 2);
    else
        ft_putstr_fd(": Command not found\n", 2);
    return (127);
}

int     execute_command(t_context *context, char **env)
{
    int		status;

    free_context_list(context->next);
    if (context->input != -1)
        dup2(context->input, 0), close(context->input);
    if (context->output != -1)
        dup2(context->output, 1), close(context->output);
    if (!context->args[0])
        return (free_env(env), 0);
    if (context->cmd)
        execve(context->cmd, context->args, env);
    status = get_execution_error(context->args[0]);
    return (status);
}


bool    execute_context(t_shell *shell, char **env)
{
    int         status;
    t_context   *next;
	pid_t		pid;
	
	while (shell->context)
    {
        next = shell->context->next;
        pid = fork();
        if (pid == -1)
            return (false);
		if (pid == 0)
		{
			if (is_builtin(shell->context->cmd))
				status = new_execute_builtin(shell, env);
			else
				status = execute_command(shell->context, env);
			free_env(env);
			free_shell(shell);
			exit(status);
		}
        free_context(shell->context);
        shell->context = next;
    }
	waitpid(pid, &status, 0);
	while (wait(NULL) != -1)
		;
    return (true);
}

