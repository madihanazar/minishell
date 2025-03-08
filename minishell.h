
#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <string.h>
# include "readline/readline.h"
# include "readline/history.h"
# include <termios.h>
# include <stdbool.h>

// extern int g_sig;
extern int	g_status;

typedef struct s_cmd {
    char    *cmd_name;   
    char    **args;     
    int     arg_count;
} t_cmd;

typedef enum e_node_type {
    NODE_COMMAND,
	NODE_ARG,
    PIPE,       
    REDIR_IN,  
    REDIR_OUT,  
    HEREDOC,    
    APPEND      
} t_node_type;


typedef struct s_tree
{
	struct s_tree	*left;
	struct s_tree	*right;
	char			*cmd;
	char			**args;
	t_node_type		type;
}	t_tree;

// remove
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

typedef struct s_context
{
	char	*cmd;
	char	**args;
	int		input;
	int		output;
	int		error;
	struct s_context	*next;
}	t_context;

typedef struct s_shell
{
	t_list		*env_list;
	t_tree		*tree;
	t_context	*context;
	// int         status;
	t_list		*export_list;
} t_shell;

typedef struct s_exp
{
    int  i;       // Index for iterating through the input string
    int  j;       // Index for storing characters in the token
    int  sq;      // Single quote flag (1 if inside single quotes, 0 otherwise)
    int  dq;      // Double quote flag (1 if inside double quotes, 0 otherwise)
    int  len;     // Length of the input string or token
    char *token;  // Pointer to the extracted token
} t_exp;

// typedef struct s_expand
// {
//     char    *result;
//     int     i;          // index for input string
//     int     j;          // index for result string
//     int     len;        // length for variable names or allocations
//     int     s_quote;    // single quote flag
//     int     d_quote;    // double quote flag
// } t_expand;

char	*find_first_pipe(char *str);
char	*find_first_redir(t_tree*node, char *str);
t_tree	*free_handle_pipe_redir(int *flag, t_tree *node, char *str);
t_tree	*free_commands(int *flag, char **cmd_tokens, t_tree *node, char *str);
int		calculate_length(char *redir_pos);
int		find_num_arguments(t_tree *node);
t_tree	*create_redir_node(char *redir_pos);
void	free_strings(char *str1, char *str2);
t_tree	*build_ast(char *str, t_shell *shell, int *flag);
t_tree	*create_tree(char *str, t_shell *shell);
t_tree	*create_node(char *str,  t_node_type type);
char	**split_cmd(char *str, char split_char, t_shell *shell);
void	free_result(char **result, int count);
char	*expand_var(char *str, int *i, t_shell *shell);
char	*get_env_value(char *name, t_shell *shell);
char	*get_env_value_helper(char *name, t_shell *shell);
void	free_ast(t_tree *node);
void	free_split(char **result);
void	print_ast(t_tree *node, int depth);
void	free_shell(t_shell *shell);
void	free_env(char **env_copy);
void	rl_replace_line(const char *text, int pos);
t_shell	*create_shell(void);
char	**create_env_copy(char **env);
bool	split_args(t_tree *node);
bool	perform_exp(t_tree *node, t_shell *shell);
bool	split_evenly(char *str, char *curr, t_tree **left, t_tree **right);
bool	handle_pipes(t_tree *node);
char	**quote_split(char *str, char split_char);

//string utils
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strdup(char *s);
char	*ft_strtrim(char const *s1, char const *set);
int		ft_strlen(const char *str);
char	*ft_strchr(const char *str, int c);
int		ft_isalnum(int c);
int		ft_strncmp(const char *str1, const char *str2, size_t n);
char	*ft_strcpy(char *dest, const char *src);
void	ft_bzero(void *ptr, size_t n);
char	*ft_strjoin(char const *s1, char const *s2);
int		ft_strcmp(const char *s1, const char *s2);
char	**ft_split(const char *s, char c);
void	ft_putstr_fd(char *s, int fd);
int		ft_isalpha(int c);
char	*ft_itoa(int n);
int		find_num_of_digits(int n);
void	make_string(char *f_string, unsigned int num, int index, int n);

int		execute_pipe(t_tree *node, char ***env, t_shell *shell);
int		execute_redir(t_tree *node, char ***env, t_shell *shell);
int		is_builtin(char *cmd);
int		execute_builtin(char **args, t_shell *shell);
char	*join_path(char *path, char *args);
char	*extract_path(char *cmd, char **env);
int		execute_command(t_tree *node, char ***env);
int		execute_cmd(t_shell *shell);
int		execute_node(t_shell *shell);
char	**build_args(t_tree *node);
void	test_command(char **env, char *test_name, char *cmd);
int		execute_heredoc(t_tree *node, char ***env);
int		has_heredoc(t_tree *node);
t_tree	*handle_pipe(char *str, t_shell *shell, int *flag);
bool	handle_redirection(t_tree *node);
bool	split_redirects(char *str, char *beg, t_tree **left, t_tree **right);
t_tree	*handle_command(char *str, t_shell *shell, int *flag);
int		count_tokens(char *str, char split_char, int sq, int dq);
int		fill_tokens(char **result, char *str, char split_char);
int		get_token_len(char *str, char split_char);
char	*extract_token(char *str, int len, t_shell *shell);
int		init_extract(int len, t_exp *exp);
bool	handle_quotes(t_tree *node);
int		handle_expansion(char *str, t_exp *exp, t_shell *shell);

//built in
int		builtin_cd(t_shell *shell, char **args);
int		replace_directory(t_shell *shell, char *old_path, char *new_path);
char	*set_new_path(char **args);
int		builtin_pwd(t_tree *node, char **args, char **env);
int		builtin_echo(t_tree *node, char **argv, char **env);
int		builtin_env(t_tree *node, char **args, char **env);
int		builtin_export(t_tree *node, char **args, char ***env, t_list **export_list);
int		builtin_exit(t_tree *node, char **args, char **env, t_shell *shell);
int		add_export(char *str, char ***env, t_list **export_list);
int		add_export_1(char *str, char ***env, t_list **export_list);
int		check_export(char *str);
int		builtin_unset(t_tree *node, char **args, char ***env);
char	**list_to_env(t_list *list);
t_list	*env_to_list(char **env);
int		env_init(t_list **env_list);
void	free_env_list(t_list **node);
t_list	*create_env_node(void *content);
char	*find_shlvl_str(t_list	*shlvl_node);
int		assign_shlvl_val(int shlvl_val);
t_list	*find_node(t_list **env_list, char *str, int len);
int		ft_atoi(const char *str);

//list
t_list	*ft_lstnew(void *content);
int		ft_lstsize(t_list *lst);
void	ft_lstclear(t_list **lst, void (*del)(void *));
void	ft_lstadd_back(t_list **lst, t_list *new);
void	ft_lstdelone(t_list *lst, void (*del)(void *));

//signal
void	handle_sigint(int sig);
int		check_status(int status);

//exec
int		new_execute(t_shell *shell);
void	free_context_list(t_context *context);
void	free_context(t_context *context);
char	*expanded_str(char *str, char *var, t_shell *shell);
bool	process_heredocs(t_shell *shell, t_tree *node, char **env);
void	child_heredoc(int *fd, t_shell *shell, t_tree *node, char **env);
bool	process_pipes(t_shell *shell, t_context *context, t_tree *node, char **env);
bool	preprocess(t_shell *shell, t_context *context, t_tree *node, char **env);

#endif