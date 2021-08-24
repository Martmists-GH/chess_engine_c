//
// Created by mart on 8/24/21.
//

#include "ChessPiece.h"

void ChessPiece::dummy() {
    type = INVALID;
    black = false;
    moved = false;
}

void ChessPiece::empty() {
    type = EMPTY;
    black = false;
    moved = false;
}
