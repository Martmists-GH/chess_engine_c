//
// Created by mart on 8/24/21.
//

#pragma once


class Move {
public:
    /**
     * Get a hash to verify two moves are equal
     */
    virtual long hash() = 0;

    /**
     * Dummy move
     */
    virtual void dummy() = 0;
};


