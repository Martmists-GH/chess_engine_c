//
// Created by mart on 8/24/21.
//

#pragma once

#include <vector>
#include "../util/constraints.h"
#include "Move.h"

#define MoveT Derived<Move>

template <MoveT T>
class GameState {
protected:
    int currentPlayer = 0;
    int maxPlayers = 2;

public:

    /**
     * Store all possible moves in a vector
     */
    virtual void getPossibleMoves(std::vector<T>& vector) const = 0;

    /**
     * Get max number of players
     */
    int getMaxPlayers() const {
        return maxPlayers;
    }

    /**
     * Get index of current player;
     */
    int getCurrentPlayer() const {
        return currentPlayer;
    }

    virtual void update() = 0;

    /**
     * -1 for still playing, max players for draw.
     */
    virtual int getWinner() const = 0;

    /**
     * Player with the advantage, max players for equal.
     */
    virtual int getWinningPlayer() const = 0;

    /**
     * Apply move `mv` to the current game state
     */
    virtual void move(T &mv) = 0;

    /**
     * Get a hash to see if two states are equal
     */
    virtual long hash() const = 0;
};
