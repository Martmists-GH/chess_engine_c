//
// Created by mart on 8/17/21.
//

#pragma once

#include "move.h"

typedef enum {
    PLAYING,
    CHECKMATE_WHITE,
    CHECKMATE_BLACK,
    STALEMATE,
    DRAW
} GameState;

typedef struct {
    Piece pieces[120];
    bool blackToMove : 1;
    unsigned char repeatedMoves : 4;
    GameState state : 3;

    Move lastMove;
    Move lastMove2;
} Board;

Board* copy(Board* source);
void rotate(Board* board);
int rotateIndex(int index);

void updateState(Board* board);
void move(Board* board, Move move);
Board* standard();
Board* s960();
