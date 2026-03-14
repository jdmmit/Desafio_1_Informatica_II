Universidad de Antioquia

Facultad de Ingenieria

Informatica II - Semestre 2026-1

Desafio I - Informe Preliminar

Implementacion de Tetris con manipulacion de bits en C++

Autor: Juan David Murillo Mejia

Codigo: Informática II Teoría G1 G2 G3 G4 G5( 2026-1) Teoría

Fecha: 13 de marzo de 2026

Profesores: Augusto Salazar



1. Analisis del problema

El objetivo del desafio es hacer una version de Tetris que corra por consola. Lo que lo hace distinto a un Tetris normal es que internamente todo el tablero y las piezas se manejan con bits, no con matrices de enteros. Cada celda del tablero no se guarda como un int de 4 bytes sino como un bit dentro de un byte, lo que hace que el uso de memoria sea mucho mas eficiente.

Al leer el enunciado lo primero que me llamo la atencion fue la restriccion del ancho multiplo de 8. Eso tiene sentido porque si el ancho es, por ejemplo, 16, cada fila del tablero cabe en exactamente 2 bytes (16 bits = 2 x uint8_t). Eso simplifica mucho el manejo porque se sabe exactamente cuantos bytes ocupa cada fila.

Las restricciones mas importantes que identifique son:
-	No se puede usar string, struct, STL ni librerias no autorizadas.
-	Hay que usar punteros, arreglos y memoria dinamica (new / delete).
-	Las operaciones de colision, movimiento y limpieza de filas deben hacerse con bitwise: <<, >>, &, |.
-	El tablero tiene ancho multiplo de 8, minimo 8x8, y el usuario lo configura al inicio.
-	Las piezas aparecen en el centro del tablero y se generan aleatoriamente.

2. Como se representa todo con bits

El tablero
Cada fila del tablero es un arreglo de uint8_t de tamano (ancho / 8). Un bit en 1 significa celda ocupada, un 0 significa celda libre. Se usa uint8_t porque garantiza exactamente 8 bits sin signo en cualquier compilador, lo que hace que los desplazamientos y mascaras funcionen de forma predecible.

El tablero completo es un uint8_t** (puntero a punteros). Cada elemento apunta a una fila asignada con new[]. Esto permite que cuando se elimina una fila llena, solo se mueven los punteros sin copiar datos, lo cual es eficiente.

Las piezas
Cada pieza se representa como un arreglo de 4 bytes (uint8_t[4]), donde cada byte es una fila del bounding box de 4x4 de la pieza. El bit mas significativo de cada byte corresponde a la columna mas a la izquierda.

Por ejemplo, la pieza T en su rotacion inicial se ve asi:
  forma[0] = 0b11100000   ->   X X X .
  forma[1] = 0b01000000   ->   . X . .
  forma[2] = 0b00000000   ->   . . . .
  forma[3] = 0b00000000   ->   . . . .

Las 4 rotaciones de cada una de las 7 piezas se guardan en una tabla constante global uint8_t PIEZAS[7][4][4]. Esta tabla no cambia durante la ejecucion, asi que no necesita ser dinamica. La pieza activa copia la rotacion actual en su propio arreglo para poder modificarla.

3. Operaciones principales con bits

Colision: se hace un AND entre los bits de la pieza (colocada en su posicion del tablero) y los bytes de la fila correspondiente. Si el resultado es distinto de cero, hay colision. Tambien se verifica que la pieza no salga de los bordes del tablero.

Fijar pieza: cuando la pieza no puede bajar mas, se hace un OR entre sus bits y los bytes del tablero. Eso marca las celdas como ocupadas.

Mover izquierda/derecha: desplazar los bytes de la pieza con << o >> segun la direccion. Hay que verificar que no se cruce el borde antes de aplicar el movimiento.

Limpiar filas: una fila esta completa si todos sus bytes son 0xFF (AND con 0xFF da 0xFF). Al eliminarla, los punteros de las filas superiores se desplazan hacia abajo y se recicla el puntero de la fila eliminada asignandole ceros.

Game Over: antes de colocar una pieza nueva se verifica si colisiona en la posicion inicial (centro superior). Si hay colision, el juego termina.

4. Estructura del proyecto

El proyecto se divide en varios archivos .h y .cpp para cumplir con la programacion modular:

-	board.h / board.cpp: todo lo relacionado con el tablero (creacion, liberacion, limpieza de filas, verificacion).
-	piece.h / piece.cpp: definicion de las piezas, generacion aleatoria y rotacion.
-	game.h / game.cpp: logica del juego, turno a turno, gravedad, Game Over.
-	display.h / display.cpp: impresion del tablero en consola con '#' y '.'.
-	input.h / input.cpp: lectura de la accion del usuario (A, D, S, W, Q).
-	main.cpp: punto de entrada, pide las dimensiones al usuario e inicia el juego.

5. Eleccion de variables y tipos

La eleccion de tipos de datos fue pensada para usar la menor cantidad de memoria posible y para que los operadores bitwise funcionen sin comportamientos inesperados:

-	uint8_t para filas del tablero y filas de piezas: 8 bits exactos, sin signo. Evita problemas con el bit de signo en los desplazamientos.
-	uint8_t** para el tablero: puntero a punteros permite mover filas sin copiar datos.
-	int para dimensiones y posicion de la pieza: la posicion puede ser negativa cuando la pieza esta entrando por arriba, por eso no se puede usar uint.
-	int para tipo y rotacion de pieza: indices simples de 0 a 6 y 0 a 3. int es mas que suficiente.

6. Plan de trabajo

Las tareas para la semana que viene (entrega 20 de marzo) son:

-	Crear el proyecto en Qt y configurar los archivos de cabecera.
-	Implementar el modulo del tablero con memoria dinamica y representacion en bits.
-	Implementar las piezas y la tabla de rotaciones.
-	Implementar colisiones, movimiento y fijacion.
-	Implementar la limpieza de filas y el desplazamiento de punteros.
-	Implementar la visualizacion en consola y la entrada del usuario.
-	Integrar todo, probar y grabar el video.

7. Conclusion

Despues de analizar el problema queda claro que lo mas importante es entender bien como se alinean los bits de la pieza sobre los bytes del tablero, especialmente cuando la pieza cae entre dos bytes de una misma fila. Eso es lo mas delicado de implementar y donde hay que tener mas cuidado con los desplazamientos.

La arquitectura modular que defini hace que sea mas facil depurar cada parte por separado. La proxima fase es pasar del diseno al codigo real en Qt C++.

