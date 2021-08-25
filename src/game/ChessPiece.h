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

typedef struct {
    PieceType type: 3;
    bool moved: 1;
    bool black: 1;
} ChessPiece;

static ChessPiece INVALID_PIECE = {
        INVALID, false, false
};

static ChessPiece EMPTY_PIECE = {
        EMPTY, false, false
};