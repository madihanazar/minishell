/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 17:23:55 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 17:23:55 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_context	*create_context(void)
{
	t_context	*context;

	context = malloc(sizeof(t_context));
	if (!context)
		return (NULL);
	context->args = NULL;
	context->cmd = NULL;
	context->input = -1;
	context->output = -1;
	context->error = 0;
	context->next = NULL;
	return (context);
}

void	free_context_list(t_context *context)
{
	t_context	*curr;
	t_context	*temp;

	curr = context;
	temp = curr;
	while (curr)
	{
		temp = curr->next;
		free_context(curr);
		curr = temp;
	}
}

void	free_context(t_context *context)
{
	if (!context)
		return ;
	if (context->args != NULL)
	{
		free_split(context->args);
		context->args = NULL;
	}
	if (context->cmd)
	{
		free(context->cmd);
		context->cmd = NULL;
	}
	if (context->input >= 0)
		close(context->input);
	context->input = -1;
	if (context->output >= 0)
		close(context->output);
	context->output = -1;
	context->error = 0;
	context->next = NULL;
	free(context);
}
