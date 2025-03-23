/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 18:27:05 by nkunnath          #+#    #+#             */
/*   Updated: 2025/03/22 18:27:06 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_status(int status)
{
	if (WIFSIGNALED(status))
		return (print_signal_errors(WTERMSIG(status)), 128 + WTERMSIG(status));
	return (WEXITSTATUS(status));
}
