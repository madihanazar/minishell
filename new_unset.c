/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_unset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 21:12:31 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 21:12:31 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	display_export_error(char *str, int *status)
{
	ft_putstr_fd("bash: export: ", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd(" not a valid identifier\n", 2);
	*status = 1;
}

static void	remove_from_env_list(t_list **head, char *name)
{
	t_list	*tmp;

	if (*head == NULL)
		return ;
	if (ft_strncmp((*head)->content, name, ft_strlen(name)) == 0)
	{
		tmp = (*head)->next;
		ft_lstdelone(*head, free);
		*head = tmp;
		remove_from_env_list(head, name);
		return ;
	}
	remove_from_env_list(&((*head)->next), name);
}

static void	display_unset_error(char *str)
{
	printf("bash: unset: ");
	printf("`%s': ", str);
	printf("not a valid identifier\n");
}

int	builtin_unset(t_shell *shell)
{
	char	**args;
	int		i;
	int		j;
	int		status;

	i = 1;
	status = 0;
	args = shell->context->args;
	while (shell->context->args[i])
	{
		j = 0;
		if (!ft_isalpha(shell->context->args[i][j]) &&
			shell->context->args[i][j] != '_')
			return (display_unset_error(args[i]), 1);
		while (ft_isalnum(shell->context->args[i][j]) ||
			shell->context->args[i][j] == '_')
			j++;
		if (shell->context->args[i][j] != '\0')
			return (display_unset_error(args[i]), 1);
		remove_from_env_list(&(shell->env_list), args[i]);
		i++;
	}
	return (status);
}
