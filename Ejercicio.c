#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DENSIDAD 0.05


typedef struct {
    int    fila;
    int    col;
    double valor;
} ElementoEsparso;              

typedef struct {
    int filas;
    int cols;
    int nnz;                    
} Cabecera;              

/* ---------------- Paso 1: generacion ---------------- */

static int generar_matriz(const char *nombre, int filas, int cols) {
    FILE *f = fopen(nombre, "w");
    if (f == NULL) { perror(nombre); return 0; }

    long total = (long)filas * cols;
    long nnz   = (long)(total * DENSIDAD);

    char *ocupada = calloc((size_t)total, 1); 
    if (ocupada == NULL) { fclose(f); return 0; }

    fprintf(f, "%d %d %ld\n", filas, cols, nnz);

    long escritos = 0;
    while (escritos < nnz) {
        int i = rand() % filas;
        int j = rand() % cols;
        long pos = (long)i * cols + j;

        if (ocupada[pos]) continue;
        ocupada[pos] = 1;

        double valor = (rand() % 1000) / 10.0 + 0.1;
        fprintf(f, "%d %d %.2f\n", i, j, valor);
        escritos++;
    }

    free(ocupada);
    fclose(f);
    printf("  %s: %d x %d con %ld no nulos\n", nombre, filas, cols, nnz);
    return 1;
}

/* ---------------- Paso 2: texto a binario ---------------- */

static int convertir_a_binario(const char *entrada, const char *salida) {
    FILE *txt = fopen(entrada, "r");
    if (txt == NULL) { perror(entrada); return 0; }

    FILE *bin = fopen(salida, "wb");
    if (bin == NULL) { perror(salida); fclose(txt); return 0; }

    Cabecera cab;
    if (fscanf(txt, "%d %d %d", &cab.filas, &cab.cols, &cab.nnz) != 3) {
        printf("Cabecera invalida en %s\n", entrada);
        fclose(txt); fclose(bin);
        return 0;
    }

    fwrite(&cab, sizeof(Cabecera), 1, bin);

    ElementoEsparso e;
    int leidos = 0;
    while (fscanf(txt, "%d %d %lf", &e.fila, &e.col, &e.valor) == 3) {
        fwrite(&e, sizeof(ElementoEsparso), 1, bin);
        leidos++;
    }

    fclose(txt);
    fclose(bin);
    printf("  %s -> %s : %d elementos\n", entrada, salida, leidos);
    return 1;
}

/* ---------------- Paso 3: multiplicacion ---------------- */


