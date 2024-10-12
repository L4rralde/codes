#ifndef PGM1_H
#define PGM1_H

unsigned char **pgmRead(char *,int *,int *);
void free_pgm(unsigned char **);
int pgmWrite(char *, int, int, unsigned char **,char *);
unsigned char **convolution(unsigned char **, int, int, float **, int);

unsigned char **gradient(unsigned char **, int, int);
unsigned char **sobel(unsigned char**, int, int);


#endif
