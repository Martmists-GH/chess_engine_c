//
// Created by mart on 8/24/21.
//

#pragma once

typedef enum {
    KING = 0,
    QUEEN,
    BISHOP,
    KNIGHT,
    ROOK,
    PAWN,
    EMPTY,
    INVALID,
} PieceType;

class ChessPiece {
public:
    PieceType type : 3;
    bool moved : 1;
    bool black : 1;

    void dummy();
    void empty();
};


