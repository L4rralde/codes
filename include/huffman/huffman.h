/*
    Autor: Emmanuel Alejandro Larralde Ortiz | emmanuel.larralde@cimat.mx
    Descripción:
        Header de una librería para hacer compresión y decompresión
        a partir del algoritmo de Huffman.
*/

#ifndef HEAPQ_H
#define HEAPQ_H

struct Code{
    int len;
    unsigned int num;
    char *str;
};

char *code_to_str(struct Code *);
struct Code *code_from_str(char *);
struct Code **codes_from_file(char *);
void free_codes(struct Code **);

struct QElement{
    int freq;
    unsigned char c;
    struct Code *code;
    int _valid_code;
    struct QElement *next;
    struct QElement *left;
    struct QElement *right;
};

struct HeapQ{
    int len;
    struct QElement *head;
};

struct QElement *new_qe(int);
void free_qe(struct QElement *);
void free_tree(struct QElement *);
struct HeapQ *new_q();
int push(struct HeapQ *, struct QElement *);
struct QElement *pop(struct HeapQ *);

struct QElement **get_file_hist(char *, int *);
struct QElement *huffman(struct QElement **);
void set_codes(struct QElement *, int, int);
int save_code(struct QElement **, char *);

struct QElement **generate_code(char *, char *);
struct Stats{
    int old_size;
    int new_size;
};

struct Stats *zip_file(char *, char *, struct Code **);

int match_code(struct Code **, unsigned long int, int);
int unzip_file(char *, char *, struct Code **);

#endif