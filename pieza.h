#ifndef PIEZA_H
#define PIEZA_H

#include <cstdint>

#define NUM_PIEZAS 7
#define NUM_ROTS   4
#define TAM        4

// cada pieza en cada rotacion es un uint16_t (cuadricula 4x4 en bits)

extern uint16_t formas[NUM_PIEZAS][NUM_ROTS];

bool getPiezaBit(int tipo, int rot, int f, int c);
int  piezaRandom();

#endif
