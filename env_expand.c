#include "minishell.h"

char *get_env_value(char *name, char **env)
{
    int i;
    int len;
    char *value;
    
    if (!name || !env)
        return (NULL);
        
    len = ft_strlen(name);
    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
        {
            value = ft_strdup(env[i] + len + 1);  // Create a copy of the value
            return (value);
        }
        i++;
    }
    
    return (NULL);
}
char *expand_var(char *str, int *i, char **env)
{
	int len;
	char *var_name;
	char *value;

	var_name = NULL;
	value = NULL;
	len = 0;
	(*i)++;
	// if (str[*i] == '?')
	// {
	// 	// to do
	// }
	while (str[*i + len] && (str[*i + len] == '_' || ft_isalnum(str[*i + len])))
		len++;
	if (len == 0)
		return(ft_strdup("$"));
	var_name = ft_substr(str, *i, len);
    printf("len %d\n", len);
    printf("var name is %s\n", var_name);
	if (!var_name)
        return (NULL);
	*i += len;
	value = get_env_value(var_name, env);
	free(var_name);
	if (value)
		return (value);
	return (ft_strdup(""));
}

char *check_expand_var(char *value, int j, int len, int i)
{
    int value_len;
    int token_size;
    char *new_token;

    value_len = 0;
    value_len = ft_strlen(value);
    token_size = len + 1;
    if (j + value_len + (len - i) >= token_size)
    {
		token_size = j + value_len + (len - i) + 1;
        new_token = malloc(sizeof(char) * token_size);
        if (!new_token)
        {
            //free(token);
            //free(value);
            return (NULL);
        }
        // ft_strcpy(new_token, token);
        // free(token);  // Free the old token
        // token = new_token;  // Update token to point to the new memory
	}
    return (new_token);
}