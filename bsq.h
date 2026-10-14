

#ifndef BSQ_H
# define BSQ_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

typedef struct s_bsq
{
	int     nb_line;
    int     nb_col;
	int     max_x;
	int     max_y;
	int     size;
	char    c_empty;
	char    c_wall;
	char    c_draw;
	char    **map;
}   t_bsq;

int bsq_start(t_bsq *bsq, char *filename);
int parse_map(t_bsq *bsq, FILE *file);
int ft_atoi(char *str);
int parse_param(t_bsq *bsq, char *line);
int check_double(t_bsq *bsq);
int add_mapline(t_bsq *bsq, char *line, int l);
int add_mapline_map(t_bsq *bsq, char *line, int l);
int check_valid_chars(t_bsq *bsq);

void printmap(t_bsq *bsq);
void solve(t_bsq *bsq, int x, int y);
void free_map(char** map);

#endif
