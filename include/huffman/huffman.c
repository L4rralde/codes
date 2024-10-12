#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../files/files.h"

struct Code{
    int len;
    unsigned int num;
    char *str;
};

struct Code *new_code(){
    struct Code *new_c = (struct Code *) malloc(sizeof(struct Code));
    new_c->len = 0;
    new_c->num = 0;
    return new_c;
}

struct Code *copy_code(struct Code *code){
    struct Code *new_c = new_code();
    new_c->len = code->len;
    new_c->num = code->num;
    if(code->str != NULL){
        new_c->str = (char *) malloc(sizeof(code->len + 1) * sizeof(char));
        strcpy(new_c->str, code->str);
    }
    return new_c;
}

void free_code(struct Code *code){
    if(code->str != NULL)
        free(code->str);
    free(code);
}

char *code_to_str(struct Code *code){
    int len = code->len;

    char *str = (char *) malloc((len + 1) * sizeof(char));
    str[len] = '\0';
    int copy = code->num;
    printf("--%d\n", copy);
    for(int i=len-1; i>=0; --i){
        str[i] = (copy & 1) + 48;
        copy = copy >> 1;
    }
    return str;
}

struct Code *code_from_str(char *str){
    int len = strlen(str);
    if(len == 0)
        return NULL;
    struct Code *code = new_code();
    code->len = len;
    code->num = 0;
    for(int i=0; i<len; ++i)
        code->num = (code->num << 1) + (str[i] == '1');
    code->str = (char *) malloc(sizeof(len + 1) * sizeof(char));
    strcpy(code->str, str);
    return code;
}

struct Code **codes_from_file(char *fpath){
    int n = 0;
    char **str_codes = read_lines(fpath, &n);
    struct Code **code_dict = (struct Code **) malloc(sizeof(struct Code *) * 256);
    for(int i=0; i<256; ++i)
        code_dict[i] = code_from_str(str_codes[i]);
    free_lines(str_codes, n);

    return code_dict;
}

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

struct QElement *new_qe(int freq){
    struct QElement *qe = (struct QElement *) malloc(sizeof(struct QElement));
    qe->freq = freq;
    qe->c = 0;
    qe->code = new_code();
    qe->_valid_code = 1;
    qe->code->len = 0;
    qe->code->num = 0;
    return qe;
}

struct QElement *copy_qe(struct QElement *qe){
    struct QElement *qe_cp = new_qe(qe->freq);
    qe_cp->c = qe->c;
    qe_cp->code = copy_code(qe->code);
    qe_cp->left = qe->left;
    qe_cp->right = qe->right;
    qe_cp->next = qe->next;

    return qe_cp;
}


void free_qe(struct QElement *qe){
    if(qe->_valid_code){
        free_code(qe->code);
        qe->_valid_code = 0;
    }
    free(qe);
}

void free_tree(struct QElement *root){
    if(root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free_qe(root);
}

struct HeapQ *new_q(){
    struct HeapQ *q = (struct HeapQ *) malloc(sizeof(struct HeapQ));
    q->len = 0;
    return q;
}

void print_q(struct HeapQ* q){
    struct QElement *ptr;
    ptr = q->head;
    printf("len=%d: ", q->len);
    for(int i=0; i<q->len; ++i){
        printf("%c:%d ", ptr->c, ptr->freq);
        ptr = ptr->next;
    }
    printf("\n");
}

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

struct QElement *pop(struct HeapQ *q){
    struct QElement *aux = q->head;
    q->head = q->head->next;
    q->len--;
    return aux;
}

struct QElement **get_file_hist(char *fpath, int *total){
    FILE *file = fopen(fpath, "r");
    if(file == NULL)
        return NULL;
    struct QElement **hist = (struct QElement **) malloc(256 * sizeof(struct QElement *));
    for(int i=0; i<256; ++i){
        hist[i] = new_qe(0);
        hist[i]->c = (char) i;
    }
    
    int c, cnt;
    for(cnt=0;  (c = fgetc(file)) != EOF; ++cnt)
        hist[c]->freq++;

    *total = cnt;
    fclose(file);
    return hist;
}

int compare_by_freq(const void *arg1, const void *arg2){
    struct QElement *qe1, *qe2;
    qe1 = * (struct QElement **) arg1;
    qe2 = * (struct QElement **) arg2;

    return qe1->freq - qe2->freq;
}

void set_codes(struct QElement *tree, int parent_code, int code_len){
    if(tree == NULL)
        return;
    tree->code->num = parent_code;
    tree->code->len = code_len;
    printf("%d\n", parent_code);
    set_codes(tree->left, (parent_code << 1) + 1, code_len + 1);
    set_codes(tree->right, parent_code << 1, code_len + 1);
}

struct QElement *huffman(struct QElement **hist){
    struct HeapQ *q = new_q();
    for(int i=0; i<256; ++i)
        if(hist[i]->freq > 0)
            push(q, hist[i]);
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

int zip_file(char *src_file, char *dst_file, struct Code **code_dict){
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
    for(int i=0; i<cs_n; ++i){
        c = cs[i];
        if(code_dict[c] ==  NULL){
            printf("ERROR: Not recognized character\n");
            fclose(dst);
            free(cs);
            return -1;
        }
        curr_code = code_dict[c];
        printf("Code. %c: %s\n", c, curr_code->str);
        shifts = curr_code->len;
        buffer = buffer << shifts;
        buffer |= curr_code->num;
        len_buffer += shifts;
        printf("buffer: len=%d, cont=%lx\n", len_buffer, buffer);
        while(len_buffer >= 8){
            byte = buffer & (255 << (len_buffer - 8));
            byte = buffer >> (len_buffer - 8);
            buffer ^= (byte << (len_buffer - 8));
            len_buffer -= 8;
            fprintf(dst, "%c", byte);
            printf("Wrote byte: %x\n", byte);
            printf("buffer: len=%d, cont=%lx\n", len_buffer, buffer);
        }
    }
    if(len_buffer > 0){
        byte = buffer & 255;
        byte = byte << (8 - len_buffer);
        len_buffer = 0;
        printf("Wrote byte: %x\n", byte);
        fprintf(dst, "%c", byte);
    }
    free(cs);
    fclose(dst);
    return 0;
}

int match_code(struct Code **code_dict, unsigned long int buffer, int len){
    printf("Buffer: %lx\n", buffer);
    unsigned long int xor_mask;
    struct Code *curr_code;
    for(int i=0; i<256; ++i){
        curr_code = code_dict[i];
        if(curr_code == NULL)
            continue;
        if(len < curr_code->len)
            continue;
        xor_mask = (unsigned long int) curr_code->num << (len - curr_code->len);
        printf("c=%c: xor_mask: %lx. Code:%s, num:%u\n", i, xor_mask, curr_code->str, curr_code->num);
        if(((buffer ^ xor_mask) >> (len - curr_code->len)) == 0)
            return i;
    }
    return -1;
}

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
        printf("read_byte: %x\n", c);
        buffer = buffer << 8;
        buffer_len += 8;
        buffer |= c;
        printf("%lx. len=%d\n", buffer, buffer_len);
        while(1){
            match = match_code(code_dict, buffer, buffer_len);
            if(match == -1){
                break;
            }
            match_c = code_dict[match];
            printf("%c: %s. Buffer_len=%d\n", (char) match, match_c->str, buffer_len);
            fprintf(dst, "%c", (char) match);
            buffer ^= match_c->num << (buffer_len - code_dict[match]->len);
            printf("%lx\n", buffer);
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