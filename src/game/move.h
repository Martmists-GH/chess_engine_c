//
// Created by mart on 8/17/21.
//

#pragma once

#include "piece.h"

typedef struct {
    char fromIndex;
    char toIndex;
    bool isCastle : 1;
    bool isEnPassant : 1;
    PieceType promotion : 3;
} Move;

static Move DUMMY_MOVE = {
        -1, -1, false, false, PieceType::INVALID
};
