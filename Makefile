NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror 
RM = rm -rf

READLINE_FLAGS = -L/opt/vagrant/embedded/lib -lreadline

SRC_FILES = main.c \
			build_tree.c \
			string_utils.c \
			string_utils_2.c \
			string_utils_3.c \
			string_utils_4.c \
			parse.c \
			parse_2.c \
			parse_3.c \
			parse_4.c \
			parse_5.c \
			pipe_redir.c \
			tree_nodes.c \
			ft_itoa.c \
			shell_env_init.c \
			shell_env_init_2.c \
			shell_env_init_3.c \
			list_utils.c \
			context_utils.c \
			preprocess.c \
			status_handling.c \
			built_in_helpers.c \
			process_cmd.c \
			new_execute.c \
			new_built_in.c \
			new_built_in_2.c \
			new_unset.c \
			new_export.c \
			new_exec_context.c \
			extra_free.c

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