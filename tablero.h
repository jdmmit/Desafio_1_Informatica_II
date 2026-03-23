#ifndef TABLERO_H
#define TABLERO_H

#include <cstdint>

// el tablero es un arreglo de filas, cada fila son bytes (ancho/8 bytes)
struct Tablero {
    uint8_t** filas;
    int alto;
    int ancho;
    int bytesXfila;
};

Tablero* crearTablero(int ancho, int alto);
void destruirTablero(Tablero* t);
bool getBit(Tablero* t, int fila, int col);
void setBit(Tablero* t, int fila, int col, bool valor);
int limpiarFilas(Tablero* t);
void imprimirTablero(Tablero* t, int py, int px, int tipo, int rot);

#endif
