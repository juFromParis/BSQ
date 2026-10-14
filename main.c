#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "bsq.h"

int main(int ac, char **av)
{
    t_bsq bsq;

    if (ac != 2)
        return (1);
    bsq.map = NULL;
    bsq.size = 0;
    bsq.max_x = 0;
    bsq.max_y = 0;

    if (bsq_start(&bsq, av[1]))
    {
        free_map(bsq.map);
        return (1);
    }

    int y = 0;
    while (bsq.map[y] != NULL)
    {
        int x = 0;
        while (x < bsq.nb_col)
        {
            solve(&bsq, x, y);
            x++;
        }
        y++;
    }

    printmap(&bsq);
    free_map(bsq.map);
    return (0);
}

int bsq_start(t_bsq *bsq, char *filename)
{
    FILE    *file;

    file = fopen(filename, "r");
    if (!file)
        return (1);
    if(parse_map(bsq, file))
        return (1);
    fclose(file);
    if(check_valid_chars(bsq))
        return (1);
    return (0);
}

int check_valid_chars(t_bsq *bsq)
{
    int y = 0;
    while (bsq->map[y] != NULL)
    {
        int x = 0;
        while (x < bsq->nb_col)
        {
            if(bsq->map[y][x] != bsq->c_empty && bsq->map[y][x] != bsq->c_wall)
            {
                fputs("map error\n", stderr);
                return (1);
            }
            x++;
        }
        y++;
    }
    return (0);
}

int parse_map(t_bsq *bsq, FILE *file)
{
    char    *line;
    size_t  len;
    int     has_line;
    int     n = 0;
    int     l = 0;

    bsq->nb_col = 0;
    line = NULL;
    len = 0;
    has_line = 0;

    while (getline(&line, &len, file) != -1)
    {
        n++;
        if (n == 1 && parse_param(bsq, line) != 0)
        {
            free(line);
            return (fputs("map error\n", stderr), 1);
        }
        if (n > 1)
        {
            if (l >= bsq->nb_line || add_mapline(bsq, line, l) != 0)
            {
                free(line);
                return (fputs("map error\n", stderr), 1);
            }
            l++;
            has_line = 1;
        }
    }

    if (n == 0 || bsq->nb_line != n - 1)
    {
        free(line);
        return (fputs("map error\n", stderr), 1);
    }
    free(line);
    if (!has_line)
        return (1);
    return (0);
}

int ft_atoi(char *str)
{
    int i = 0;
    int n = 0;

    while (str[i] >= '0' && str[i] <= '9')
    {
        n = n * 10 + (str[i] - '0');
        i++;
    }
    return (n);
}

int parse_param(t_bsq *bsq, char *line)
{
    int digits;
    int number;

    bsq->nb_line = ft_atoi(line);
    if (bsq->nb_line <= 0)
        return (1);

    digits = 0;
    number = bsq->nb_line;
    while (number > 0)
    {
        digits++;
        number /= 10;
    }

    bsq->c_empty = line[digits + 1];
    bsq->c_wall = line[digits + 3];
    bsq->c_draw = line[digits + 5];

    if (bsq->c_empty == '\0' || bsq->c_wall == '\0'
        || bsq->c_draw == '\0')
        return (1);
    if (check_double(bsq))
        return (1);
    return (0);
}

int check_double(t_bsq *bsq)
{
    if (bsq->c_empty == bsq->c_wall
        || bsq->c_empty == bsq->c_draw
        || bsq->c_wall == bsq->c_draw)
        return (1);
    return (0);
}

int add_mapline(t_bsq *bsq, char *line, int l)
{
    int i;

    i = 0;
    while (line[i] != '\0' && line[i] != '\n')
        i++;

    if (i == 0)
        return (1);

    if (bsq->nb_col != 0 && bsq->nb_col != i)
        return (1);

    bsq->nb_col = i;
    if (add_mapline_map(bsq, line, l) != 0)
        return (1);
    return (0);
}

int add_mapline_map(t_bsq *bsq, char *line, int l)
{
    if (bsq->map == NULL)
        bsq->map = calloc(bsq->nb_line + 1, sizeof(char *));
    if (bsq->map == NULL)
        return (1);

    bsq->map[l] = strdup(line);
    if (bsq->map[l] == NULL)
        return (1);
    bsq->map[l + 1] = NULL;
    return (0);
}

void printmap(t_bsq *bsq)
{
    int x;
    int y;

    if (bsq->map == NULL)
        return ;

    y = 0;
    while (bsq->map[y] != NULL)
    {
        x = 0;
        while (x < bsq->nb_col)
        {
            if (x >= bsq->max_x
                && x < bsq->max_x + bsq->size
                && y >= bsq->max_y
                && y < bsq->max_y + bsq->size)
                fprintf(stdout, "%c", bsq->c_draw);
            else
                fprintf(stdout, "%c", bsq->map[y][x]);
            x++;
        }
        fprintf(stdout, "\n");
        y++;
    }
}

void free_map(char **map)
{
    int i;

    if (map == NULL)
        return ;

    i = 0;
    while (map[i] != NULL)
    {
        free(map[i]);
        i++;
    }
    free(map);
}

void solve(t_bsq *bsq, int x, int y)
{
    int max = 0;
    int try = 1;

    while (y + try <= bsq->nb_line && x + try <= bsq->nb_col)
    {
        max = 0;
        while (max < try)
        {
            int start = x;

            while (start - x < try)
            {
                if (bsq->map[y + max][start] != bsq->c_empty)
                    return ;
                start++;
            }
            max++;
        }

        if (try > bsq->size)
        {
            bsq->max_x = x;
            bsq->max_y = y;
            bsq->size = try;
        }
        try++;
    }
}
