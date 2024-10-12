#include <stdlib.h>
#include <stdio.h>
#include "../include/huffman/huffman.h"

int main(int argc, char **argv){
    if(argc < 4)
        return 0;
    struct Code **code_dict = codes_from_file(argv[3]);
    unzip_file(argv[1], argv[2], code_dict);
    return 0;
}
