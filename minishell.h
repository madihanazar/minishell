
#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <string.h>
# include "readline/readline.h"
#include <termios.h>

extern int g_sig;

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
	t_node_type		type;
    int             heredoc_fd;
}	t_tree;

// remove
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

typedef struct s_shell
{
    t_list      *env;         
    t_tree      *tree;
    //t_context   *context;
    int         status;
    t_list     *export_list;
} t_shell;

// typedef struct s_expand
// {
//     char    *result;
//     int     i;          // index for input string
//     int     j;          // index for result string
//     int     len;        // length for variable names or allocations
//     int     s_quote;    // single quote flag
//     int     d_quote;    // double quote flag
// } t_expand;

char	*find_last_pipe(char *str);
char	*find_last_redir(char *str);
t_tree *build_ast(char *str, char **env);
t_tree *create_tree(char *str, char **env);
t_tree	*create_node(char *str,  t_node_type type);
char **split_cmd(char *str, char split_char, char **env);
void free_result(char **result, int count);
char *expand_var(char *str, int *i, char **env);
char *get_env_value(char *name, char **env);
void free_ast(t_tree *node);
void free_split(char **result);
void print_ast(t_tree *node, int depth);
//string utils
char	*ft_substr(char const *s, unsigned int start, size_t len);
char *ft_strdup(char *s);
char *ft_strtrim(char const *s1, char const *set);
int ft_strlen(const char *str);
char *ft_strchr(const char *str, int c);
int	ft_isalnum(int c);
int	ft_strncmp(const char *str1, const char *str2, size_t n);
char *ft_strcpy(char *dest, const char *src);
void	ft_bzero(void *ptr, size_t n);
char	*ft_strjoin(char const *s1, char const *s2);
int ft_strcmp(const char *s1, const char *s2);
char	**ft_split(const char *s, char c);
void	ft_putstr_fd(char *s, int fd);
int	ft_isalpha(int c);


int execute_pipe(t_tree *node, char ***env, t_shell *shell);
int execute_redir(t_tree *node, char ***env, t_shell *shell);
int is_builtin(char *cmd);
int execute_builtin(t_tree *node, char **args, char ***env, t_shell *shell);
char *join_path(char *path, char *args);
char	*extract_path(char *envp[], char *args);
int execute_command(t_tree *node, char ***env);
int execute_node(t_tree *node, char ***env, t_shell *shell);
char **build_args(t_tree *node);
void test_command(char **env, char *test_name, char *cmd);
int execute_heredoc(t_tree *node, char ***env);
int process_heredocs(t_tree *node, char ***env);
int has_heredoc(t_tree *node);

//built in
int builtin_cd(t_tree *node, char **args, char **env);
int builtin_pwd(t_tree *node, char **args, char **env);
int builtin_echo(t_tree *node, char **argv, char **env);
int builtin_env(t_tree *node, char **args, char **env);
int builtin_export(t_tree *node, char **args, char ***env, t_list **export_list);
int add_export(char *str, char ***env, t_list **export_list);
int add_export_1(char *str, char ***env, t_list **export_list);
int check_export(char *str);
int builtin_unset(t_tree *node, char **args, char ***env);
char **list_to_env(t_list *list);
t_list *env_to_list(char **env);

//list
t_list	*ft_lstnew(void *content);
int	ft_lstsize(t_list *lst);
void	ft_lstclear(t_list **lst, void (*del)(void *));
void	ft_lstadd_back(t_list **lst, t_list *new);
void	ft_lstdelone(t_list *lst, void (*del)(void *));
//signal
void handle_sigint(int sig);

#endif