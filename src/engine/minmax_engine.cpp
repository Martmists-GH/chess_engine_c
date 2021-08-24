//
// Created by mart on 8/20/21.
//

#include "minmax_engine.h"
#include "../util/random.h"


ChessMove MinMaxEngine::process(ChessGameState &state) {
    if (state.lastMove2.fromIndex == -1) {
        this->root.currentState.move(state.lastMove);
    } else {
        auto item = this->root.deallocAndGet(state.lastMove);
        // siblings cleared, we can override parent
        this->root = item;
    }

    for (int x = 0; x < k; x++) {
        this->root.doMCTS(depth);
    }

    ChessMove mv = this->root.getBestMove();
    this->root.currentState.move(mv);
    this->root.currentState.update();
    return mv;
}

MinMaxEngine::MinMaxEngine(int depth, int k) : depth(depth), k(k) {
    this->root.currentState.standard();
}
