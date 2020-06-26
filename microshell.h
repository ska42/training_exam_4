#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>

typedef struct	s_all
{
	int			argc;
	char		***argv;
	char		**venv;
}				t_all;

#endif
