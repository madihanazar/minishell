#include "minishell.h"

static void	print_signal_errors(int status)
{
	if (status == SIGINT)
		ft_putstr_fd("\n", 2);
	else if (status == SIGQUIT)
		printf("Quit: %d\n", status);
	else if (status == SIGSEGV)
		printf("Segmentation fault: %d\n", status);
}

int	check_status(int status)
{
	if (WIFSIGNALED(status))
		return (print_signal_errors(WTERMSIG(status)), 128 + WTERMSIG(status));
	return (WEXITSTATUS(status));
}