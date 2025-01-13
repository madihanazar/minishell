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

static void print_export(char **env, char **sort_env)
{
    int len;
    int i;
    char *equals;

    len = 0;
    i = 0;
    while (env[len])
        len++;
    sort_env = malloc(sizeof(char *) * (len + 1));
    if (!sort_env)
        return ;
    while (env[i])
    {
        sort_env[i] = ft_strdup(env[i]);
        i++;
    }
    sort_env[len] = NULL;
    bubble_sort(sort_env, len);
    i = 0;
    while (sort_env[i])
    {
        equals = ft_strchr(sort_env[i], '=');
        *equals = '\0';
        printf("declare -x %s=\"%s\"\n", sort_env[i], equals + 1);
        free(sort_env[i]);
        i++;
    }
    free(sort_env);
}

static int check_export(char *str)
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

// static int add_export(char *str, char ***env)
// {
//     int i;
//     char *key;
//     char *equals;

//     // Find the position of '=' to split key and value
//     equals = ft_strchr(str, '=');
//     if (!equals)
//         return (0); // No '=' means no value to set

//     key = ft_substr(str, 0, equals - str); // Extract the key
//     if (!key)
//         return (0);

//     // Check if key already exists in the environment
//     for (i = 0; (*env)[i]; i++)
//     {
//         if (ft_strncmp((*env)[i], key, ft_strlen(key)) == 0 &&
//             (*env)[i][ft_strlen(key)] == '=')
//         {
//             free((*env)[i]); // Free old value
//             (*env)[i] = ft_strdup(str); // Replace with new value
//             free(key);
//             return (1);
//         }
//     }

//     // Key does not exist, add a new entry
//     char **new_env = malloc(sizeof(char *) * (i + 2)); // +1 for new entry, +1 for NULL
//     if (!new_env)
//     {
//         free(key);
//         return (0);
//     }

//     // Copy old environment to the new array
//     for (int j = 0; j < i; j++)
//         new_env[j] = (*env)[j];

//     new_env[i] = ft_strdup(str); // Add the new entry
//     new_env[i + 1] = NULL;       // NULL-terminate the array

//     free(*env); // Free the old environment
//     *env = new_env; // Update the pointer
//     free(key);

//     return (1);
// }

static int add_export(char *str, char ***env)
{
    if (!str || !env || !*env)
        return (0);

    char *equals = ft_strchr(str, '=');
    char *key;
    size_t key_len;

    // Calculate key length
    if (equals)
        key_len = equals - str;
    else
        key_len = ft_strlen(str);

    // Create temporary key for comparison
    key = malloc(key_len + 1);
    if (!key)
        return (0);
    ft_strcpy(key, str);

    // First pass: check if variable exists
    int i = 0;
    while ((*env)[i])
    {
        if (ft_strncmp((*env)[i], key, key_len) == 0 &&
            ((*env)[i][key_len] == '=' || (*env)[i][key_len] == '\0'))
        {
            // Found the variable, update it
            char *new_value = ft_strdup(str);
            if (!new_value)
            {
                free(key);
                return (0);
            }
            free((*env)[i]);
            (*env)[i] = new_value;
            free(key);
            return (1);
        }
        i++;
    }

    // If we get here, variable doesn't exist
    // Find first NULL slot
    i = 0;
    while ((*env)[i])
        i++;

    // Add new variable at first NULL position
    if (equals)
        (*env)[i] = ft_strdup(str);
    else
        (*env)[i] = ft_strdup(key);

    if (!(*env)[i])
    {
        free(key);
        return (0);
    }

    // Ensure next position is NULL
    (*env)[i + 1] = NULL;
    
    free(key);
    return (1);
}


int builtin_export(t_tree *node, char **args, char ***env)
{
    char **sort_env;
    int i;
    
    i = 1;
    if (!args[1])
        print_export(*env, sort_env);
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
            if (!add_export(args[i], env))
            {
                printf("bash: export: failed to add `%s`\n", args[i]);
            }
            i++;
        }
    }
    return (0);
}