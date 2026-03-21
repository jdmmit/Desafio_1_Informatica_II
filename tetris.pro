QT -= gui
QT += core

CONFIG += console
CONFIG -= app_bundle

TARGET   = tetris
TEMPLATE = app

SOURCES += main.cpp tablero.cpp pieza.cpp juego.cpp \
    juego.cpp \
    main.cpp \
    pieza.cpp \
    tablero.cpp
HEADERS += tablero.h pieza.h juego.h \
    juego.h \
    pieza.h \
    tablero.h
