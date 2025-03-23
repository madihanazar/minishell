/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_builtin_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 18:22:17 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 18:22:17 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_cderror(char *str)
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

int	builtin_cd(t_shell *shell, t_context *context)
{
	int		status;
	char	*new_path;
	char	*old_path;

	status = 0;
	old_path = getcwd(NULL, 0);
	new_path = set_new_path(context->args, shell->env_list, context->output);
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

static void	print_echo(char **argv, int newline, int fd, int i)
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

static int	find_count_of_n(char **argv, int i, int j)
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
			break ;
	}
	return (print_echo(argv, newline, fd, i), 0);
}
