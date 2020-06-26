#include "microshell.h"

/** ONLY DEBUG **/

void    ft_show_all(t_all *all)
{
    int i;
	int	j;

    printf("================================\n");
    printf("argc = %d\n", all->argc);
    printf("argv ......................................\n");
    i = 0;
    while (all->argv[i])
	{
		j = 0;
		while (all->argv[i][j])
		{
			if (j == 0)
        		printf("|%s|\n", all->argv[i][j++]);
			else
        		printf("- |%s|\n", all->argv[i][j++]);
		}
		printf("\n");
		i++;
	}
    i++;
    printf("venv ......................................\n");
    while (all->venv[i])
        printf("|%s|\n", all->venv[i++]);
    printf("================================\n");
    printf("================================\n");
    printf("================================\n\n\n");
}

/** EASY ONE - SMALLS FUNCTIONS **/

int		ft_strlen(char *str)
{
	int i;

	while (*str++)
		i++;
	return (i);
}

int		write_error(char *msg)
{
	write(STDERR_FILENO, msg, ft_strlen(msg));
	return (0);
}

char	*ft_strcpy(char	*str)
{
	int		size;
	char	*ptr;

	size = 0;
	ptr = str;
	while (*ptr++)
		size++;
	if (!(ptr = malloc(sizeof(char) * (size + 1))))
		return (NULL);
	ptr[size] = '\0';
	size = -1;
	while (str[++size])
		ptr[size] = str[size];	
	return (ptr);
}

int		ft_isspace(char c)
{
	if (c == '\t' || c == '\n' || c =='\v' || c == '\f' ||
c == '\r' || c == ' ')
		return (1);
	return (0);
}

int		size_word(char *str)
{
	int	i;

	i = 0;
	while (*str && !ft_isspace(*str++))
		i++;
	return (i);

}

int		count_words(char *str)
{
	int	i;

	i = 0;
	while (*str)
	{
		while (ft_isspace(*str))
			str++;
		if (*str && !ft_isspace(*str))
			i++;
		while (*str && !ft_isspace(*str))
			str++;
	}
	return (i);
}

void	dup_close_pipe(int	*pipe, int nb)
{
	dup2(pipe[nb], nb);
	close(pipe[0]);
	close(pipe[1]);
}

void	close_pipe(int	*pipe)
{
	close(pipe[0]);
	close(pipe[1]);
}

void	cd (char **params)
{
	if (chdir(params[1]))
	{
		write_error("error: cd: cannot change directory to ");
		write_error(params[0]);
		write_error("\n");
	}
}

/** MAINS **/

void    init_all(int argc, char **argv, char **env, t_all *all)
{
	int tmp;
	int tmp2;
    int i;
	int j;
	int k;
	int l;
	int m;
	int size;

    i = 1;
	size = 1;
    all->argc = argc;
	while (argv[i] && !strcmp(argv[i], ";"))
	{
		size++;
		i++;
	}
	while (argv[i])
	{
		if (strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			size++;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;		
		while (argv[i] && (!strcmp(argv[i], "|") || !strcmp(argv[i], ";")))
		{
			size++;
			i++;
		}
	}
	if (!(all->argv = malloc(sizeof(char **) * (size + 1))))
		return ;
	all->argv[size] = NULL;
	all->argc = size;
	if (!(all->argv[0] = malloc(sizeof(char *) * (1 + 1))))
		return ;
	all->argv[0][0] = ft_strcpy(argv[0]);
	all->argv[0][1] = NULL;
	i = 1;
	m = 1;
	while (argv[i])
	{		
		tmp2 = i;
		size = 0;
		if (strcmp(argv[i], "|") && strcmp(argv[i], ";"))
		{
			while (argv[i] && !strcmp(argv[i], ";"))
				i++;
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				size += count_words(argv[i++]);
		}
		else
			size = 1;
		if (!(all->argv[m] = malloc(sizeof(char *) * (size + 1))))
			return ;
		tmp = i;
		i = tmp2;
		j = 0;
		k = 0;
		while (argv[i][j] || (++i && !(j = 0) && i < tmp))
		{
			while (ft_isspace(argv[i][j]))
				j++;
			if (argv[i][j] && !ft_isspace(argv[i][j]))
			{
				if (!(all->argv[m][k] = malloc (sizeof(char) *
(size_word(&argv[i][j]) + 1))))
					return ;
				l = 0;
				while (argv[i][j] && !ft_isspace(argv[i][j]))
					all->argv[m][k][l++] = argv[i][j++]; 
				all->argv[m][k][l] = '\0';
				k++;
			}
		}
		all->argv[m][k] = NULL;
		m++;
	}
    all->venv = env;
}

int    ft_pipe(t_all *all)
{    
	int i;
	int *prev_pipe;
	int *next_pipe;


	prev_pipe = NULL;
    i = 1;
    while (i < all->argc)
    {
		if (all->argv[i + 1] &&
!strcmp(all->argv[i + 1][0], "|") && !all->argv[i + 1][1])
		{
			if (!(next_pipe = malloc(sizeof(int) * 2)))
				return (1);
			pipe(next_pipe);
		}
		else
			next_pipe = NULL;
		if (strcmp(all->argv[i][0], ";") && strcmp(all->argv[i][0], "|"))
		{
			if (!strcmp(all->argv[i][0], "cd") &&
(!all->argv[i][1] || all->argv[i][2]))
				return (write_error("error: cd: bad arguments\n"));
			else if (!strcmp(all->argv[i][0], "cd"))
			{
				printf("cd\n");
				cd(all->argv[i]);
			}
			else if (!fork())
			{
				if (prev_pipe)
					dup_close_pipe(prev_pipe, 0);
				if (next_pipe)
					dup_close_pipe(next_pipe, 1);
    			execve(all->argv[i][0], all->argv[i], all->venv);
				// TODO: Error
            	exit(0);
        	}
			if (prev_pipe)
				close_pipe(prev_pipe);
			wait(NULL);
			if (prev_pipe)
				free(prev_pipe);
			prev_pipe = next_pipe;
		}
        i++;
    }
	free(next_pipe);
	return (0);
}

int main(int argc, char **argv, char **env)
{
	t_all	all;

    init_all(argc, argv, env, &all);
	ft_show_all(&all);
    ft_pipe(&all);
    return (0);
}
