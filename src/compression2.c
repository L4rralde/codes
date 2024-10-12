#include <stdlib.h>
#include <stdio.h>
#include "../include/huffman/huffman.h"

int main(int argc, char **argv){
    if(argc < 3)
        return 0;
    int check = generate_code(argv[1], "tmp/code.txt");
    if(check == -1){
        printf("Couldn't generate Huffman's code\n");
        return 0;
    }

    struct Code **code_dict = codes_from_file("tmp/code.txt");

    zip_file(argv[1], "zipped.bin", code_dict);
    unzip_file("zipped.bin", argv[2], code_dict);

    free_codes(code_dict);
    return 0;
}