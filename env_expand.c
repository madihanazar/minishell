#include "minishell.h"

void	make_string(char *f_string, unsigned int num, int index, int n)
{
	char	digit;

	if (n < 0)
	{
		f_string[0] = '-';
		while (index > 0)
		{
			digit = (num % 10) + '0';
			f_string[index] = digit;
			index = index - 1;
			num = num / 10;
		}
	}
	else
	{
		while (index >= 0)
		{
			digit = (num % 10) + '0';
			f_string[index] = digit;
			index = index - 1;
			num = num / 10;
		}
	}
}

int	ft_atoi(const char *str)
{
	int	i;
	int	sign;
	int	answer;

	i = 0;
	sign = 1;
	answer = 0;
	while (str[i] != '\0')
	{
		while (str[i] == ' ' || str[i] == '\t' || (str[i] >= 9 && str[i] <= 13))
			i = i + 1;
		if (str[i] == '+' || str[i] == '-')
		{
			if (str[i] == '-')
				sign = -1;
			i = i + 1;
		}
		while (str[i] >= 48 && str[i] <= 57)
		{
			answer = (answer * 10) + (str[i] - 48);
			i = i + 1;
		}
		return (sign * answer);
	}
	return (sign * answer);
}

int	find_num_of_digits(int n)
{
	int				number_of_digits;
	unsigned int	number;

	number_of_digits = 0;
	number = 0;
	if (n < 0)
	{
		number = n * -1;
		number_of_digits = 1;
	}
	else
		number = n;
	if (number == 0)
		number_of_digits += 1;
	else
	{
		while (number >= 1)
		{
			number = number / 10;
			number_of_digits = number_of_digits + 1;
		}
	}
	return (number_of_digits);
}

char	*ft_itoa(int n)
{
	int				number_of_digits;
	unsigned int	number_two;
	char			*final_string;

	number_of_digits = 0;
	number_two = 0;
	if (n < 0)
		number_two = n * -1;
	else
		number_two = n;
	number_of_digits = find_num_of_digits(n);
	final_string = (char *)malloc((sizeof(char) * number_of_digits) + 1);
	if (final_string == NULL)
		return (NULL);
	make_string(final_string, number_two, number_of_digits - 1, n);
	final_string[number_of_digits] = '\0';
	return (final_string);
}

char	*get_env_value_helper(char *name, t_shell *shell)
{
	int		i;
	int		len;
	char	*value;
	char	**env_array;

	i = 0;
	env_array = list_to_env(shell->env_list);
	if (!name || !env_array)
		return (NULL); 
	len = ft_strlen(name);
	while (env_array[i])
	{
		if (ft_strncmp(env_array[i], name, len) == 0 && env_array[i][len] == '=')
		{
			value = ft_strdup(env_array[i] + len + 1); // Automatically, NULL is taken care of
			free_env(env_array);
			return (value);
		}
		i++;
	}
	free_env(env_array);
	return (NULL);
}

char	*get_env_value(char *str, int **i, int len, t_shell *shell)
{
	char	*value;
	char	*var_name;

	var_name = NULL;
	value = NULL;
	if (len == 0)
	{
		value = ft_strdup("$"); // Automatically, NULL is taken care of
		return (value);
	}
	var_name = ft_substr(str, **i, len);
	if (!var_name)
        return (NULL);
	**i += len;
	value = get_env_value_helper(var_name, shell);
	free(var_name);
	if (value)
		return (value);
	value = ft_strdup(""); // Automatically, NULL is taken care of
	return (value);
}

char	*expand_var(char *str, int *i, t_shell *shell)
{
	int		len;
	char	*value;

	len = 0;
	value = NULL;
	(*i)++;
	if (str[*i] == '?')
	{
		value = ft_itoa(g_status);
		return (value);
	}
	while (str[*i + len] && (str[*i + len] == '_' || ft_isalnum(str[*i + len])))
		len++;
	value = get_env_value(str, &i, len, shell);
	return (value);
}
