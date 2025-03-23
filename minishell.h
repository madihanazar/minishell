/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkunnath <nkunnath@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 21:59:15 by mnazar            #+#    #+#             */
/*   Updated: 2025/03/23 22:43:10 by nkunnath         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include <errno.h>
# include <dirent.h>

extern int	g_status;

typedef struct s_cmd
{
	char	*cmd_name;
	char	**args;
	int		arg_count;
}	t_cmd;

typedef enum e_node_type
{
	NODE_COMMAND,
	NODE_ARG,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	HEREDOC,
	APPEND
}	t_node_type;

typedef struct s_tree
{
	struct s_tree	*left;
	struct s_tree	*right;
	char			*cmd;
	char			**args;
	t_node_type		type;
	int				heredoc_fd;
}	t_tree;

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

typedef struct s_context
{
	struct s_context	*next;
	char				*cmd;
	char				**args;
	int					input;
	int					output;
	int					error;
}	t_context;

typedef struct s_shell
{
	t_list		*env_list;
	t_tree		*tree;
	t_context	*context;
	t_list		*export_list;
}	t_shell;

void	rl_replace_line(const char *text, int pos);

//build_tree.c
t_tree	*create_tree(char *str, t_shell *shell);
bool	is_valid_string(char *input);
char	*is_valid_redir(char *str);
char	*is_valid_pipe(char *input);
void	convert_to_whitespace(char	*input);

//string utils.c
int		ft_strlen(const char *str);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strtrim(char const *s1, char const *set);
char	*ft_strchr(const char *str, int c);

//string_utils_2.c
char	**ft_split(const char *s, char c);

//string utils_3.c
int		ft_strncmp(const char *str1, const char *str2, size_t n);
char	*ft_strcpy(char *dest, const char *src);
char	*ft_strjoin(char const *s1, char const *s2);
void	ft_putstr_fd(char *s, int fd);
void	ft_putendl_fd(char *s, int fd);

//string utils_4.c
char	*ft_strdup(char *s);
int		ft_isalnum(int c);
int		ft_isalpha(int c);
int		ft_isdigit(int c);

//parse.c
t_tree	*build_ast(char *str, t_shell *shell);
char	*ft_strappend(char *str1, char *str2);
char	*get_word(char *str);
char	*helper_exp(char *str, t_shell *shell);
bool	perform_exp(t_tree *node, t_shell *shell);

//parse_2.c
bool	split_args(t_tree *node);
char	**quote_split(char *str, char split_char);
int		fill_tokens(char **result, char *str, char split_char);
char	*remove_quotes(char *str, int *i, char *end);
bool	handle_quotes(t_tree *node);

//parse_3.c
int		get_token_len(char *str, char split_char);
int		count_tokens(char *str, char split_char);
char	*extract_word(char *str, char c);
char	*get_var_name(char *str);
char	*expanded_str(char *str, char *var, t_shell *shell);

//parse_4.c
bool	split_redirects(char *str, char *beg, t_tree **left, t_tree **right);
bool	handle_redirection(t_tree *node);
void	free_env(char **env_copy);
char	*get_env_value(char *name, t_shell *shell);

//parse_5.c
bool	handle_pipes(t_tree *node);
bool	split_evenly(char *str, char *curr, t_tree **left, t_tree **right);
int		get_len_separator(char *str);
int		check_symbol(int symbol, char c, int neg, int flag);

//pipe_redir.c
char	*find_first_pipe(char *str);
void	check_redir_type(t_tree *node, char *str);
char	*find_first_redir(t_tree *node, char *str);
void	free_strings(char *str1, char *str2);

//tree_nodes.c
t_tree	*create_node(char *str, t_node_type type);
void	free_ast(t_tree *node);
void	free_split(char **result);
void	free_result(char **result, int count);

//ft_itoa.c
char	*ft_itoa(int n);
int		ft_atoi(const char *str);

//shell_env_init.c
t_shell	*create_shell(void);
void	free_shell(t_shell *shell);
int		assign_shlvl_node(t_list **env_list);
int		env_init(t_list **env_list);
t_list	*find_node(t_list **env_list, char *str, int len);

//shell_env_init_2.c
t_list	*create_env_node(void *content);
void	free_env_list(t_list **node);
char	**create_env_copy(char **env);
int		assign_shlvl_val(int shlvl_val);
char	*find_shlvl_str(t_list	*shlvl_node);

//shell_env_init_3.c
char	**list_to_env(t_list *list);
t_list	*env_to_list(char **env);
t_list	*handle_empty_env(void);
t_list	*assign_pwds(char *str);

//list_utils.c
t_list	*ft_lstnew(void *content);
int		ft_lstsize(t_list *lst);
void	ft_lstclear(t_list **lst, void (*del)(void *));
void	ft_lstadd_back(t_list **lst, t_list *new);
void	ft_lstdelone(t_list *lst, void (*del)(void *));

//context_utils.c
void	free_context_list(t_context *context);
void	free_context(t_context *context);
t_context	*create_context(void);

//preprocess.c
bool	preprocess(t_shell *shell, t_context *con, t_tree *node, char **env);
bool	proc_hdocs(t_shell *shell, t_context *con, t_tree *node, char **env);
bool	proc_pipes(t_shell *shell, t_context *con, t_tree *node, char **env);

//status_handling.c
void	print_signal_errors(int status);
int		check_status(int status);

//built_in_helpers.c
int		is_builtin(char *cmd);
char	*set_new_path(char **args, t_list *env_list, int fd);
int		replace_directory(t_shell *shell, char *old_path, char *new_path);
t_list	*find_node_from_env(char *to_find, t_list *env_list, int *flag);

//process_cmd.c
bool	process_command(t_context *context, t_tree *node, char **env);
bool	check_heredoc(t_tree *node);

//new_execute.c
bool	traverse_tree(t_context *context, t_tree *node, char **env);
int		new_execute(t_shell *shell);
bool	process_input(t_context *context, t_tree *node, char **env);
bool	process_output(t_context *context, t_tree *node, char **env, int flag);
void	heredoc_sig(int sig);

//new_built_in.c
int		new_execute_builtin(t_shell *shell, char **env);
int		builtin_exit(t_shell *shell, char **env);
int		builtin_pwd(int fd);
int		builtin_env(char **env, int fd);

//new_built_in_2.c
int		builtin_cd(t_shell *shell, t_context *context);
int		builtin_echo(char **argv, int fd);

//new_unset.c
int		builtin_unset(t_shell *shell);
void	display_export_error(char *str, int *status);

//new_export.c
bool	add_to_env_list(t_shell *shell, char *str);
int		builtin_export(t_shell *shell, char **env);

//new_exec_context.c
int		get_execution_error(char *cmd);
int		execute_command(t_context *context, char **env);
bool	execute_context(t_shell *shell, char **env, pid_t *pid);

#endif