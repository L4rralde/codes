#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/huffman/huffman.h"


int main(int argc, char **argv){
    if(argc < 3)
        return 0;
    int check = generate_code(argv[1], argv[2]);
    return 0;
}
