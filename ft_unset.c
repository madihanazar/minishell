/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnazar <mnazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 13:26:09 by mnazar            #+#    #+#             */
/*   Updated: 2025/01/19 13:26:09 by mnazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int remove_var(char *str, char ***env)
{
    t_list *env_list;
    t_list *current;
    t_list *prev;
    char *key;
    char **new_env;

    key = ft_strjoin(str, "=");  // to make sure ive checked until we reach = sign
    if (!key)
        return (1);

    env_list = env_to_list(*env);
    if (!env_list && *env && (*env)[0])
    {
        free(key);
        return (1);
    }

    current = env_list;
    prev = NULL;
    while (current)
    {
        if (!ft_strncmp(current->content, key, ft_strlen(key)))
        {
            if (!prev)
                env_list = current->next;
            else
                prev->next = current->next;
		    ft_lstdelone(current, free);
            new_env = list_to_env(env_list);
            if (!new_env)
            {
                ft_lstclear(&env_list, free);
                return (0);
            }
            
            *env = new_env;
            
            ft_lstclear(&env_list, free);
            free(key);
            return (0);
        }
        prev = current;
        current = current->next;
    }
    ft_lstclear(&env_list, free);
    free(key);
    return (0);
}

int builtin_unset(t_tree *node, char **args, char ***env)
{
    int i;

    i = 1;
    while (args[i])
    {
        if (!check_export(args[i]))
        {
            printf("bash: unset: ");
            printf("`%s': ", args[i]);
            printf("not a valid identifier\n");
        }
        remove_var(args[i], env);
        i++;
    }
    return (0);
}