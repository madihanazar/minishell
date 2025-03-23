/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extra_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 23:45:38 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/23 23:49:12 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env_and_shell(char **env, t_shell *shell, int fd, int flag)
{
	if (flag == 0)
		ft_putendl_fd("exit", fd);
	free_env(env);
	free_shell(shell);
}
