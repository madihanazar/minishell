NAME = minishell
CC = cc
# CFLAGS = -Wall -Wextra -Werror 
# CFLAGS = -g3
RM = rm -rf


READLINE_FLAGS = -L/opt/vagrant/embedded/lib -lreadline


# SRC_FILES = main.c \
#            build_tree.c \
#            built_in.c \
#            env_expand.c \
#            exec.c \
#            string_utils.c \
#            tree_nodes.c  \
# 		   pipe_redir.c  \
# 		   ft_export.c  \
# 		   list_utils.c \
# 		   ft_unset.c \
# 		   heredoc.c \
# 		   status_handling.c \
# 		   shell_env_init.c

SRC_FILES = main.c \
           build_tree.c \
           env_expand.c \
           string_utils.c \
           tree_nodes.c  \
           list_utils.c \
           status_handling.c \
           pipe_redir.c  \
           parse.c \
           shell_env_init.c \
		   new_execute.c \
		   new_built_in.c \
		   built_in_helpers.c
           # built_in.c \
           # exec.c \
           # ft_export.c  \
           # ft_unset.c \
           # heredoc.c



OBJS = $(SRC_FILES:.c=.o)


HEADER = minishell.h

all: $(NAME)

$(NAME): $(OBJS) $(HEADER)
	$(CC) $(OBJS) $(READLINE_FLAGS) -o $(NAME)


%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re