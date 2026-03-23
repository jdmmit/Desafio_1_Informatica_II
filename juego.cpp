#include "juego.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

Juego* crearJuego(int ancho, int alto) {
    srand(time(0));
    Juego* j  = new Juego;
    j->tablero = crearTablero(ancho, alto);
    j->puntaje = 0;
    j->gameOver = false;
    nuevaPieza(j);
    return j;
}

void destruirJuego(Juego* j) {
    destruirTablero(j->tablero);
    delete j;
}

bool hayColision(Juego* j, int tipo, int rot, int py, int px) {
    for (int f = 0; f < TAM; f++) {
        for (int c = 0; c < TAM; c++) {
            if (!getPiezaBit(tipo, rot, f, c)) continue;
            int tf = py + f;
            int tc = px + c;
            if (tc < 0 || tc >= j->tablero->ancho) return true;
            if (tf >= j->tablero->alto)             return true;
            if (tf >= 0 && getBit(j->tablero, tf, tc)) return true;
        }
    }
    return false;
}

void nuevaPieza(Juego* j) {
    j->tipo = piezaRandom();
    j->rot  = 0;
    j->px   = j->tablero->ancho / 2 - 2;
    j->py   = 0;
    if (hayColision(j, j->tipo, j->rot, j->py, j->px))
        j->gameOver = true;
}

void fijarPieza(Juego* j) {
    // poner los bits de la pieza en el tablero con OR
    for (int f = 0; f < TAM; f++) {
        for (int c = 0; c < TAM; c++) {
            if (!getPiezaBit(j->tipo, j->rot, f, c)) continue;
            setBit(j->tablero, j->py + f, j->px + c, true);
        }
    }
    int filas = limpiarFilas(j->tablero);
    if      (filas == 1) j->puntaje += 100;
    else if (filas == 2) j->puntaje += 300;
    else if (filas == 3) j->puntaje += 500;
    else if (filas == 4) j->puntaje += 800;
    nuevaPieza(j);
}

void moverIzq(Juego* j) {
    if (!hayColision(j, j->tipo, j->rot, j->py, j->px - 1))
        j->px--;
}

void moverDer(Juego* j) {
    if (!hayColision(j, j->tipo, j->rot, j->py, j->px + 1))
        j->px++;
}

void bajar(Juego* j) {
    if (!hayColision(j, j->tipo, j->rot, j->py + 1, j->px))
        j->py++;
    else
        fijarPieza(j);
}

void rotar(Juego* j) {
    int nuevaRot = (j->rot + 1) % NUM_ROTS;
    if (!hayColision(j, j->tipo, nuevaRot, j->py, j->px))
        j->rot = nuevaRot;
}

void correrJuego(int ancho, int alto) {
    Juego* j = crearJuego(ancho, alto);

    while (!j->gameOver) {
        imprimirTablero(j->tablero, j->py, j->px, j->tipo, j->rot);
        cout << "Puntaje: " << j->puntaje << "\n";
        cout << "Accion: [A]Izq [D]Der [S]Bajar [W]Rotar [Q]Salir: ";

        char accion;
        cin >> accion;

        if      (accion == 'a' || accion == 'A') moverIzq(j);
        else if (accion == 'd' || accion == 'D') moverDer(j);
        else if (accion == 's' || accion == 'S') bajar(j);
        else if (accion == 'w' || accion == 'W') rotar(j);
        else if (accion == 'q' || accion == 'Q') break;

        // gravedad: la pieza baja un lugar por turno
        bajar(j);
    }

    if (j->gameOver) {
        imprimirTablero(j->tablero, -1, -1, -1, 0);
        cout << "\n*** GAME OVER ***  Puntaje final: " << j->puntaje << "\n";
    }

    destruirJuego(j);
}