//
// Created by mart on 8/17/21.
//

#pragma once


#include "../game/board.h"

class Engine {
public:
    virtual Move process(Board* state) = 0;
};
