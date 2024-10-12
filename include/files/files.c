/*
Autor: Emmanuel Alejandro Larralde Ortiz | emmanuel.larralde@cimat.mx
Descripción:
    Librería con funciones para leer archivos de texto y csv.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"

//Duplicado: copia un string en una nueva localidad de memoria.
char *_strcp(char *str){
    int len = 0;
    while(str[len++] != '\0');
    char *copy = (char *) malloc(len * sizeof(char));
    for(int i=0; i<len; ++i)
        copy[i] = str[i];
    return copy;
}

//Genera un arreglo dinámico con todas las líneas de un archivo de texto.
char **read_lines(char *fpath, int *nlines){
    //fpath: ruta al archivo de texto.
    //*nlines: Puntero de la cantidad de líneas.
    FILE * fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    //Abre el archivo.
    fp = fopen(fpath, "r");
    if (fp == NULL)
        return NULL;
    
    //Aloja memoria para cada string correspondiente a una línea.
    int line_cnt = 0;
    int max_lines = 8;
    char **lines = (char **) malloc(max_lines * sizeof(char *));
    if(lines == NULL){
        printf("Couldn't alloc lines\n");
        return (char **) NULL;
    }
    char **new_lines;

    //Lee una línea mientras halla líneas que leer.
    while((read = getline(&line, &len, fp)) != -1){
        //Reasigna más memoria si se va a superar la que ya se solicitó
        if(line_cnt == max_lines){
            max_lines += 8;
            new_lines = (char **) realloc(lines, max_lines * sizeof(char *));
            if(new_lines == NULL){
                printf("Coulnd't reallocate lines");
                return lines;
            }
            lines = new_lines;
        }
        //Remueve el salto de línea de la línea leída.
        line[strcspn(line, "\n")] = '\0';
        //Copia la línea leída al arreglo dinámico de líneas.
        lines[line_cnt++] = _strcp(line);
    }

    //Reajusta el tamaño del arreglo para no usar memoria de más
    new_lines = (char **) realloc(lines, line_cnt * sizeof(char *));
    if(new_lines)
        lines = new_lines;
    *nlines = line_cnt;

    free(line);
    fclose(fp);
    return lines;
}

//Libera la memoria dinámica usada por el arreglo dinámico
//generado por la función read_lines
void free_lines(char **lines, int n){
    for(int i=0; i<n; ++i)
        free(lines[i]);
    free(lines);
}

//Lee un archivo csv y regresa un arreglo bidimensional de strings
//que representan las filas y columnas del archivo.
char ***read_csv(char *fpath, int *nlines, int *ncols){
    char **lines;
    int _nlines;
    
    //Lee las líneas del archivo.s
    lines = read_lines(fpath, &_nlines);
    if(lines == (char **) NULL)
        return (char ***) NULL;
    //De la primera línea, calcula la cantidad de columnas
    int comma_cnt = 0;
    for(int i=0; lines[0][i] != '\0'; ++i)
        comma_cnt += lines[0][i] == ',';
    int csv_head_size = comma_cnt+1;

    //Aloja memoria para toda la tabla.
    char ***csv_lines, **csv_line, *csv_element;
    csv_lines = (char ***) malloc(_nlines * sizeof(char **));

    int e_size, line_c, max_buffer_size;
    for(int i=0; i<_nlines; ++i){//Para cada línea
        csv_line = (char **) malloc(csv_head_size * sizeof(char *));
        *(csv_lines + i) = csv_line;
        line_c = 0;
        max_buffer_size = 128;
        //Para cada columna
        for(int csv_element_cnt=0;  csv_element_cnt<csv_head_size; ++csv_element_cnt){
            csv_element = (char *) malloc(max_buffer_size * sizeof(char));
            *(csv_line + csv_element_cnt) = csv_element;
            //Detecta cada valor separado por coma.
            for(e_size = 0; lines[i][line_c]!= ',' && lines[i][line_c]!= '\0'; ++e_size){
                *(csv_element + e_size) = lines[i][line_c];
                line_c++;
            }
            line_c++;
            *(csv_element + e_size + 1) = '\0';
        }
        free(*(lines + i));
    }
    free(lines);

    *nlines = _nlines;
    *ncols = csv_head_size;
    return csv_lines;
}

//libera la memoria dinámica usada por la tabla generada por la función read_csv()
void free_csv(char ***csv_data, int nrows, int ncols){
    for(int i=0; i<nrows; ++i){
        for(int j=0; j<ncols; ++j)
            free(csv_data[i][j]);
        free(csv_data[i]);
    }
    free(csv_data);
}

//Escribe una tabla con la misma estructura que las generadas por
//la función read_csv() en un archivo .csv
int write_csv(char *fpath, char ***data, int nrows, int ncols){
    FILE *file;
    file = fopen(fpath, "w");
    if(file == (FILE *) NULL){
        printf("Couldn't create file %s\n", fpath);
        return -1;
    }
    for(int row=0; row<nrows-1; row++){ //Para todas las filas menos la última
        for(int col=0; col<ncols - 1; col++) //Para todas las columnas menos la última
            fprintf(file, "%s,", data[row][col]);
        fprintf(file, "%s\n", data[row][ncols - 1]);
    }
    //Para la última fila
    for(int col=0; col<ncols - 1; col++)
        fprintf(file, "%s,", data[nrows-1][col]);
    fprintf(file, "%s", data[nrows-1][ncols - 1]);

    fclose(file);
    return 0;
}

//Lee todos los caracteres de un archivo
char *read_file(char *fpath, int *n){
    FILE *file = fopen(fpath, "r");
    if (file == NULL)
        return NULL;
    int cnt;
    int c;
    int room = 1024;
    char *str = (char *) malloc(sizeof(char) * room);
    char *aux;
    for(cnt=0; 1; ++cnt){
        if(cnt == room){
            room += 1024;
            aux = (char *) realloc(str, room * sizeof(char));
            if(aux == NULL){
                fclose(file);
                return NULL;
            }
            str = aux;
        }
        c = fgetc(file);
        if(c == EOF)
            break;
        str[cnt] = (char) c;
    }
    fclose(file);
    str[cnt] = '\0';
    cnt++;
    aux = (char *) realloc(str, cnt * sizeof(char));
    if(aux != NULL)
        str = aux;
    *n = cnt;
    return str;
}

void write_file(char *fpath, char *str){
    FILE *file = fopen(fpath, "w");
    if (file == NULL)
        return;
    printf("Writing back to file %s\n", fpath);
    fprintf(file, "%s", str);
    fclose(file);
}

void write_file_bytes(char *fpath, char *str, int len){
    FILE *file = fopen(fpath, "w");
    if (file == NULL)
        return;
    printf("Writing back to file %s\n", fpath);
    for(int i=0; i<len; ++i){
        fprintf(file, "%c", str[i]);
    }
    fclose(file);
}
