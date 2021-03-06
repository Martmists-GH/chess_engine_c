//
// Created by mart on 8/20/21.
//

#pragma once

#include <vector>
#include "engine.h"
#include "../mcts/Node.h"

class MinMaxEngine : public Engine {
private:
    int depth, k;
    Node* root;

public:
    MinMaxEngine(int depth, int k, int c);
    ~MinMaxEngine();

    ChessMove process(ChessGameState* state) override;
};