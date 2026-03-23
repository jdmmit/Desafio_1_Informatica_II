# Universidad de Antioquia
## Facultad de Ingenieria
### Informatica II - Semestre 2026-1
### Desafio 1 - Informe de Desarrollo

# Implementacion de Tetris mediante manipulacion de bits en C++

| | |
|---|---|
| **Integrantes** | Juan David Murillo Mejia / Juan Camilo Medina Isaza |
| **Asignatura** | Informatica II |
| **Profesores** | Anibal Guerra / Augusto Salazar |
| **Fecha de entrega** | 20 de marzo de 2026 |
| **Semestre** | 2026-1 |

---

## 1. Analisis del problema

El desafio consiste en implementar una version funcional del juego Tetris que corra completamente por consola, usando C++ en el framework Qt. Lo que distingue este problema de una implementacion comun es que la representacion del tablero y el manejo de las piezas deben hacerse a nivel de bits, no con arreglos de enteros o booleanos.

Desde el inicio identificamos dos grandes bloques: la representacion del estado del juego y la logica que lo gobierna. Para la representacion, el tablero se modela como un arreglo de filas donde cada fila es una secuencia de bits. Dado que el ancho del tablero siempre es multiplo de 8, cada fila cabe exactamente en un arreglo de bytes (`uint8_t`), lo que ahorra memoria respecto a usar un int por celda. Para la logica, se necesita movimiento, rotacion, deteccion de colisiones, fijacion de piezas y limpieza de filas.

Las piezas tambien se representan con bits: cada pieza en cada rotacion se codifica como un `uint16_t` que describe una cuadricula de 4x4. Esto permite consultar que celdas ocupa la pieza con una sola operacion de desplazamiento, sin estructuras adicionales.

### 1.1 Restricciones identificadas

- El ancho del tablero debe ser multiplo de 8 para que la representacion por bytes sea exacta.
- No se pueden usar string, STL ni librerias no autorizadas.
- La solucion debe usar memoria dinamica (`new` / `delete`) de forma explicita.
- No se puede usar ANSI C: todo debe ser C++ estandar valido dentro del framework Qt.
- Cada pieza debe representarse y moverse usando operadores a nivel de bits.

### 1.2 Alternativa de solucion propuesta

La solucion se divide en tres modulos (tablero, pieza, juego) cada uno con su `.h` y `.cpp`, mas un `main.cpp`. Esta separacion permite que cada parte sea comprensible por separado y facilita la sustentacion.

El tablero se implementa como `uint8_t**` donde cada fila es un arreglo de bytes asignado dinamicamente. La lectura y escritura de celdas se hace con desplazamientos y mascaras. Las piezas se codifican como constantes `uint16_t` en una tabla de 7 tipos por 4 rotaciones. La logica de juego conecta ambas partes.

---

## 2. Esquema de tareas del desarrollo

Al analizar el problema dividimos el trabajo en las siguientes tareas, distribuidas entre los dos integrantes:

| Tarea | Descripcion |
|---|---|
| **T1 - Tablero** | Definir la estructura `Tablero` con `uint8_t**`, implementar `crearTablero`, `destruirTablero`, `getBit` y `setBit` con operadores de bits. |
| **T2 - Limpieza de filas** | Implementar `limpiarFilas`: detectar filas llenas (byte == `0xFF`), eliminarlas intercambiando punteros y reiniciar la fila superior vacia. |
| **T3 - Visualizacion** | Implementar `imprimirTablero` que superpone la pieza activa sobre el tablero fijo al momento de imprimir en consola. |
| **T4 - Piezas** | Definir las 7 piezas en sus 4 rotaciones como constantes `uint16_t`. Implementar `getPiezaBit` y `piezaRandom`. |
| **T5 - Logica de juego** | Implementar `hayColision`, `fijarPieza`, `nuevaPieza`, `moverIzq`, `moverDer`, `bajar` y `rotar`. |
| **T6 - Ciclo principal** | Implementar `correrJuego`: leer entrada del usuario con `cin`, ejecutar la accion, aplicar gravedad y detectar Game Over. |
| **T7 - Validacion** | Validar en `main.cpp` que el ancho sea multiplo de 8 y que ambas dimensiones sean minimo 8. |

### 2.1 Distribucion del trabajo

Juan David Murillo Mejia se encargo de los modulos `tablero.h/cpp` y `juego.h/cpp`, especialmente la logica de bits del tablero y las colisiones. Juan Camilo Medina Isaza se encargo de `pieza.h/cpp` (codificacion de las 7 piezas y sus rotaciones), la funcion de visualizacion y el ciclo principal en `correrJuego`.

---

## 3. Algoritmos implementados

### 3.1 Representacion del tablero con bits

El tablero es `uint8_t**` filas, con `bytesXfila = ancho / 8` bytes por fila. Para acceder a la celda `(fila, col)`:

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

Esta logica permite leer o escribir cualquier celda con operaciones de bits puras.

### 3.2 Representacion de las piezas

Cada pieza en cada rotacion es un `uint16_t` (cuadricula 4x4). El bit en posicion `(15 - (fila*4 + col))` indica si esa celda esta ocupada:

```cpp
(formas[tipo][rot] >> (15 - (f * 4 + c))) & 1
```

Esto permite almacenar toda la forma de una pieza en un solo entero de 16 bits y consultarla con un desplazamiento.

### 3.3 Deteccion de colisiones

El algoritmo recorre las 16 celdas de la cuadricula 4x4. Por cada celda activa verifica tres condiciones: que la columna resultante `(px+c)` este dentro del tablero, que la fila resultante `(py+f)` no supere el fondo, y que el bit correspondiente en el tablero no este en 1. Si alguna falla, hay colision. La condicion `tf >= 0` es importante para piezas recien generadas cuya parte superior todavia esta fuera del tablero.

### 3.4 Limpieza de filas con intercambio de punteros

Cuando todos los bytes de una fila son `0xFF`, esa fila esta llena. El algoritmo guarda el puntero de esa fila, desplaza hacia abajo todos los punteros superiores sin copiar datos, y reutiliza el puntero guardado como nueva fila vacia en la posicion 0. Esto es O(alto) en operaciones de puntero pero no mueve ningun byte de datos.

### 3.5 Gravedad por turno

En lugar de un timer, la gravedad se aplica al final de cada turno del usuario llamando a `bajar()`. Si la pieza no puede bajar, se fija automaticamente y se genera una nueva. Esto hace que el juego avance a un ritmo vinculado a las acciones del usuario, tal como indica el enunciado (flujo basado en turnos).

---

## 4. Problemas de desarrollo afrontados

### 4.1 Entender las operaciones de bits

El punto de confusion inicial fue como mapear una posicion `(fila, columna)` a un bit dentro de un byte. La pregunta era si el bit mas significativo corresponde a la columna izquierda o derecha. Decidimos que el bit 7 (mas significativo) es la columna izquierda dentro de cada grupo de 8, lo que resulta en `bitIdx = 7 - (col % 8)`. Hasta que eso no estuvo claro, las piezas se imprimian en posiciones incorrectas.

### 4.2 Manejo de memoria dinamica y punteros

La estructura `uint8_t**` implica dos niveles de memoria dinamica: el arreglo de punteros a filas, y cada fila individualmente. Al principio cometimos el error de no liberar en el orden correcto en `destruirTablero`. La solucion fue liberar primero cada fila con `delete[]` y luego el arreglo de punteros con `delete[]`.

Otro problema fue en `limpiarFilas`: al principio copiabamos los datos byte por byte, lo que era correcto pero innecesario. Luego entendimos que bastaba con intercambiar los punteros, que es lo que el enunciado indica con 'manipulacion de punteros'.

### 4.3 Codificacion de las rotaciones de las piezas

Codificar las 7 piezas con sus 4 rotaciones como constantes `uint16_t` fue el paso que mas tiempo tomo. Habia que visualizar mentalmente la cuadricula 4x4 y convertirla a hexadecimal. Nos equivocamos varias veces con la pieza T y la J. Lo solucionamos dibujando cada cuadricula en papel, anotando el valor de cada bit y convirtiendo manualmente a hex.

### 4.4 Colisiones con el borde superior

Un caso que no habiamos considerado era cuando `py` es negativo (la pieza acaba de aparecer y parte de su cuadricula 4x4 esta por encima del tablero). En ese caso `tf = py + f` puede ser negativo, y llamar `getBit` con un indice negativo daba comportamiento indefinido. Agregar la condicion `tf >= 0` antes de consultar el tablero resolvio el problema.

---

## 5. Evolucion de la solucion

El desarrollo siguio un proceso iterativo que partio de lo mas simple hacia lo completo:

1. **Primera iteracion:** Implementamos el tablero con `getBit` y `setBit` y lo imprimimos para verificar que los bits funcionaran correctamente antes de agregar nada mas.
2. **Segunda iteracion:** Codificamos las piezas como `uint16_t` y probamos `getPiezaBit` imprimiendo cada pieza de forma aislada para verificar que las formas fueran correctas.
3. **Tercera iteracion:** Conectamos piezas con tablero implementando `hayColision` y `fijarPieza`. Probamos fijando piezas en posiciones predeterminadas.
4. **Cuarta iteracion:** Agregamos el ciclo de juego con entrada del usuario, movimiento y gravedad por turno.
5. **Quinta iteracion:** Implementamos `limpiarFilas` con intercambio de punteros, el puntaje acumulado y la deteccion de Game Over.
6. **Sexta iteracion:** Revision final: verificacion de liberacion de memoria, pruebas con distintos tamanos de tablero y ajustes a la visualizacion por consola.

### 5.1 Consideraciones para la implementacion

- Siempre liberar memoria en `destruirTablero` y `destruirJuego` para evitar fugas.
- La condicion `tf >= 0` en `hayColision` es critica para piezas recien generadas.
- El intercambio de punteros en `limpiarFilas` es mas eficiente que copiar datos.
- Usar `cout` y `cin` de `iostream` en lugar de `QTextStream` para no depender de modulos Qt innecesarios.
- Las piezas se ubican en `px = ancho/2 - 2` para quedar centradas respecto al tablero.

---

## 6. Justificacion de tipos de datos y estructuras

| Tipo / Estructura | Por que se eligio |
|---|---|
| `uint8_t**` (tablero) | Cada byte almacena 8 columnas. Con ancho multiplo de 8, no hay desperdicio. Es 8 veces mas compacto que usar un int por celda. Tipo estandar de C++ de 8 bits sin signo, definido en `<cstdint>`. |
| `uint16_t` (piezas) | La cuadricula de una pieza es 4x4 = 16 bits. Un `uint16_t` los almacena exactamente y permite consultar cualquier celda con un solo desplazamiento. Tipo estandar de C++, no depende de Qt. |
| `uint8_t` (bytes por fila) | Tipo minimo sin signo de 8 bits. Natural para comparar con `0xFF` como mascara de fila completa. Garantizado por el estandar de C++ en `<cstdint>`. |
| `int` (posicion px, py) | Las posiciones pueden ser negativas cuando la pieza esta sobre el borde superior al aparecer, por eso se usa `int` con signo y no un tipo sin signo. |
| `iostream` (entrada/salida) | Se uso `cout` y `cin` de la libreria estandar de C++ en lugar de `QTextStream`, ya que el codigo no necesita funciones especificas de Qt para la entrada y salida por consola. |
| `struct` (sin clases) | El enunciado no permite STL. Las structs agrupan datos relacionados de forma simple sin usar OOP. |

---

## 7. Conclusion

El desarrollo de este desafio nos permitio aplicar de forma concreta conceptos que hasta ahora habian sido principalmente teoricos, especialmente las operaciones a nivel de bits y el manejo manual de memoria dinamica. La parte mas util fue obligarnos a pensar en como representar informacion de forma compacta, algo que normalmente no se considera cuando se usan estructuras de alto nivel.

El mayor aprendizaje fue entender que un bit es una unidad de informacion valida y manipulable directamente. Representar un tablero de juego como un arreglo de bytes y una pieza como un entero de 16 bits, y lograr que todo funcione correctamente, resulto ser un ejercicio muy efectivo para consolidar ese concepto.

Dividir la solucion en modulos tambien facilito el trabajo en equipo: cada integrante podia trabajar en su parte sin interferir con la del otro, y la integracion fue directa porque los contratos entre modulos (los archivos `.h`) estaban definidos desde el inicio.
