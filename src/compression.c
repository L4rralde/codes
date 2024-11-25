/*
    Autor: Emmanuel Alejandro Larralde Ortiz | emmanuel.larralde@cimat.mx
    Descripción:
        Comprime y descomprime archivos usando el algoritmo de Huffman.
    Compilacion:
        gcc include/files/files.c include/pgm1/pgm1.c include/huffman/huffman.c src/compression.c -o output/compression.o -lm
    Uso:
        ./output/compression.o <input_path> <unzip_path>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/huffman/huffman.h"
#include "../include/pgm1/pgm1.h"


int main(int argc, char **argv){
    if(argc < 3){
        printf("./output/compression.o <input_path> <unzip_path>\n");
        return 0;
    }

    //Si es una imagen, calcula el numero de pixeles.
    int n_pixels = 0;
    int is_img = strstr(argv[1], "pgm") != NULL;
    if(is_img){
        int rows, cols;
        unsigned char **img = pgmRead(argv[1], &rows, &cols);
        n_pixels = rows * cols;
        printf("Number of pixels: %d\n", n_pixels);
        free(img[0]);
        free(img);
    }

    //Genera los codigos de huffman del contenido de un archivo.
    struct QElement **hist = generate_code(argv[1], "tmp/code.txt");
    if(hist == NULL){
        printf("Couldn't generate Huffman's code\n");
        return 0;
    }
    printf("Huffman codes file: tmp/code.txt\n");

    //Calcula el valor esperado de la longitud de los codigos de Huffman.
    float expected = 0;
    for(int i=0; i<256; ++i){
        expected += hist[i]->code->len * hist[i]->freq;
        free_qe(hist[i]);
    }
    free(hist);

    //Carga los codigos recien generados
    struct Code **code_dict = codes_from_file("tmp/code.txt");

    //Comprime el archivo
    struct Stats *stats = zip_file(argv[1], "zipped.bin", code_dict);
    printf("Compressed file: zipped.bin\n");
    expected /= stats->old_size;
    
    //Descomprime el archivo
    unzip_file("zipped.bin", argv[2], code_dict);

    //Imprime las estadísticas.
    printf("Original size: %d bytes\n", stats->old_size);
    printf("New size: %d bytes\n", stats->new_size);
    printf("Average char length: %f bits\n", expected);
    printf("Compression ratio: %f\n", (float) stats->old_size/stats->new_size);
    if(is_img)
        printf("bpp: %f\n", 8.0 * stats->new_size/n_pixels);

    free_codes(code_dict);
    free(stats);
    return 0;
}
