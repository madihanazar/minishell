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
	if (!var_name)
        return (NULL);
	*i += len;
	value = get_env_value(var_name, env);
	free(var_name);
	if (value)
		return (value);
	return (ft_strdup(""));
}
