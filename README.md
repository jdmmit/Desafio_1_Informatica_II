# Universidad de Antioquia
## Facultad de Ingenieria
### Informatica II - Semestre 2026-1
### Desafio I - Informe Final

# Implementacion de Tetris con manipulacion de bits en C++

| | |
|---|---|
| **Autores** | Juan David Murillo Mejia |
| **Codigo** | Informatica II Teoria G1 G2 G3 G4 G5 (2026-1) |
| **Fecha** | 20 de marzo de 2026 |
| **Profesores** | Augusto Salazar / Anibal Guerra |

---

## Video de presentacion

###[Ver video en YouTube](https://youtu.be/SDVodrXll5s)

## 1. Analisis del problema

El objetivo del desafio es hacer una version de Tetris que corra por consola. Lo que lo hace distinto a un Tetris normal es que internamente el tablero y las piezas se manejan con bits, no con matrices de enteros. Cada celda del tablero no se guarda como un int de 4 bytes sino como un bit dentro de un byte, lo que hace que el uso de memoria sea mucho mas eficiente.

La restriccion del ancho multiplo de 8 tiene sentido porque si el ancho es por ejemplo 16, cada fila del tablero cabe en exactamente 2 bytes. Eso simplifica el manejo porque se sabe exactamente cuantos bytes ocupa cada fila y los operadores bitwise funcionan de forma predecible.

### Restricciones identificadas

- No se pueden usar string, STL ni librerias no autorizadas.
- Hay que usar punteros, arreglos y memoria dinamica (new / delete).
- Las operaciones de colision, movimiento y limpieza de filas deben hacerse con bitwise: `<<`, `>>`, `&`, `|`.
- El tablero tiene ancho multiplo de 8, minimo 8x8, y el usuario lo configura al inicio.
- Las piezas aparecen en el centro del tablero y se generan aleatoriamente.
- No se puede usar ANSI C: todo debe ser C++ estandar valido dentro del framework Qt.

---

## 2. Como se representa todo con bits

### El tablero

Cada fila del tablero es un arreglo de `uint8_t` de tamano `(ancho / 8)`. Un bit en 1 significa celda ocupada, un 0 significa celda libre. Se usa `uint8_t` porque garantiza exactamente 8 bits sin signo en cualquier compilador, definido en `<cstdint>`, lo que hace que los desplazamientos y mascaras funcionen de forma predecible.

El tablero completo es un `uint8_t**` (puntero a punteros). Cada elemento apunta a una fila asignada con `new[]`. Esto permite que cuando se elimina una fila llena, solo se intercambian los punteros sin copiar datos.

Para leer o escribir una celda `(fila, col)`:

```cpp
byteIdx = col / 8;
bitIdx  = 7 - (col % 8);   // bit mas significativo = columna izquierda

// leer:
(filas[fila][byteIdx] >> bitIdx) & 1

// poner en 1:
filas[fila][byteIdx] |=  (1 << bitIdx);

// poner en 0:
filas[fila][byteIdx] &= ~(1 << bitIdx);
```

### Las piezas

Cada pieza en cada rotacion se representa como un `uint16_t`, un entero de 16 bits que describe la forma en una cuadricula de 4x4. El bit 15 corresponde a la celda fila 0 columna 0, el bit 14 a fila 0 columna 1, y asi hasta el bit 0 que es fila 3 columna 3.

Para consultar si una celda `(f, c)` de la pieza esta ocupada:

```cpp
(formas[tipo][rot] >> (15 - (f * 4 + c))) & 1
```

Las 4 rotaciones de cada uno de los 7 tetriminos se guardan en una tabla constante global `uint16_t formas[7][4]`. Se eligio `uint16_t` porque 4x4 = 16 bits, el tipo encaja exactamente sin desperdiciar nada.

A diferencia del diseno preliminar donde las piezas se representaban como `uint8_t[4]`, en la implementacion final se uso un unico `uint16_t` por rotacion. Esto simplifica `getPiezaBit` a una sola operacion de desplazamiento.

---

## 3. Operaciones principales con bits

### Colision

La funcion `hayColision` recorre las 16 celdas de la cuadricula 4x4 de la pieza. Por cada celda activa calcula su posicion en el tablero (`tf = py + f`, `tc = px + c`) y verifica tres condiciones: que `tc` este dentro del ancho, que `tf` no supere el fondo, y que el bit en el tablero no este en 1. La condicion `tf >= 0` es critica porque cuando la pieza aparece, parte de su cuadricula puede estar por encima del tablero con indice negativo.

### Fijar pieza

Cuando la pieza no puede bajar mas, se recorre su cuadricula 4x4 y por cada celda activa se llama a `setBit` sobre el tablero, que aplica OR para marcar esa celda como ocupada.

### Limpiar filas

Una fila esta completa si todos sus bytes son `0xFF`. Al eliminarla, se guarda su puntero, se desplazan hacia abajo todos los punteros superiores uno a uno sin copiar datos, y se reutiliza el puntero guardado como nueva fila vacia en la posicion 0. Esto es O(alto) en operaciones de puntero pero no mueve ningun byte de datos.

### Game Over

Antes de colocar una pieza nueva se verifica si colisiona en la posicion inicial (`px = ancho/2 - 2`, `py = 0`). Si hay colision inmediata, el juego termina.

---

## 4. Estructura del proyecto

| Modulo | Responsabilidad |
|---|---|
| `tablero.h / tablero.cpp` | Estructura `Tablero` con `uint8_t**`, `crearTablero`, `destruirTablero`, `getBit`, `setBit`, `limpiarFilas`, `imprimirTablero`. |
| `pieza.h / pieza.cpp` | Tabla de formas `uint16_t[7][4]`, `getPiezaBit`, `piezaRandom`. |
| `juego.h / juego.cpp` | Estructura `Juego`, `hayColision`, `fijarPieza`, `nuevaPieza`, `moverIzq`, `moverDer`, `bajar`, `rotar`, `correrJuego`. |
| `main.cpp` | Punto de entrada. Pide dimensiones al usuario con validacion y llama a `correrJuego`. |

La entrada y salida se maneja con `cout` y `cin` de `iostream` estandar de C++, en lugar de `QTextStream` de Qt, porque el programa no necesita funcionalidad especifica de Qt para la consola.

---

## 5. Eleccion de variables y tipos

| Tipo | Por que se eligio |
|---|---|
| `uint8_t**` (tablero) | Puntero doble a bytes exactos de 8 bits. Permite intercambiar punteros de filas sin copiar datos al limpiar filas llenas. Definido en `<cstdint>`, estandar C++. |
| `uint16_t` (piezas) | 16 bits encajan exactamente en la cuadricula 4x4 de cada pieza. Una sola operacion de desplazamiento consulta cualquier celda. |
| `uint8_t` (bytes por fila) | 8 bits sin signo. Natural para comparar con `0xFF` como mascara de fila completa. Evita problemas con el bit de signo en desplazamientos. |
| `int` (posicion px, py) | La posicion puede ser negativa cuando la pieza esta entrando por la parte superior, por eso no se puede usar un tipo sin signo. |
| `int` (tipo, rotacion) | Indices simples de 0 a 6 y 0 a 3. `int` es suficiente y evita conversiones innecesarias. |

---

## 6. Cambios respecto al informe preliminar

- **Representacion de piezas:** pasaron de `uint8_t[4]` (4 bytes por rotacion) a `uint16_t` (1 valor de 16 bits por rotacion). Esto simplifica `getPiezaBit` a una sola operacion de desplazamiento.
- **Estructura de modulos:** `display` e `input` no son modulos separados. `imprimirTablero` quedo en `tablero.cpp` y la lectura del usuario quedo directamente en `correrJuego` dentro de `juego.cpp`.
- **Entrada/salida:** se uso `iostream` (`cout` / `cin`) en lugar de `QTextStream` para no depender de modulos Qt innecesarios.
- **Movimiento lateral:** se mueve la posicion `px` de la pieza en lugar de desplazar sus bits, lo que evita problemas de alineacion entre bytes.

---

## 7. Conclusion

Lo mas importante del desafio fue entender bien como se alinean los bits de la pieza sobre los bytes del tablero, y como manejar correctamente los indices cuando la pieza aparece parcialmente fuera del tablero. La arquitectura modular permitio desarrollar y probar cada parte por separado antes de integrarlas.

Las decisiones de tipos de datos, especialmente `uint8_t**` para el tablero y `uint16_t` para las piezas, resultaron ser las mas adecuadas tanto para la eficiencia en memoria como para la claridad del codigo. El intercambio de punteros en `limpiarFilas` fue la operacion mas interesante de implementar porque aplica directamente el concepto de manipulacion de punteros que plantea el enunciado.
