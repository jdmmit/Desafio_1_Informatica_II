#include "tablero.h"
#include "pieza.h"
#include <iostream>
using namespace std;

Tablero* crearTablero(int ancho, int alto) {
    Tablero* t   = new Tablero;
    t->ancho      = ancho;
    t->alto       = alto;
    t->bytesXfila = ancho / 8;
    t->filas      = new uint8_t*[alto];

    for (int i = 0; i < alto; i++) {
        t->filas[i] = new uint8_t[t->bytesXfila];
        for (int j = 0; j < t->bytesXfila; j++)
            t->filas[i][j] = 0;
    }
    return t;
}

void destruirTablero(Tablero* t) {
    for (int i = 0; i < t->alto; i++)
        delete[] t->filas[i];
    delete[] t->filas;
    delete t;
}

// lee un bit del tablero usando mascara
bool getBit(Tablero* t, int fila, int col) {
    if (fila < 0 || fila >= t->alto || col < 0 || col >= t->ancho) return false;
    int byteIdx = col / 8;
    int bitIdx  = 7 - (col % 8);
    return (t->filas[fila][byteIdx] >> bitIdx) & 1;
}

// escribe un bit del tablero usando OR o AND con mascara
void setBit(Tablero* t, int fila, int col, bool valor) {
    if (fila < 0 || fila >= t->alto || col < 0 || col >= t->ancho) return;
    int byteIdx = col / 8;
    int bitIdx  = 7 - (col % 8);
    if (valor)
        t->filas[fila][byteIdx] |= (1 << bitIdx);
    else
        t->filas[fila][byteIdx] &= ~(1 << bitIdx);
}

// elimina filas llenas y baja las de arriba intercambiando punteros
int limpiarFilas(Tablero* t) {
    int eliminadas = 0;
    for (int i = t->alto - 1; i >= 0; i--) {
        // revisar si la fila esta llena (todos los bytes = 0xFF)
        bool llena = true;
        for (int j = 0; j < t->bytesXfila; j++) {
            if (t->filas[i][j] != 0xFF) { llena = false; break; }
        }
        if (!llena) continue;

        // guardar puntero de la fila y bajar todo con punteros
        uint8_t* temp = t->filas[i];
        for (int k = i; k > 0; k--)
            t->filas[k] = t->filas[k - 1];

        // la fila nueva arriba queda vacia
        t->filas[0] = temp;
        for (int j = 0; j < t->bytesXfila; j++)
            t->filas[0][j] = 0;

        eliminadas++;
        i++;
    }
    return eliminadas;
}

void imprimirTablero(Tablero* t, int py, int px, int tipo, int rot) {
    // limpiar consola
    cout << "\033[2J\033[H";

    for (int i = 0; i < t->alto; i++) {
        cout << "|";
        for (int j = 0; j < t->ancho; j++) {
            bool esPieza = false;
            int pf = i - py;
            int pc = j - px;
            if (tipo >= 0 && pf >= 0 && pf < TAM && pc >= 0 && pc < TAM)
                esPieza = getPiezaBit(tipo, rot, pf, pc);

            if (esPieza)
                cout << "[]";
            else if (getBit(t, i, j))
                cout << "##";
            else
                cout << " .";
        }
        cout << "|\n";
    }

    cout << "+";
    for (int j = 0; j < t->ancho; j++) cout << "--";
    cout << "+\n";
}

