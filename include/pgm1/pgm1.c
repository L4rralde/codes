/**********************************************************
 * This code was written by Richard Zanibbi in May 1998
 * based on code by Donald Quon and Hai Pham,
 * and Dr. R. T. Whitaker of the University of Tennesee
 * for use in Queen's University's CISC 859 course

 * Modified to make use of dynamic memory and independent
 * reading and writing functions by
 * R. Hasimoto-CIMAT, Sept. 2022.
 * Emmanuel Alejandro Larralde Ortiz, student @CIMAT.
************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXLENGTH 100
#define MAXVALUE  255
unsigned char **pgmRead(char *fileName,int *rows,int *cols)
{
      FILE *filePointer;   
      char line[MAXLENGTH]; /* for character input from file */
      int maximumValue = 0; /* max value from header */
      int binary;           /* flag to indicate if file is binary (P5) or ascii (P2)*/
      long i,j;             /* (i,j) for loops */
      int test,temp;        /* for detecting EOF(test) and temp storage */
      unsigned char *tempPtr, **image;

      /* Open the file, return an error if necessary. */
      if ((filePointer = fopen(fileName,"r")) == NULL) {
	   printf ("ERROR: cannot open file: %s\n\n", fileName);
	   fclose (filePointer);
	   return (0);
      }
    
      *cols = *rows =0;

      /* Check the file signature ("Magic Numbers" P2 and P5); skip comments
         and blank lines (CR with no spaces before it).*/
      fgets (line,MAXLENGTH,filePointer);
      while (line[0]=='#' || line[0]=='\n') 
           fgets (line,MAXLENGTH,filePointer);

      if (line[0]=='P' && (line[1]=='2')) {
	   binary = 0;
	 /*   printf ("\nFile Format: P2\n"); */
      }
      else if (line[0]=='P' && (line[1]=='5')) {
	   binary = 1;
	  /*  printf ("\nFORMAT: P5\n"); */
      }
      else {
	   printf ("ERROR: incorrect file format\n\n");
	   fclose (filePointer);
	   return (0);
      } 

      /* Input the width, height and maximum value, skip comments and blank
       * lines. */
      fgets (line,MAXLENGTH,filePointer);
      while (line[0]=='#' || line[0]=='\n') 
         fgets (line,MAXLENGTH,filePointer);
      sscanf (line,"%d %d",cols,rows);

      fgets (line,MAXLENGTH,filePointer);
      while (line[0]=='#' || line[0]=='\n') 
         fgets(line,MAXLENGTH,filePointer);
      sscanf (line,"%d",&maximumValue);

      if ((*cols)<1 ||(*rows)<1 || maximumValue<0 || maximumValue>MAXVALUE){
	   printf ("ERROR: invalid file specifications (cols/rows/max value)\n\n");
	   fclose (filePointer);
	   return (0);
      }

      // creating memory for the input image
      image = (unsigned char **)malloc((*rows) * sizeof(unsigned char *));
      if (image == NULL)
      {
         puts("\nFailure to allocate room for pointers");
         return(NULL);
      }

      tempPtr = (unsigned char *) malloc((*rows)*(*cols)*sizeof(unsigned char));
      if (tempPtr == NULL)
      {
         puts("\nFailure to allocate room for the array");
         return(NULL);
      }
      for (i = 0; i < *rows; i++)
      {
        image[i] = tempPtr + ( i * (*cols) );
      }


      /* Read in the data for binary (P5) or ascii (P2) PGM formats   */
      if (binary) {
	   for (i = 0; i < (*rows); i++) {
	        fread((void *)&(image[i][0]),
		  sizeof(unsigned char),(*cols),filePointer); 
		if (feof(filePointer)) break;
	   }
      }
      else {
	   for (i= 0; i < (*rows); i++) {
	        for (j =0; j < (*cols); j++) { 
	             test = fscanf (filePointer,"%d",&temp);
		     if (test == EOF) break;
		     image[i][j] = (unsigned char)temp;
		}
		if (test == EOF) break;
	   }
      }

      /* close the file and return 1 indicating success */
      fclose (filePointer);
      return (image);
}

void free_pgm(unsigned char **img){
    free(*img);
    free(img);
}

/* INPUT: a filename (char*), the dimensions of the pixmap (rows,cols of
 *   type long), and a pointer to a 2D array (MAXROWS x MAXCOLS) in row
 *   major order.
 * OUTPUT: an integer is returned indicating if the desired file was written
 *   (in P5 PGM format (binary)).  A 1 is returned if the write was completed
 *   and 0 if it was not.  An error message is returned if the file is not
 *   properly opened.  
 */ 
int pgmWrite(char* filename, int rows,int cols,unsigned char **image,char* comment_string)
{
      FILE* file;        /* pointer to the file buffer */
      long nwritten = 0; /* counter for the number of pixels written */
      long i;          /* for loop counters */

      /* open the file; write header and comments specified by the user. */
      if ((file = fopen(filename, "w")) == NULL)	{
           printf("ERROR: file open failed\n");
	   return(0);
      }
      fprintf(file,"P5\n");

      if (comment_string != NULL) fprintf(file,"# %s \n", comment_string);
    
      /* write the dimensions of the image */	
      fprintf(file,"%d %d \n", cols, rows);

      /* NOTE: MAXIMUM VALUE IS WHITE; COLOURS ARE SCALED FROM 0 - */
      /* MAXVALUE IN A .PGM FILE. */
      
      /* WRITE MAXIMUM VALUE TO FILE */
      fprintf(file, "%d\n", (int)255);

      /* Write data */
      for (i = 0;i < rows;i++) {
          nwritten += fwrite((void*)image[i],sizeof(unsigned char),cols,file);
      }
      printf("nwritten = %ld,",nwritten);
      	
      fclose(file);
      return(1);
}

/*
Aplica un filtro con un kernel cuadrado de tamaño kn*kn sobre una imagen img de tamaño img_rows * img_cols
usando la operación de correlación cruzada y devuelve la imagen producto de aplicar este filtro.
*/
unsigned char **convolution(unsigned char **img, int img_rows, int img_cols, float **kernel, int kn){
    //Dimensiones de la nueva imagen
    int nimg_nrows = img_rows - kn + 1;
    int nimg_ncols = img_cols - kn + 1;

    //Instanciación de la nueva imagen.
    unsigned char *aux_ptr, **new_img;
    new_img = (unsigned char **)malloc(nimg_nrows * sizeof(unsigned char *));
    if (new_img == NULL){
        printf("Failure to allocate room for pointers\n");
        return NULL;
    }
    aux_ptr = (unsigned char *) malloc(nimg_ncols * nimg_nrows * sizeof(unsigned char));
    if(aux_ptr == NULL){
        printf("Failure to allocate room for pointers\n");
        return NULL;
    }   
    for(int i=0; i<nimg_nrows; ++i)
        new_img[i] = aux_ptr + i*nimg_ncols;

    //Correlación cruzada.
    int half_kn = kn >> 1;
    int row_limit = img_rows - half_kn;
    int col_limit = img_cols - half_kn;
    float acc;
    //Para cada renglon por el que pasara la ventana.
    for(int i = half_kn; i < row_limit; ++i){
        //Para cada columna por la que pasara la ventana
        for(int j = half_kn; j < col_limit; ++j){
            //Multiplicacion elemento a elemento de la ventana y el kernel.
            acc = 0;
            for(int ki = -half_kn; ki <= half_kn; ++ki){
                for(int kj = -half_kn; kj <= half_kn; ++kj){
                    acc += img[i+ki][j+kj] * kernel[ki+half_kn][kj+half_kn];
                }
            }
            //El resultado de la operacion de esta ventana se almacena en la imagen nueva
            new_img[i-half_kn][j-half_kn] = (unsigned char) acc;
        }
    }

    return new_img;
}

//Calcula el gradiente de una imagen como sqrt(dI/dx^2 + dI/dy^2)
unsigned char **gradient(unsigned char **img, int rows, int cols){
    //Instanciacion de la nueva imagen
    unsigned char *ucptr, **new_img;
    int nimg_rows, nimg_cols;
    nimg_rows = rows - 2;
    nimg_cols = cols - 2;
    new_img = (unsigned char **) malloc(nimg_rows * sizeof(unsigned char *));
    if (new_img == NULL){
        printf("Failure to allocate room for pointers\n");
        return NULL;
    }
    ucptr = (unsigned char *) malloc(nimg_cols * nimg_rows * sizeof(unsigned char));
    if(ucptr == NULL){
        printf("Failure to allocate room for pointers\n");
        return NULL;
    }
    for(int i=0; i < nimg_rows; ++i)
        new_img[i] = ucptr + i*nimg_cols;

    //Cálculo de gradientes.
    float gx, gy, g;
    float max = 0;
    for(int i=1; i < rows-1; ++i){
        for(int j=1; j < cols-1; ++j){
            //dI/dx = (I[i][j+1] - I[i][j-1])/2
            gx = 0.5*(img[i][j+1] - img[i][j-1]);
            //dI/dy = (I[i-1][j] - I[i+1][j)/2.
            //Recordemos que los índices de las columnas es de
            //arriba hacia abajo, lo contrario a un plano cartesiano.
            gy = 0.5*(img[i-1][j] - img[i+1][j]);
            //Grad = sqrt(gx**2 + gy**2)
            g = sqrt(gx*gx + gy*gy);
            //cast explícito para evitar acceder a memoria no alojada.
            new_img[i-1][j-1] = (unsigned char) g;
            if(g > max)
                max = g;
        }
    }

    //Aumentar brillo o
    float factor;
    factor = 255.0/((unsigned char) max);
    for(int i=0; i<nimg_rows; ++i)
        for(int j=0; j<nimg_cols; ++j)
            new_img[i][j] =  (unsigned char) (new_img[i][j]*factor);

    return new_img;
}

//Aplica un filtro sobel a una imagen. S = sqrt(Sx^2 + Sy^2)
unsigned char **sobel(unsigned char **img, int rows, int cols){
    //Kernel Sx, ya reflejado en sus ejes y normalizado
    float Sx[3][3] = {
        {-1.0/8, 0, 1.0/8},
        {-2.0/8, 0, 2.0/8},
        {-1.0/8, 0, 1.0/8},
    };
    //Kernel Sy, ya reflejado en sus ejes y normalizado
    float Sy[3][3] = {
        {1.0/8, 2.0/8, 1.0/8},
        {0, 0, 0},
        {-1.0/8, -2.0/8, -1.0/8},
    };

    //Conversión de double[][] a double** para los kernels.
    unsigned char **sx, **sy;
    float *sx_kernel_rows[] = {Sx[0], Sx[1], Sx[2]};
    float **sx_kernel = sx_kernel_rows;
    float *sy_kernel_rows[] = {Sy[0], Sy[1], Sy[2]};
    float **sy_kernel = sy_kernel_rows;

    sx = convolution(img, rows, cols, sx_kernel, 3); //Sobel horizontal
    sy = convolution(img, rows, cols, sy_kernel, 3); //Sobel vertical.

    //Instanciación de la nueva imagen
    unsigned char *ucptr, **new_img;
    int nimg_rows, nimg_cols;
    nimg_rows = rows - 2;
    nimg_cols = cols - 2;
    new_img = (unsigned char **) malloc(nimg_rows * sizeof(unsigned char *));
    if (new_img == NULL){
        printf("Failure to allocate room for pointers\n");
        return NULL;
    }
    ucptr = (unsigned char *) malloc(nimg_cols * nimg_rows * sizeof(unsigned char));
    if(ucptr == NULL){
        printf("Failure to allocate room for pointers\n");
        return NULL;
    }
    for(int i=0; i < nimg_rows; ++i)
        new_img[i] = ucptr + i*nimg_cols;

    //Out[i][j] = sqrt(sx[i][j]^2 + sy[i][j]^2)
    float s, max;
    max = 0;
    for(int i=0; i < nimg_rows; ++i){
        for(int j=0; j < nimg_cols; ++j){
            s = sqrt(sx[i][j]*sx[i][j] + sy[i][j]*sy[i][j]);
            new_img[i][j] = (unsigned char) s;
            if(s > max)
                max = s;
        }
    }

    //Aumentar brillo o renormalización
    float factor;
    factor = 255.0/((unsigned char) max);
    for(int i=0; i<nimg_rows; ++i)
        for(int j=0; j<nimg_cols; ++j)
            new_img[i][j] =  (unsigned char) (new_img[i][j]*factor);


    free(sx[0]);
    free(sx);
    free(sy[0]);
    free(sy);

    return new_img;
}