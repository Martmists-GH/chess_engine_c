//
// Created by mart on 8/24/21.
//

#pragma once

#include "ChessMove.h"
#include "ChessPiece.h"
#include <vector>

typedef enum {
    PLAYING,
    CHECKMATE_WHITE,
    CHECKMATE_BLACK,
    STALEMATE,
    DRAW
} Status;

typedef struct {
    ChessPiece pieces[120] { };
    unsigned char repeatedMoves : 4 = 0;
    Status status : 3 = PLAYING;
    bool blackToMove : 1 = false;
    ChessMove lastMove = DUMMY_MOVE;
    ChessMove lastMove2 = DUMMY_MOVE;
} ChessGameState;

void standard(ChessGameState* this_);
void s960(ChessGameState* this_);

void rotate(ChessGameState* this_);

int getWinner(ChessGameState* this_);
int getWinningPlayer(ChessGameState* this_);
void getPossibleMovesSimple(ChessGameState* this_, std::vector<ChessMove>& vector, int index);
void getPossibleMoves(ChessGameState* this_, std::vector<ChessMove>& vector, int index);
void getPossibleMoves(ChessGameState* this_, std::vector<ChessMove>& vector);
void move(ChessGameState* this_, ChessMove mv);
void update(ChessGameState* this_);
