#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef struct s_bsq
{
    char    **map;
    int     nb_line;
    int     nb_col;
    char    c_empty;
    char    c_wall;
    char    c_draw;
    int     size;
    int     max_x;
    int     max_y;
}   t_bsq;

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

int ft_atoi(char *str)
{
    int i = 0;
    int n = 0;
    int digit;

    while (str[i] >= '0' && str[i] <= '9')
    {
        digit = str[i] - '0';
        if (n > (INT_MAX - digit) / 10)
            return (-1);
        n = n * 10 + digit;
        i++;
    }
    return (n);
}

int check_double(t_bsq *bsq)
{
    if (bsq->c_empty == bsq->c_wall
        || bsq->c_empty == bsq->c_draw
        || bsq->c_wall == bsq->c_draw)
        return (1);

    return (0);
}

int parse_param(t_bsq *bsq, char *line)
{
    int i;
    int len;

    i = 0;
    len = 0;
    while (line[len] != '\0')
        len++;

    if (line[0] < '0' || line[0] > '9')
        return (1);

    bsq->nb_line = ft_atoi(line);
    if (bsq->nb_line <= 0)
        return (1);

    while (line[i] >= '0' && line[i] <= '9')
        i++;

    if (len != i + 7)
        return (1);

    if (line[i] != ' ' || line[i + 2] != ' '
        || line[i + 4] != ' '
        || line[i + 6] != '\n')
        return (1);

    bsq->c_empty = line[i + 1];
    bsq->c_wall = line[i + 3];
    bsq->c_draw = line[i + 5];

    if (bsq->c_empty < 32 || bsq->c_empty > 126
        || bsq->c_wall < 32 || bsq->c_wall > 126
        || bsq->c_draw < 32 || bsq->c_draw > 126)
        return (1);

    if (check_double(bsq))
        return (1);

    return (0);
}

int add_mapline_map(t_bsq *bsq, char *line, int l)
{
    int i;

    if (bsq->map == NULL)
        bsq->map = calloc(bsq->nb_line + 1, sizeof(char *));

    if (bsq->map == NULL)
        return (1);

    bsq->map[l] = malloc(bsq->nb_col + 1);
    if (bsq->map[l] == NULL)
        return (1);

    i = 0;
    while (i < bsq->nb_col)
    {
        bsq->map[l][i] = line[i];
        i++;
    }
    bsq->map[l][i] = '\0';
    bsq->map[l + 1] = NULL;

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
    if (line[i] == '\n')
    {
        if (line[i + 1] != '\0')
            return (1);
    }
    else if (line[i] != '\0')
        return (1);

    if (bsq->nb_col != 0 && bsq->nb_col != i)
        return (1);

    bsq->nb_col = i;

    if (add_mapline_map(bsq, line, l) != 0)
        return (1);

    return (0);
}

int parse_map(t_bsq *bsq, FILE *file)
{
    char    *line;
    size_t  len;
    int     n;
    int     l;
    int     result;
    size_t read_len;

    bsq->nb_col = 0;
    line = NULL;
    len = 0;
    n = 0;
    l = 0;
    result = 0;

    while ((read_len = getline(&line, &len, file)) != (size_t)-1)
    {
        n++;
        if (n == 1)
        {
            if (parse_param(bsq, line))
            {
                result = 1;
                break;
            }
        }
        else
        {
            if (l >= bsq->nb_line
                || add_mapline(bsq, line, l) != 0)
            {
                result = 1;
                break;
            }
            l++;
        }
    }

    if (n == 0 || l == 0 || l != bsq->nb_line
        || ferror(file))
        result = 1;

    free(line);
    return (result);
}

int check_valid_chars(t_bsq *bsq)
{
    int y = 0;

    while (bsq->map[y] != NULL)
    {
        int x = 0;
        while (x < bsq->nb_col)
        {
            if (bsq->map[y][x] != bsq->c_empty
                && bsq->map[y][x] != bsq->c_wall)
                return (1);
            x++;
        }
        y++;
    }
    return (0);
}

void solve(t_bsq *bsq, int x, int y)
{
    int max;
    int try;
    int start;

    if (bsq->map[y][x] != bsq->c_empty)
        return ;

    try = bsq->size + 1;

    while (y + try <= bsq->nb_line && x + try <= bsq->nb_col)
    {
        max = 0;
        while (max < try)
        {
            start = x;
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

int bsq_start(t_bsq *bsq, FILE *file)
{
    int y;
    int x;

    if (parse_map(bsq, file))
        return (1);

    if (check_valid_chars(bsq))
        return (1);

    y = 0;
    while (bsq->map[y] != NULL)
    {
        x = 0;
        while (x < bsq->nb_col)
        {
            solve(bsq, x, y);
            x++;
        }
        y++;
    }

    printmap(bsq);
    return (0);
}

int main(int ac, char **av)
{
    t_bsq   bsq;
    FILE    *file;
    int     i;

    i = 1;
    while (i < ac || (ac == 1 && i == 1))
    {
        bsq.map = NULL;
        bsq.size = 0;
        bsq.max_x = 0;
        bsq.max_y = 0;

        if (ac == 1)
            file = stdin;
        else
            file = fopen(av[i], "r");

        if (!file)
            fprintf(stderr, "map error\n");
        else
        {
            if (bsq_start(&bsq, file))
                fprintf(stderr, "map error\n");

            if (file != stdin)
                fclose(file);
        }

        free_map(bsq.map);

        if (i < ac - 1)
            fprintf(stdout, "\n");
        i++;
    }
    return (0);
}
