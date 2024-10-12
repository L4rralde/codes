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

struct QElement{
    int freq;
    unsigned char c;
    struct Code *code;
    struct QElement *next;
    struct QElement *left;
    struct QElement *right;
};

struct HeapQ{
    int len;
    struct QElement *head;
};

struct QElement *new_qe(int);
struct QElement *copy_qe(struct QElement *);
void free_qe(struct QElement *);
void free_tree(struct QElement *);
struct HeapQ *new_q();
void print_q(struct HeapQ*);
void free_q(struct HeapQ *);
int push(struct HeapQ *, struct QElement *);
struct QElement *pop(struct HeapQ *);

struct QElement **get_file_hist(char *, int *);
int compare_by_freq(const void *, const void *);
struct QElement *huffman(struct QElement **);
char *char_append(char *, char);
void set_codes(struct QElement *, int, int);
int save_code(struct QElement **, char *);

int zip_file(char *, char *, struct Code **);

int match_code(struct Code **, unsigned long int, int);
int unzip_file(char *, char *, struct Code **);

#endif