//
// Created by mart on 8/20/21.
//

#pragma once

#include "engine.h"

class MinMaxEngine : public Engine {
private:
    int depth;
    Board* boards;
    float alphabeta(Board *board, float alpha, float beta, int depth);

public:
    MinMaxEngine(int depth);
    ~MinMaxEngine();

    Move process(Board* state) override;
};