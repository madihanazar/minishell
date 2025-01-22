#include "minishell.h"

static int get_token_len(char *str, char split_char)
{
    int len;
    int double_quote;
    int single_quote;

    len = 0;
    double_quote = 0;
    single_quote = 0;
    while (str[len])
    {
        if (str[len] == '"' && !single_quote)
            double_quote = !double_quote;
        else if (str[len] == '\'' && !double_quote)
            single_quote = !single_quote;
        else if (str[len] == split_char && !double_quote && !single_quote)
            break;
        len++;
    }
    return (len);
}

static char *extract_token(char *str, int len, char **env)
{
    char *token;
    int i;
    int j;
    int single_quote;
    int double_quote;
    char *value;
    int token_size;
    int value_len;

    token_size = len + 1;  // Initial size is the length of the input string
    token = malloc(sizeof(char) * token_size);  
    if (!token)
    {
        return (NULL);
    }
	ft_bzero(token, token_size);
    i = 0;
    j = 0;
    single_quote = 0;
    double_quote = 0;
    value = NULL;

    while (i < len)
    {
        if (str[i] == '"' && !single_quote)
        {
			double_quote = !double_quote;
			i++;   
			continue;    // to move on and not the quotes in the argument 
			// echo "hi" will now give hi and not "hi"
		}    
        else if (str[i] == '\'' && !double_quote)
        {
			single_quote = !single_quote;
			i++;
			continue;
		}
        else if (str[i] == '$' && !single_quote)
        {
            value = expand_var(str, &i, env);
            if (value)
            {
                value_len = ft_strlen(value);
                if (j + value_len + (len - i) >= token_size)
                {
					token_size = j + value_len + (len - i) + 1;
                    char *new_token = malloc(sizeof(char) * token_size);
                    if (!new_token)
                    {
                        free(token);
                        free(value);
                        return (NULL);
                    }
                    ft_strcpy(new_token, token);
                    free(token);  // Free the old token
                    token = new_token;  // Update token to point to the new memory
				}
				ft_strcpy(&token[j], value);
                j += value_len;
                free(value);  // Free the expanded value
                continue;
            }
        }
        token[j++] = str[i++];
    }
    token[j] = '\0';  // Null-terminate the token
    return (token);
}


char **split_cmd(char *str, char split_char, char **env)
{
	int i;
	int double_quote;
	int single_quote;
	int count;
	char **result;
	int token_len;
	int j;

	i = 0;
	double_quote = 0;
	single_quote = 0;
	count = 0;
	token_len = 0;
	while (str[i])
	{
    	if (str[i] == '"' && !single_quote)
			double_quote = !double_quote;
		else if (str[i] == '\'' && !double_quote)
        	single_quote = !single_quote;
    	if (str[i] == split_char && !double_quote && !single_quote)
        	count++;
    	i++;
	}
	 result = malloc(sizeof(char *) * (count + 2));  // null terminator
	 if (!result)
	 	return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		while (str[i] == split_char)
			i++;
		if (!str[i])
			break;
		token_len = get_token_len(str + i, split_char);
		//printf("token len %d\n", token_len);
		result[j] = extract_token(str + i, token_len, env);
        if (!result[j])
        {
            free_split(result);
            return (NULL);
        }
		//printf("token before i %d\n", i);
        i += token_len;
		//printf("i value %d\n", i);
        j++;
    }
    result[j] = NULL;  
	//j = 0;
	// while (result[j] != NULL)
	// 	printf("result is:::%s\n",result[j++]);
	return (result);
}


t_tree *build_ast(char *str, char **env)
{
    char *pipe_pos = NULL;
    t_tree *node = NULL;
    char *redir_pos = NULL;
    int len;
    char *right_str = NULL;

    if (str == NULL || *str == '\0')
        return (NULL);

    pipe_pos = find_last_pipe(str);
    if (pipe_pos)
    {
        node = create_node("|", PIPE);
        if (!node)
            return (NULL);
		node->left = build_ast(ft_substr(str, 0, pipe_pos - str), env);
        node->right = build_ast(ft_strdup(pipe_pos + 1), env);
        return (node);
    }
	redir_pos = find_last_redir(str);
    if (redir_pos)
    {
        len = 1;
        if (*(redir_pos + 1) == *redir_pos)
            len = 2;
		if (*redir_pos == '>' && *(redir_pos + 1) == '>')
            node = create_node(">>", APPEND);
        else if (*redir_pos == '<' && *(redir_pos + 1) == '<')
            node = create_node("<<", HEREDOC);
        else if (*redir_pos == '>')
            node = create_node(">", REDIR_OUT);
        else if (*redir_pos == '<')
            node = create_node("<", REDIR_IN);
        if (!node)
            return (NULL);
        node->left = build_ast(ft_substr(str, 0, redir_pos - str), env);
        right_str = redir_pos + len;
        node->right = build_ast(ft_strdup(right_str), env);
        return (node);
    }

    char **cmd_tokens = split_cmd(str, ' ', env);
    if (cmd_tokens && cmd_tokens[0])
    {
        node = create_node(cmd_tokens[0], NODE_COMMAND);
        if (!node)
        {
            free_split(cmd_tokens);
            return (NULL);
        }

        // Handle arguments as right children
        t_tree *current = node;
        int i = 1;
        while (cmd_tokens[i])
        {
			current->right = create_node(cmd_tokens[i], NODE_ARG);
            if (current->right == NULL)
                break;
            current = current->right;
            i++;
        }
        free_split(cmd_tokens);
    }
    
    return (node);
}

t_tree *create_tree(char *str, char **env)
{
    char *trimmed = NULL;
	t_tree *root = NULL;

	if (!str)
        return (NULL);
    trimmed = ft_strtrim(str, " ");
	if (trimmed == NULL)
		return (NULL);
    root = build_ast(trimmed, env);
    free(trimmed);
    return (root);
}

void print_ast(t_tree *node, int depth)
{
    if (node != NULL)
    {
        // Print the left subtree first
        if (node->left)
        {
            print_ast(node->left, depth + 1);
        }

        // Print the current node with indentation based on depth
        for (int i = 0; i < depth; i++)
            printf("    "); // Indentation for visualization
        printf("Depth: %d, Command: %s\n", depth, node->cmd);

        // Print the right subtree
        if (node->right)
        {
            print_ast(node->right, depth + 1);
        }
    }
}


// static void print_ast(t_tree *node, int depth)
// {
//     if (node != NULL)
//     {
//         // Print the left subtree first (if it exists)
//         if (node->left)
//         {
//             print_ast(node->left, depth + 1);  // Increase depth for left child
//         }

//         // Print the current node's command and depth
//         printf("Depth: %d, Command: %s\n", depth, node->cmd);

//         // Print the right subtree (if it exists)
//         if (node->right)
//         {
//             print_ast(node->right, depth + 1);  // Increase depth for right child
//         }
//     }
// }

static void test_ast(char *command, char **env) {
    printf("\nTesting command: %s\n", command);
    t_tree *root = create_tree(command, env);
    printf("AST structure:\n");
    print_ast(root, 0);
    printf("------------------------\n");
    // Add function to free the tree
    free_ast(root);
}

// int main()
// {
//     char *command = "cat < input.txt | echo \"hello world\" | cat < hi | grep hi | wc -l << output.txt";
//     //char *command = "cat < input.txt|grep hi";
//     // Build the AST
//     t_tree *root = create_tree(command);
    
//     // Print the resulting AST in an in-order traversal
//     printf("In-order traversal of the AST:\n");
//     print_ast(root, 0);
//     printf("\n");

//     // Free the memory allocated for the tree
//     // (this is important to avoid memory leaks)
//     // You'll need a function to recursively free the tree nodes
//     return 0;
// }

// int main(int ac, char **argv, char **env)
// {
// 	(void)ac;
// 	(void)argv;
// 	//  test_ast("ls");
//     // test_ast("ls -la");
//     // test_ast("ls | grep a");
// 	// test_ast("pwd");
// 	// test_ast("echo hello");
// 	// test_ast("echo -n hello");
// 	// test_ast("ls -la /home");
// 	// test_ast("cat file1 file2 file3");
// 	test_ast("cat file.txt | grep error | sort | uniq", env);
// 	// test_ast("wc -l < input.txt > output.txt");
// 	// test_ast("echo \"hello world\"");
// 	// test_ast("echo 'single quotes'");
// 	// test_ast("echo \"quote with | pipe\"");
// 	// test_ast("echo \"redirect > inside quote\"");
// 	// //test_ast("echo hello '|' echo he");
// 	// test_ast("cat < in.txt >> app.txt > out.txt");
// 	//test_ast("echo $PATH", env);
// 	//printf("env var:%s\n",env[0]);
// 	//test_ast("echo $HOME $USER", env);
// 	// test_ast("echo \"hello world\"", env);
// 	test_ast("echo \"$HOME\"", env);
// 	test_ast("cd /home", env);
// 	test_ast("export VAR=value", env);
// 	// test_ast("cat <");
// 	// test_ast("echo hello |");
// 	// test_ast("| ls");
// 	// test_ast("echo    hello     world");
// 	// test_ast("ls   -l    |    grep    a");
// 	// test_ast("cat    <    input.txt    >    output.txt");
// 	// test_ast("echo hello \'|\' world");
// 	// test_ast("echo hello '<' test.txt");
// 	// test_ast("echo hello world | cat 'hi'");
// 	return (0);
// }