/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 18:27:05 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 17:38:10 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_status(int status)
{
	if (WIFSIGNALED(status))
		return (print_signal_errors(WTERMSIG(status)), 128 + WTERMSIG(status));
	return (WEXITSTATUS(status));
}

static void	ft_putchar_fd(char c, int fd)
{
	if (!fd)
		return ;
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

void	print_signal_errors(int status)
{
	if (status == SIGINT)
		ft_putstr_fd("\n", 2);
	else if (status == SIGQUIT)
	{
		ft_putstr_fd("Quit: ", 2);
		ft_putnbr_fd(status, 2);
		ft_putstr_fd("\n", 2);
	}
	else if (status == SIGSEGV)
	{
		ft_putstr_fd("Segmentation fault: ", 2);
		ft_putnbr_fd(status, 2);
		ft_putstr_fd("\n", 2);
	}
}
