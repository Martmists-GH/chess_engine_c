//
// Created by mart on 8/17/21.
//

#pragma once
#include <string>

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

typedef struct {
    PieceType type : 3;
    bool moved : 1;
    bool black : 1;
} Piece;

static Piece DUMMY_PIECE = {
        PieceType::INVALID,
        false,
        false
};

static Piece EMPTY_PIECE = {
        PieceType::EMPTY,
        false,
        false
};
