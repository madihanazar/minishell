#include "minishell.h"

static void bubble_sort(char **arr, int size)
{
    int i = 0;
    char *temp;

    while (i < size - 1)
    {
        int j = 0;

        while (j < size - i - 1)
        {
            if (ft_strcmp(arr[j], arr[j + 1]) > 0)
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
            j++;
        }
        i++;
    }
}

static void print_export(char **env, char **sort_env, t_list **export_list)
{
    int len;
    int i;
    int env_count = 0;
    int export_count = 0;
    char *equals;

    while (env[env_count])
        env_count++;

    t_list *current = *export_list;
    while (current)
    {
        export_count++;
        current = current->next;
    }
    sort_env = malloc(sizeof(char *) * (env_count + export_count + 1));
    if (!sort_env)
        return;
    i = 0;
    while (env[i])
    {
        sort_env[i] = ft_strdup(env[i]);
        i++;
    }
    current = *export_list;
    while (current)
    {
        sort_env[i] = ft_strdup(current->content);
        i++;
        current = current->next;
    }
    sort_env[i] = NULL;
    bubble_sort(sort_env, env_count + export_count);
    i = 0;
    while (sort_env[i])
    {
        equals = ft_strchr(sort_env[i], '=');
        if (equals)
        {
            *equals = '\0';
            printf("declare -x %s=\"%s\"\n", sort_env[i], equals + 1);
        }
        else
        {
            printf("declare -x %s\n", sort_env[i]);
        }
        free(sort_env[i]);
        i++;
    }
    free(sort_env);
}

int check_export(char *str)
{
    if (!ft_isalpha(*str) && *str != '_')
		return (0);
	while (*str != '\0' && *str != '=')
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (0);
		str++;
	}
	return (1);
}

static t_list *create_env_node(char *content)
{
    t_list *new;

    if (!content)
        return (NULL);
	new = ft_lstnew((void *)content);
    if (!new)
        return (NULL);
    return (new);
}

t_list *env_to_list(char **env)
{
    t_list *head;
    t_list *new_node;
    int i;

    if (!env || !env[0])
        return (NULL);
    head = create_env_node(env[0]);
    if (!head)
        return (NULL);
    i = 1;
    while (env[i])
    {
        new_node = create_env_node(env[i]);
        if (!new_node)
        {
            ft_lstclear(&head, free);
            return (NULL);
        }
        ft_lstadd_back(&head, new_node);
        i++;
    }
    return (head);
}

char **list_to_env(t_list *list)
{
    char **new_env;
    t_list *current;
    int count;
    int i;

    count = ft_lstsize(list);
    new_env = malloc(sizeof(char *) * (count + 1));
    if (!new_env)
        return (NULL);

    current = list;
    i = 0;
    while (current)
    {
        new_env[i] = ft_strdup(current->content);
        if (!new_env[i])
        {
            while (--i >= 0)
                free(new_env[i]);
            free(new_env);
            return (NULL);
        }
        current = current->next;
        i++;
    }
    new_env[i] = NULL;
    return (new_env);
}
int add_export_1(char *str, char ***env, t_list **export_list)
{
    t_list *current;
    char **env_arr = *env;
    int str_len = ft_strlen(str);
    
    while (*env_arr)
    {
        // Check if this env variable matches our string
        if (!ft_strncmp(*env_arr, str, str_len) && 
            ((*env_arr)[str_len] == '=' || (*env_arr)[str_len] == '\0'))
        {
            return (0);  // Variable exists in env, do nothing
        }
        env_arr++;
    }
    current = *export_list;
    while (current)
    {
        printf("im outside\n");
        if (!ft_strncmp(current->content, str, ft_strlen(str)))
        {
            printf("im same\n");
            return (0);
        }    
        current = current->next;
    }
    printf("im inside\n");
    t_list *new_node = create_env_node(str);
    if (!new_node)
        return (1);

    if (!*export_list)
        *export_list = new_node;
    else
        ft_lstadd_back(export_list, new_node);
    current = *export_list;
    while (current)
    {
        printf("%s\n", (char *)current->content);
        current = current->next;
    }
    return (0);
}
int add_export(char *str, char ***env, t_list **export_list)
{
    t_list *env_list;
    t_list *current;
    char *key;
    char *equals;
    char **new_env;

    equals = ft_strchr(str, '=');
    key = ft_substr(str, 0, equals - str);
    if (!key)
        return (1);
    current = *export_list;
    while (current)
    {
        if (!ft_strncmp(current->content, key, ft_strlen(key)) && 
            (((char *)current->content)[ft_strlen(key)] == '\0' || 
             ((char *)current->content)[ft_strlen(key)] == '='))
        {
            // If we're adding a value (equals exists) and found a match in export_list
            // Remove it from export_list as it will now go to env
            if (equals)
            {
                t_list *temp = *export_list;
                t_list *prev = NULL;
                
                while (temp && ft_strncmp(temp->content, key, ft_strlen(key)) != 0)
                {
                    prev = temp;
                    temp = temp->next;
                }
                
                if (temp)
                {
                    if (prev)
                        prev->next = temp->next;
                    else
                        *export_list = temp->next;
                    free(temp->content);
                    free(temp);
                }
            }
            break;
        }
        current = current->next;
    }
    env_list = env_to_list(*env);
    if (!env_list && *env && (*env)[0])
    {
        free(key);
        return (1);
    }   
    current = env_list;
    printf("\n--- Initial List ---\n");
    t_list *temp = env_list;
    while (temp)
    {
        printf("List node: %s\n", (char *)temp->content);
        temp = temp->next;
    }
    while (current)
    {
       // printf("imside while\n");
        if (!ft_strncmp(current->content, key, ft_strlen(key)) && 
            ((char *)current->content)[ft_strlen(key)] == '=')
        {
            char *new_content = ft_strdup(str);
            printf("new_content %s\n", new_content);
            if (!new_content)
            {
                free(key);
                ft_lstclear(&env_list, free);
                return (0);
            }
            free(current->content);
            current->content = new_content;
            free(key);
            
            new_env = list_to_env(env_list);
            if (!new_env)
            {
                ft_lstclear(&env_list, free);
                return (0);
            }
            
            // if (*env)
            //     free(*env);
            *env = new_env;
            
            ft_lstclear(&env_list, free);
            return (0);
        }
        current = current->next;
    }

    t_list *new_node = create_env_node(str);
    if (!new_node)
    {
        free(key);
        ft_lstclear(&env_list, free);
        return (1);
    }

    if (!env_list)
        env_list = new_node;
    else
        ft_lstadd_back(&env_list, new_node);
    // t_list *curr = env_list;
    // while (curr)
    // {
    //     printf("%s\n", (char *)curr->content);
    //     curr = curr->next;
    // }
    new_env = list_to_env(env_list);
    if (!new_env)
    {
        free(key);
        ft_lstclear(&env_list, free);
        return (1);
    }
    printf("\n--- New Environment Array ---\n");
    int i = 0;
    while (new_env[i])
    {
        printf("new_env[%d]: %s\n", i, new_env[i]);
        i++;
    }

    // if (*env)
    //     free(*env);
    *env = new_env;
    printf("\n--- Final Environment Check ---\n");
    i = 0;
    while ((*env)[i])
    {
        printf("(*env)[%d]: %s\n", i, (*env)[i]);
        i++;
    }


    ft_lstclear(&env_list, free);
    free(key);

    return (0);
}


int builtin_export(t_tree *node, char **args, char ***env, t_list **export_list)
{
    char **sort_env;
    int i;
    
    i = 1;
    if (!args[1])
        print_export(*env, sort_env, export_list);
    else
    {
        while (args[i])
        {
            if (!check_export(args[i]))
            {
                printf("bash: export: ");
                printf("`%s`: ", args[i]);
                printf("not a valid identifier\n");
            }
            if (ft_strchr(args[i], '='))
            {
                if (add_export(args[i], env, export_list))
                {
                    printf("bash: export: failed to add `%s`\n", args[i]);
                }
            }
            else
            {
                if (add_export_1(args[i], env, export_list))
                {
                    printf("im isndie func\n");
                    printf("bash: export: failed to add `%s`\n", args[i]);
                }
            }
            i++;
        }
    }
	free_split(args);
    return (0);
}