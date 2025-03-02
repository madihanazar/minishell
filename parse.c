#include "minishell.h"


bool	split_pipes(t_tree *node)
{
	char	*curr;

	if (node == NULL)
		return (1);
	if (node->type != NODE_COMMAND)
		return (split_pipes(node->left)
			&& split_pipes(node->right));
	curr = find_last_pipe(node->cmd);
	if (!curr)		// if pipe is not found
		return (true);
	if (!split_evenly(node->cmd, curr, &node->left, &node->right))
		return (ft_putstr_fd("split pipe failed\n", 2), false);
	free_null((void **)&node->cmd);
	node->type = PIPE;
	return (split_pipes(node->left) && split_pipes(node->right));
}

bool	split_evenly(char *str, char *curr, t_tree **left, t_tree **right)
{
	char	*lstr;
	char	*rstr;

	lstr = ft_substr(str, 0, curr - str);
	rstr = ft_substr(str, curr - str + 1, -1);
	if (!lstr || !rstr)    //what if lstr is null?
		return (free(lstr), free(rstr), false);
	*left = create_node(lstr, NODE_COMMAND);
	*right = create_node(rstr, NODE_COMMAND);
	if (!*left || !*right)
		return (free(lstr), free(rstr), false);
	return (true);
}
t_tree	*build_ast(char *str, t_shell *shell, int *flag)
{
	t_tree	*node;

	node = create_node(str, NODE_COMMAND); //by default type = command
	if (!node)
		return (NULL);
	if (!split_pipes(node))
		return (ft_putstr_fd("split pipes failed\n", 2),
			free_ast(&node), NULL);
	node = handle_pipe(str, shell, flag);
	if (*flag)
		return (NULL);
	if (node)
		return (node);
	node = handle_redirection(str, shell, flag);
	if (*flag)
		return (NULL);
	if (node)
		return (node);
	node = handle_command(str, shell, flag);
	if (*flag)
		return (NULL);
	return (node);
}