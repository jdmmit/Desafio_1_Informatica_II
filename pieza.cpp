#include "pieza.h"
#include <cstdlib>
#include <ctime>

// cada quint16 representa una cuadricula 4x4
// bit 15 = fila0 col0, bit 14 = fila0 col1 ... bit 0 = fila3 col3
uint16_t formas[NUM_PIEZAS][NUM_ROTS] = {
    // I
    { 0x0F00, 0x2222, 0x0F00, 0x2222 },
    // O
    { 0x6600, 0x6600, 0x6600, 0x6600 },
    // T
    { 0x0E40, 0x4C40, 0x4E00, 0x4640 },
    // S
    { 0x06C0, 0x4C80, 0x06C0, 0x4C80 },
    // Z
    { 0x0C60, 0x2640, 0x0C60, 0x2640 },
    // J
    { 0x44C0, 0x8E00, 0x6440, 0x0E20 },
    // L
    { 0x4460, 0x0E80, 0xC440, 0x2E00 }
};

bool getPiezaBit(int tipo, int rot, int f, int c) {
    if (f < 0 || f >= TAM || c < 0 || c >= TAM) return false;
    int pos = f * TAM + c;
    return (formas[tipo][rot] >> (15 - pos)) & 1;
}

int piezaRandom() {
    return rand() % NUM_PIEZAS;
}
