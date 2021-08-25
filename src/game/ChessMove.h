//
// Created by mart on 8/24/21.
//

#pragma once

#include "ChessPiece.h"


typedef struct {
    char fromIndex;
    char toIndex;
    bool isCastle: 1;
    bool isEnPassant: 1;
    PieceType promotion: 3;
} ChessMove;

static ChessMove DUMMY_MOVE = {
    -1, -1, false, false, INVALID
};
