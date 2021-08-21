//
// Created by mart on 8/20/21.
//

#pragma once

#include <vector>
#include "engine.h"

class MinMaxEngine : public Engine {
private:
    int depth, k;
    Board* boards;
    std::vector<Move>* moves;
    void monteCarlo(Board *board, int* won, int* total, int depth);
    float alphabeta(Board *board, float alpha, float beta, int depth);

public:
    MinMaxEngine(int depth, int k);
    ~MinMaxEngine();

    Move process(Board* state) override;
};