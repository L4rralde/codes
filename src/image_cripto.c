#include <stdlib.h>
#include <stdio.h>
#include "../include/pgm1/pgm1.h"

double logistic(double);
union{
    double d;
    unsigned long int ul;
} u;

struct Map{
    double *xs;
    int M;
    int cnt;
};

struct Map *new_map(int);
void free_map(struct Map *);
void push(struct Map *, double);
unsigned long int get_interaction(struct Map *);
double new_traj(struct Map *);

unsigned long int double_to_bits(double);
double bits_to_double(unsigned long int);

int main(int argc, char **argv){
    if(argc != 3){
        printf("./output/image_cripto.o <img_in> <img_out>\n");
        return 0;
    }
    int M = 10;
    srand(0);

    struct Map *map = new_map(M);
    int rows, cols;
    unsigned char **img = pgmRead(argv[1], &rows, &cols);
    
    double x;
    unsigned int *ptr = (unsigned int *) *img;
    unsigned int mask;
    for(int i=0; i<(rows*cols)/4.0; ++i){
        x = new_traj(map);
        mask = double_to_bits(x) & 0xFFFFFFFF;
        *ptr ^= mask;
        ptr++;
    }
    pgmWrite(argv[2], rows, cols, img, NULL);

    free_pgm(img);
    free_map(map);
    return 0;
}

double logistic(double x){
    return 4.0 * x * (1 - x);
}

struct Map *new_map(int M){
    struct Map *map = (struct Map *) malloc(sizeof(struct Map));

    map->cnt = 0;
    map->M = M;
    map->xs = calloc(M ,sizeof(double));
    double x = (double)rand()/(double)RAND_MAX;

    for(int i = M-1; i>=0; --i){
        *(map->xs + i) = x;
        x = logistic(x);
    }
    return map;
}

void free_map(struct Map *map){
    free(map->xs);
    free(map);
}

void push(struct Map *map, double x){
    for(int i=map->M-1; i>0; --i)
        *(map->xs + i) = *(map->xs + i - 1);
    *map->xs = x;
}

unsigned long int double_to_bits(double x){
    u.d = x;
    return u.ul;
}

unsigned long int get_interaction(struct Map *map){
    unsigned long int H = 0;
    for(int i=0; i<map->M; ++i)
        H ^= double_to_bits(*(map->xs + i));
    return H;
}

double new_traj(struct Map *map){
    double x = *map->xs;
    int idx = double_to_bits(x) % map->M;
    x = *(map->xs + idx);
    unsigned long int aux;

    map->cnt++;
    if(map->cnt == 6){
        map->cnt = 0;
        aux = double_to_bits(logistic(*map->xs)) + (255 & get_interaction(map));
        x = bits_to_double(aux);
    }else{
        x = logistic(*map->xs);
    }
    push(map, x);
    return x;
}

double bits_to_double(unsigned long int ul){
    u.ul = ul;
    return u.d;
}
