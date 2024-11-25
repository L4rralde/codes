# Tarea 7

## Ejercicio 1

Mi solución sirve para comprimir cualquier tipo de archivo ascii.
### Compilacion:

```sh
gcc include/files/files.c include/pgm1/pgm1.c include/huffman/huffman.c src/compression.c -o output/compression.o -lm
```

### Uso:

```sh
./output/compression.o <input_path> <unzip_path>
```

## Ejercicio 2

### Compilación:

```sh
gcc include/files/files.c include/pgm1/pgm1.c src/image_cripto.c -o output/image_cripto.o -lm
```

### Uso:

```sh
./output/image_cripto.o <img_in> <img_out>
```