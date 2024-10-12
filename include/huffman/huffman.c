/*
    Autor: Emmanuel Alejandro Larralde Ortiz | emmanuel.larralde@cimat.mx
    Descripción:
        Código fuente de una librería para hacer compresión y decompresión
        a partir del algoritmo de Huffman.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../files/files.h"

//Structura que almacena la codificacion de un caracter.
struct Code{
    int len;
    unsigned int num;
    char *str;
};

//Crea una nueva instancia del estruct Code
struct Code *new_code(){
    struct Code *new_c = (struct Code *) malloc(sizeof(struct Code));
    new_c->len = 0;
    new_c->num = 0;
    return new_c;
}

//Libera la memoria utilizada por una instancia de struct.
void free_code(struct Code *code){
    if(code->str != NULL)
        free(code->str);
    free(code);
}

//Librera una lista de Codes
void free_codes(struct Code **codes){
    for(int i=0; i<256; ++i)
        if(codes[i] != NULL)
            free_code(codes[i]);
    free(codes);
}

//Convierte un codigo numerico en un string.
char *code_to_str(struct Code *code){
    int len = code->len;

    char *str = (char *) malloc((len + 1) * sizeof(char));
    str[len] = '\0';
    int copy = code->num;
    for(int i=len-1; i>=0; --i){
        str[i] = (copy & 1) + 48;
        copy = copy >> 1;
    }
    return str;
}

//Crea una instancia de Code a partir de una cadena de caracteres
struct Code *code_from_str(char *str){
    int len = strlen(str);
    if(len == 0)
        return NULL;
    struct Code *code = new_code();
    code->len = len;
    code->num = 0;
    for(int i=0; i<len; ++i)
        code->num = (code->num << 1) + (str[i] == '1');
    code->str = (char *) malloc((len + 1) * sizeof(char));
    strcpy(code->str, str);
    return code;
}

//Carga los codigos almacenados en un archivo
struct Code **codes_from_file(char *fpath){
    int n = 0;
    char **str_codes = read_lines(fpath, &n);
    struct Code **code_dict = (struct Code **) malloc(sizeof(struct Code *) * 256);
    for(int i=0; i<256; ++i)
        code_dict[i] = code_from_str(str_codes[i]);
    free_lines(str_codes, n);

    return code_dict;
}

//estructura para construir un árbol de Huffman.
struct QElement{
    int freq;
    unsigned char c;
    struct Code *code;
    int _valid_code;
    int _is_from_hist;
    struct QElement *next;
    struct QElement *left;
    struct QElement *right;
};

//Estructura de una Cola de prioridad.
struct HeapQ{
    int len;
    struct QElement *head;
};

//Crea una instancia de QElement
struct QElement *new_qe(int freq){
    struct QElement *qe = (struct QElement *) malloc(sizeof(struct QElement));
    qe->freq = freq;
    qe->c = 0;
    qe->code = new_code();
    qe->_valid_code = 1;
    qe->_is_from_hist = 0;
    qe->code->len = 0;
    qe->code->num = 0;
    return qe;
}

//Libera la memoria utilizada por una instancia QElement
void free_qe(struct QElement *qe){
    if(qe->_valid_code){
        free_code(qe->code);
        qe->_valid_code = 0;
    }
    free(qe);
}

//Libera la memoria alojada durante la construcción del árbol
void free_tree(struct QElement *root){
    if(root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    if(root->_is_from_hist == 0)
        free_qe(root);
}

//Crea una instancia de HeapQ
struct HeapQ *new_q(){
    struct HeapQ *q = (struct HeapQ *) malloc(sizeof(struct HeapQ));
    q->len = 0;
    return q;
}

//Agrega un elemento a una cola de prioridad HeapQ
int push(struct HeapQ *q, struct QElement *qe){
    int freq = qe->freq;
    if(q->len == 0){
        q->head = qe;
        q->len = 1;
        return 0;
    }
    if(freq <= q->head->freq){
        qe->next = q->head;
        q->head = qe;
        q->len++;
        return 0;
    }
    struct QElement *ptr = q->head;
    int i;
    for(i=1; i<q->len; ++i){
        if(freq <= ptr->next->freq)
            break;
        ptr = ptr->next;
    }
    qe->next = ptr->next;
    ptr->next = qe;
    q->len++;
    return i;
}

//Toma el elemento de mayor prioridad de una cola HeapQ
struct QElement *pop(struct HeapQ *q){
    struct QElement *aux = q->head;
    q->head = q->head->next;
    q->len--;
    return aux;
}

//Obtiene el histograma de un archivo de texto.
struct QElement **get_file_hist(char *fpath, int *total){
    FILE *file = fopen(fpath, "r");
    if(file == NULL)
        return NULL;
    struct QElement **hist = (struct QElement **) malloc(256 * sizeof(struct QElement *));
    for(int i=0; i<256; ++i){
        hist[i] = new_qe(0);
        hist[i]->_is_from_hist = 1;
        hist[i]->c = (char) i;
    }
    
    int c, cnt;
    for(cnt=0;  (c = fgetc(file)) != EOF; ++cnt)
        hist[c]->freq++;

    *total = cnt;
    fclose(file);
    return hist;
}

//Asigna los codigos a los elementos del arbol de Huffman.
void set_codes(struct QElement *tree, int parent_code, int code_len){
    if(tree == NULL)
        return;
    tree->code->num = parent_code;
    tree->code->len = code_len;
    tree->code->str = code_to_str(tree->code);
    set_codes(tree->left, (parent_code << 1) + 1, code_len + 1);
    set_codes(tree->right, parent_code << 1, code_len + 1);
}

//Obtiene la codificación de huffman de los caracteres a partir de un histograma.
struct QElement *huffman(struct QElement **hist){
    struct HeapQ *q = new_q();
    for(int i=0; i<256; ++i){
        if(hist[i]->freq > 0)
            push(q, hist[i]);
    }
    struct QElement *new_node;
    while(q->len > 1){
        new_node = new_qe(0);
        new_node->left = pop(q);
        new_node->right = pop(q);
        new_node->freq = new_node->left->freq + new_node->right->freq;
        push(q, new_node);
    }
    new_node = pop(q);
    int code_len = 0;
    int root_code = 0;
    set_codes(new_node, root_code, code_len);
    free(q);
    return new_node;
}

//Guarda los codigos de huffman en un archivo de texto,
int save_code(struct QElement **hist, char *fname){
    FILE *file = fopen(fname, "w");
    char *aux_str;
    if(file == NULL)
        return -1;
    for(int i=0; i<256; ++i){
        if(hist[i]->freq == 0){
            fprintf(file, "\n");
        }else{
            aux_str = code_to_str(hist[i]->code);
            fprintf(file, "%s\n", aux_str);
            free(aux_str);
        }
    }
    fclose(file);
    return 0;
}

//Genera los codigos de Huffman del contenido de un archivo de texto.
struct QElement **generate_code(char *fpath, char *code_path){
    int total;
    struct QElement **hist = get_file_hist(fpath, &total);
    if(hist == NULL){
        printf("Couldn't calculate histogram\n");
        return NULL;
    }
    struct QElement *tree = huffman(hist);
    if(tree == NULL){
        printf("Couldn't build tree\n");
        return NULL;
    }
    save_code(hist, code_path);
    free_tree(tree);
    return hist;
}

//Estructura de datos con las estadísticas de la compresion.
struct Stats{
    int old_size;
    int new_size;
};

//Comprime un archivo a partir de un archivo con codigos de huffman.
struct Stats *zip_file(char *src_file, char *dst_file, struct Code **code_dict){
    int cs_n;
    char *cs = read_file(src_file, &cs_n);
    cs_n--;
    FILE *dst = fopen(dst_file, "w");
    int c;
    unsigned long int buffer = 0;
    int len_buffer = 0;
    int shifts;
    struct Code *curr_code;
    unsigned char byte;
    int cnt = 0;
    for(int i=0; i<cs_n; ++i){
        c = cs[i];
        if(code_dict[c] ==  NULL){
            printf("ERROR: Not recognized character\n");
            fclose(dst);
            free(cs);
            return NULL;
        }
        curr_code = code_dict[c];
        //printf("Code. %c: %s\n", c, curr_code->str);
        shifts = curr_code->len;
        buffer = buffer << shifts;
        buffer |= curr_code->num;
        len_buffer += shifts;
        //printf("buffer: len=%d, cont=%lx\n", len_buffer, buffer);
        while(len_buffer >= 8){
            byte = buffer & (255 << (len_buffer - 8));
            byte = buffer >> (len_buffer - 8);
            buffer ^= (byte << (len_buffer - 8));
            len_buffer -= 8;
            fprintf(dst, "%c", byte);
            cnt++;
            //printf("Wrote byte: %x\n", byte);
            //printf("buffer: len=%d, cont=%lx\n", len_buffer, buffer);
        }
    }
    if(len_buffer > 0){
        byte = buffer & 255;
        byte = byte << (8 - len_buffer);
        len_buffer = 0;
        //printf("Wrote byte: %x\n", byte);
        fprintf(dst, "%c", byte);
        cnt++;
    }
    free(cs);
    fclose(dst);
    struct Stats *stats = (struct Stats *) malloc(sizeof(struct Stats));
    stats->old_size = cs_n;
    stats->new_size = cnt;
    return stats;
}

//Encuentra si los bits mas significativos de un buffer corresponden a un caracter codificado.
int match_code(struct Code **code_dict, unsigned long int buffer, int len){
    //printf("Buffer: %lx\n", buffer);
    unsigned long int xor_mask;
    struct Code *curr_code;
    for(int i=0; i<256; ++i){
        curr_code = code_dict[i];
        if(curr_code == NULL)
            continue;
        if(len < curr_code->len)
            continue;
        xor_mask = (unsigned long int) curr_code->num << (len - curr_code->len);
        //printf("c=%c: xor_mask: %lx. Code:%s, num:%u\n", i, xor_mask, curr_code->str, curr_code->num);
        if(((buffer ^ xor_mask) >> (len - curr_code->len)) == 0)
            return i;
    }
    return -1;
}

//Descomprime un archivo a partir de un archivo con codigos de huffman.
int unzip_file(char *src_file, char *dst_file, struct Code **code_dict){
    FILE *src = fopen(src_file, "r");
    if (src == NULL)
        return -1;
    FILE *dst = fopen(dst_file, "w");
    unsigned long int buffer = 0;
    int buffer_len = 0;
    int c, match;
    struct Code *match_c;
    while ((c = fgetc(src)) != EOF){
        //printf("read_byte: %x\n", c);
        buffer = buffer << 8;
        buffer_len += 8;
        buffer |= c;
        //printf("%lx. len=%d\n", buffer, buffer_len);
        while(1){
            match = match_code(code_dict, buffer, buffer_len);
            if(match == -1){
                break;
            }
            match_c = code_dict[match];
            //printf("%c: %s. Buffer_len=%d\n", (char) match, match_c->str, buffer_len);
            fprintf(dst, "%c", (char) match);
            buffer ^= match_c->num << (buffer_len - code_dict[match]->len);
            //printf("%lx\n", buffer);
            buffer_len -= match_c->len;
        }
    }
    while(buffer_len >= 0){
        match = match_code(code_dict, buffer, buffer_len);
        if(match == -1){
            fclose(src);
            fclose(dst);
            return -1;
        }
        fprintf(dst, "%c", (char) match);
        buffer ^= code_dict[match]->num << (buffer_len - code_dict[match]->len);
        buffer_len -= code_dict[match]->len;
    }
    fclose(src);
    fclose(dst);
    return 0;
}
