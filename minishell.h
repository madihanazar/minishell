/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 20:50:09 by mnazar            #+#    #+#             */
/*   Updated: 2024/12/23 20:51:34 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <stdlib.h>

typedef struct s_tree
{
	struct s_tree	*left;
	struct s_tree	*right;
	char			*cmd;
}	t_tree;
char	*ft_substr(char const *s, unsigned int start, size_t len);
#endif