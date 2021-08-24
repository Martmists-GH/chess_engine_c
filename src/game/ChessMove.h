//
// Created by mart on 8/24/21.
//

#pragma once


#include "../mcts/Move.h"
#include "ChessPiece.h"

class ChessMove : public Move {
public:
    char fromIndex;
    char toIndex;
    bool isCastle : 1;
    bool isEnPassant : 1;
    PieceType promotion : 3;

    ChessMove();
    ChessMove(int fromIndex, int toIndex);
    ChessMove(int fromIndex, int toIndex, bool isCastle, bool isEnPassant, PieceType promotion);

    void dummy() override;
    long hash() override;
};


