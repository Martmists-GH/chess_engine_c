//
// Created by mart on 8/24/21.
//

#pragma once


#include "../mcts/GameState.h"
#include "ChessMove.h"
#include "ChessPiece.h"

typedef enum {
    PLAYING,
    CHECKMATE_WHITE,
    CHECKMATE_BLACK,
    STALEMATE,
    DRAW
} Status;

class ChessGameState : public GameState<ChessMove> {
public:
    ChessPiece pieces[120];
    bool blackToMove : 1 = false;
    unsigned char repeatedMoves : 4 = 0;
    Status status : 3 = PLAYING;

    ChessMove lastMove;
    ChessMove lastMove2;

    ChessGameState();
    void standard();
    void s960();

    void rotate();

    int getWinner() const override;
    int getWinningPlayer() const override;
    void getPossibleMovesSimple(std::vector<ChessMove>& vector, int index) const;
    void getPossibleMoves(std::vector<ChessMove>& vector, int index) const;
    void getPossibleMoves(std::vector<ChessMove>& vector) const override;
    void move(ChessMove &mv) override;
    void update() override;
    long hash() const override;
};
