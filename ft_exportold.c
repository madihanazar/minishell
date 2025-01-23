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
// static int add_export(char *str, char ***env)
// {
//     int i = 0;
//     char *new_value;
//     char **new_env;
//     char *key;
//     char *equals;

//     // For empty export command or just a key without '='
//     equals = ft_strchr(str, '=');
//     if (!equals)
//         return (1);  // Valid but nothing to add to env

//     // Get key length
//     key = ft_substr(str, 0, equals - str);
//     if (!key)
//         return (0);
// 	printf("im in\n");
// 	new_env = malloc(sizeof(char *) * (i + 2));  
// 	if (!new_env)
//     {
//         free(key);
//         return (0);
//     }
// 	while((*env)[i])
// 		i++;
// 	int j = 0;
// 	for (j = 0; j < i; j++)
//         new_env[j] = (*env)[j];
// 	new_env[j + 1] = NULL;
// 	i = 0;
//     // Look for existing key
//     while ((*new_env)[i])
//     {
//         printf("im in\n");
// 		printf("%s\n", (*new_env)[i]);
// 		printf("%s\n", key);
// 		if (ft_strncmp((*new_env)[i], key, ft_strlen(key)) == 0 && 
//             (*new_env)[i][ft_strlen(key)] == '=')
//         {
// 			printf("im in\n");
//             new_value = ft_strdup(str);
//             if (!new_value)
//             {
//                 free(key);
//                 return (0);
//             }
// 			printf("Freeing (*env)[%d]: %s\n", i, (*env)[i]);

//             free((*new_env)[i]);  // Free only if we successfully created new_value
//             (*new_env)[i] = new_value;
//             free(key);
//             return (1);
//         }
//         i++;
//     }

//     // Add new entry
//     //new_env[i] = ft_strdup(str);
//     // if (!new_env[i])
//     // {
//     //     free(key);
//     //     free(new_env);
//     //     return (0);
//     // }
//     // new_env[i + 1] = NULL;

//     // Update environment pointer
//     free(*env);
//     *env = new_env;
//     free(key);

//     return (1);
// }

// static int add_export(char *str, char ***env)
// {
//     int i = 0;
//     char **new_env;

//     // Skip if no '='
//     if (!ft_strchr(str, '='))
//         return (1);

//     // Count current environment variables
//     while ((*env)[i])
//         i++;

//     // Allocate new environment array
//     new_env = malloc(sizeof(char *) * (i + 2));
//     if (!new_env)
//         return (0);

//     // Copy pointers to existing environment strings
//     for (int j = 0; j < i; j++)
//         new_env[j] = (*env)[j];

//     // Add new entry
//     new_env[i] = ft_strdup(str);
//     if (!new_env[i])
//     {
//         free(new_env);
//         return (0);
//     }
//     new_env[i + 1] = NULL;

//     // Only free the array pointer, not the strings
//     if (*env)
//         free(*env);
//     *env = new_env;

//     return (1);
// }

static int add_export(char *str, char ***env)
{
    int i = 0;
    char *new_value;
    char **new_env;
    char *key;
    char *equals;

    // For empty export command or just a key without '='
    equals = ft_strchr(str, '=');
    if (!equals)
        return (1);  // Valid but nothing to add to env

    // Get key length
    key = ft_substr(str, 0, equals - str);
    if (!key)
        return (0);
	printf("im in\n");
    // Count environment variables
    while ((*env)[i])
        i++;

    // Create new environment array
    new_env = malloc(sizeof(char *) * (i + 2));  
    if (!new_env)
    {
        free(key);
        return (0);
    }

    // Copy existing environment to new array
	printf("im in\n");
    for (int j = 0; j < i; j++)
        new_env[j] = ft_strdup((*env)[j]);  // Create copies of strings
    new_env[i] = NULL;  // Null terminate the array
    // Look for existing key in new array
    i = 0;
    while (new_env[i])
    {
        //printf("%s\n", new_env[i]);
		printf("im in\n");
		if (!ft_strncmp(new_env[i], key, ft_strlen(key)) && 
            new_env[i][ft_strlen(key)] == '=')
        {
            // Update existing entry
			printf("im batman\n");
            new_value = ft_strdup(str);
			//printf("%s\n", new_value);
            if (!new_value)
            {
                // Cleanup if strdup fails
                while (new_env[i])
                {
                    free(new_env[i]);
                    i++;
                }
                free(new_env);
                free(key);
                return (0);
            }
			printf("im superman\n");
            free(new_env[i]);  // Now safe to free as this is our copy
			printf("im catwoman\n");
            new_env[i] = new_value;
            free(key);
            
            // if (*env)
            //     free(*env);
            *env = new_env;
            return (1);
        }
        i++;
    }

    // Key not found, add new entry
    new_env[i] = ft_strdup(str);
    if (!new_env[i])
    {
        // Cleanup if strdup fails
        int j = 0;
        while (new_env[j])
        {
            free(new_env[j]);
            j++;
        }
        free(new_env);
        free(key);
        return (0);
    }
    new_env[i + 1] = NULL;

    if (*env)
        free(*env);
    *env = new_env;
	i = 0;
	while ((*env)[i])
	{
		printf("ENV:%s\n", (*env)[i]);
		i++;
	}
    free(key);

    return (1);
}

// static int add_export(char *str, char **env)
// {
//     int i = 0;
//     char *new_value;
//     char **new_env;
//     char *key;
//     char *equals;

//     // For empty export command or just a key without '='
//     equals = ft_strchr(str, '=');
//     if (!equals)
//         return (1);  // Valid but nothing to add to env

//     // Get key length
//     key = ft_substr(str, 0, equals - str);
//     if (!key)
//         return (0);

//     // Look for existing key
//     while ((*env)[i])
//     {
//         if (ft_strncmp((*env)[i], key, equals - str) == 0 && 
//             (*env)[i][equals - str] == '=')
//         {
//             // Update existing entry
//             new_value = ft_strdup(str);
//             if (!new_value)
//             {
//                 free(key);
//                 return (0);
//             }
//             free((*env)[i]);  // Free only if we successfully created new_value
//             (*env)[i] = new_value;
//             free(key);
//             return (1);
//         }
//         i++;
//     }

//     // Create new environment array
//     new_env = malloc(sizeof(char *) * (i + 2));  // Space for new entry and NULL
//     if (!new_env)
//     {
//         free(key);
//         return (0);
//     }

//     // Copy old environment
//     for (int j = 0; j < i; j++)
//         new_env[j] = (*env)[j];

//     // Add new entry
//     new_env[i] = ft_strdup(str);
//     if (!new_env[i])
//     {
//         free(key);
//         free(new_env);
//         return (0);
//     }
//     new_env[i + 1] = NULL;

//     // Update environment pointer
//     *env = new_env;
//     free(key);

//     return (1);
// }


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