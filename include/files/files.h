/*
Autor: Emmanuel Alejandro Larralde Ortiz | emmanuel.larralde@cimat.mx
Descripción:
    Librería con funciones para leer archivos de texto y csv.
*/

#ifndef FILES_H
#define FILES_H

#include <stdlib.h>

char **read_lines(char *, int *);
char *read_file(char *, int *);
void write_file(char *, char *);
void write_file_bytes(char *, char *, int);
void free_lines(char **, int);
char ***read_csv(char *, int *, int *);
void free_csv(char ***, int, int);
int write_csv(char *, char ***, int, int);

char ***copy_csv_data(char ***, int, int);

#endif