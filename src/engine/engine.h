//
// Created by mart on 8/17/21.
//

#pragma once


#include "../game/ChessGameState.h"

class Engine {
public:
    virtual ChessMove process(ChessGameState& state) = 0;
};
