#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/huffman/huffman.h"


int main(int argc, char **argv){
    if(argc < 3)
        return 0;
    int total;
    struct QElement **hist = get_file_hist(argv[1], &total);
    struct QElement *tree = huffman(hist);
    if(tree == NULL)
        printf("WARNING\n");

    char *aux_str;
    for(int i=0; i<256; ++i){
        if(hist[i]->freq > 0){
            aux_str = code_to_str(hist[i]->code);
            printf("%c: freq=%d code=%s\n", hist[i]->c, hist[i]->freq, aux_str);
            free(aux_str);
        }
    }

    save_code(hist, argv[2]);
    free_tree(tree);
    for(int i=0; i<256; ++i)
        free_qe(hist[i]);
    free(hist);
    return 0;
}
