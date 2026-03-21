#include <iostream>
#include "juego.h"

using namespace std;

int main(){
    int ancho, alto;

    cout << "Ancho del tablero (minimo 8, multiplo de 8): ";
    cin >> ancho;
    while (ancho < 8 || ancho % 8 != 0) {
        cout << "Invalido. Intente de nuevo: ";
        cin >> ancho;
    }

    cout << "Alto del tablero (minimo 8): ";
    cin >> alto;
    while (alto < 8) {
        cout << "Invalido. Intente de nuevo: ";
        cin >> alto;
    }

    correrJuego(ancho, alto);
    return 0;
}