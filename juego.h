#ifndef JUEGO_H
#define JUEGO_H

#include "tablero.h"
#include "pieza.h"

struct Juego {
    Tablero* tablero;
    int tipo;       // tipo de pieza actual
    int rot;        // rotacion actual
    int px, py;     // posicion de la pieza
    int puntaje;
    bool gameOver;
};

Juego*  crearJuego(int ancho, int alto);
void    destruirJuego(Juego* j);
bool    hayColision(Juego* j, int tipo, int rot, int py, int px);
void    fijarPieza(Juego* j);
void    nuevaPieza(Juego* j);
void    moverIzq(Juego* j);
void    moverDer(Juego* j);
void    bajar(Juego* j);
void    rotar(Juego* j);
void    correrJuego(int ancho, int alto);

#endif
