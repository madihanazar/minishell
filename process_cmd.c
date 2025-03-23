/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_cmd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 17:46:18 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 17:46:18 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*join_path(char *path, char *args)
{
	char	*temp;
	char	*full_cmd;

	temp = ft_strjoin(path, "/");
	if (temp == NULL)
		return (NULL);
	full_cmd = ft_strjoin(temp, args);
	free(temp);
	return (full_cmd);
}

static char	*extract_path(char *cmd, char **env)
{
	int		i;
	char	**paths;
	char	*full_cmd;

	i = 0;
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	while (env[i] != NULL && (ft_strncmp(env[i], "PATH=", 5) != 0))
		i++;
	if (env[i] == NULL)
		return (NULL);
	paths = ft_split(env[i] + 5, ':');
	if (paths == NULL)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_cmd = join_path(paths[i++], cmd);
		if (full_cmd == NULL)
			return (free_split(paths), NULL);
		if (access(full_cmd, X_OK) == 0)
			return (free_split(paths), full_cmd);
		free(full_cmd);
	}
	return (free_split(paths), NULL);
}

bool	process_command(t_context *context, t_tree *node, char **env)
{
	context->args = node->args;
	node->args = NULL;
	if (!context->args)
		return (false);
	if (context->args[0])
	{
		if (is_builtin(context->args[0]))
			context->cmd = ft_strdup(context->args[0]);
		else
			context->cmd = extract_path(context->args[0], env);
	}
	return (true);
}

bool	check_heredoc(t_tree *node)
{
	if (node == NULL)
		return (false);
	if (node->type == HEREDOC)
		return (true);
	return (check_heredoc(node->left));
}
