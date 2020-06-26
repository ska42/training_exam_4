#include "microshell.h"

/** ONLY DEBUG **/

/*
** NOTES:
** - Ne pas hésiter a faire pour fast debug et utiliser -g3 -fsanitize a chaque
** problemes
**
*/

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

	i = 0;
	while (*str++)
		i++;
	return (i);
}

int		write_error(char *msg)
{
	write(STDERR_FILENO, msg, ft_strlen(msg));
	return (1);
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
		write_error(params[1]);
		write_error("\n");
	}
}

/** MAINS **/

/*
** NOTES:
** - i et size a 1 pour pas proc le nom du prog
** - Ne pas oublier de mettre NULL en end a chaque fin (et au bon endroit)
** - Possibilité de copier coller la boucle et les if entre les parties 
** - Ne pas oublier all->argc = size
** - Ne pas oublier all->venv = env
**
*/

int		init_all(int argc, char **argv, char **env, t_all *all)
{
	int		i;
	int		j;
	int		k;
	int		tmp;
	int		size;

	i = 1;
	size = 1;
	while (argv[i])
	{
		if ((!strcmp(argv[i], "|") || !strcmp(argv[i], ";")) && ++size)
			i++;
		else if (++size)
		{
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				i++;	
		}
	}
	if (!(all->argv = malloc(sizeof(char **) * (size + 1))))
		return (write_error("error: fatal\n"));
	all->argv[size] = NULL;
	all->argc = size;
///////////////////////////////////////////
	if (!(all->argv[0] = malloc(sizeof(char *) * 2)))
		return (write_error("error: fatal\n"));
	all->argv[0][0] = argv[0];
	all->argv[0][1] = NULL;
	i = 1;
	j = 1;
	while (argv[i])
	{
		size = 0;
		if ((!strcmp(argv[i], "|") || !strcmp(argv[i], ";")))
		{
			if (!(all->argv[j] = malloc(sizeof(char *) * 2)))
				return (write_error("error: fatal\n"));
			all->argv[j][0] = argv[i];
			all->argv[j++][1] = NULL;
		}
		else
		{
			size++;
			tmp = i;
			while (argv[i] && argv[i + 1] && strcmp(argv[i + 1], "|") &&
strcmp(argv[i + 1], ";") && ++i)
				size++;
			if (!(all->argv[j] = malloc(sizeof(char *) * (size + 1))))
				return (write_error("error: fatal\n"));
			k = 0;
			while (tmp <= i)
				all->argv[j][k++] = argv[tmp++];
			all->argv[j++][k] = NULL;
		}
		i++;
	}
	all->venv = env;
	return (0);
}

/*
** NOTES:
** - mettre i a 1 pour pas proc sur le nom du prog
** - Pour les pipes : toujours mettre a NULL si pas de prev/next, pour pouvoir check
** - Dans le fork
** 		Si prev dup2 en 0 (in) (on remplace l'in si le precedent est un pipe)
**		Si next dup2 en 1 (out) (on remplace l'out si le prochain est un pipe)
** - Apres
** 		close tout une fois fini donc prev
** - Ne pas oublier de free les pipes
** - cd ne doit pas se lancer en pipe
**
*/

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
				return (write_error("error: fatal\n"));
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
				cd(all->argv[i]);
			else
			{
				if (!fork())
				{
						if (prev_pipe)
								dup_close_pipe(prev_pipe, 0);
						if (next_pipe)
								dup_close_pipe(next_pipe, 1);
						if (execve(all->argv[i][0], all->argv[i], all->venv))
						{
								write_error("error: cannot execute ");
								write_error(all->argv[i][0]);
								return (write_error("\n"));
						}
						exit(0);
				}
				if (prev_pipe)
					close_pipe(prev_pipe);
				if (wait(NULL) < 0)
					return (write_error("error: fatal\n"));
				if (prev_pipe)
				free(prev_pipe);
				prev_pipe = next_pipe;
			}
		}
        i++;
    }
	if (next_pipe)
	{
		close_pipe(next_pipe);
		free(next_pipe);
	}
	return (0);
}

int main(int argc, char **argv, char **env)
{
	t_all	all;

    if (init_all(argc, argv, env, &all))
		return (0);
	//ft_show_all(&all);
    ft_pipe(&all);
    return (0);
}
