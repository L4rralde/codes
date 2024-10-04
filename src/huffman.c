#include <stdlib.h>
#include <stdio.h>

int *get_file_hist(char *, int *);

int main(int argc, char **argv){
    if(argc < 2)
        return 0;
    int total;
    int *hist = get_file_hist(argv[1], &total);

    printf("Total: %d\n", total);
    free(hist);
    return 0;
}


int *get_file_hist(char *fpath, int *total){
    FILE *file = fopen(fpath, "r");
    if(file == NULL)
        return NULL;
    int *hist = calloc(256, sizeof(int));
    if(hist == NULL){
        printf("Couldn't allocate memoery\n");
        return NULL;
    }
    int c, cnt;
    for(cnt=0;  (c = fgetc(file)) != EOF; ++cnt){
        printf("%c", c);
        hist[c]++;
    }

    *total = cnt;
    fclose(file);
    return hist;
}