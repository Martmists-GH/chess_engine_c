//
// Created by mart on 8/20/21.
//

#include <cstdio>
#include <fstream>
#include "minmax_engine.h"
#include "../util/random.h"
#include "gperftools/malloc_extension.h"

// initial board for setup
static ChessGameState board;

ChessMove MinMaxEngine::process(ChessGameState* state) {
//    printf("Process called\n");

    if (state->lastMove2.fromIndex == -1) {
        memcpy(&this->root->currentState, state, sizeof(ChessGameState));
        this->root->player2 = state->blackToMove;
    } else {
        auto item = this->root->getChild(state->lastMove);
        this->root->clearChildren(item);
        delete this->root;
        this->root = item;
        this->root->parent = nullptr;
    }

//    printf("Looping k times...\n");
    for (int x = 0; x < k; x++) {
        printf("%d/%d\n", x, k);
        this->root->doMCTS(depth);
    }

    ChessMove mv = this->root->getBestMove();
//    printf("Move: %d,%d\n", mv.fromIndex, mv.toIndex);

    // handle own move
    auto item = this->root->getChild(mv);
    this->root->clearChildren(item);
    delete this->root;
    this->root = item;
    this->root->parent = nullptr;

    return mv;
}

MinMaxEngine::MinMaxEngine(int depth, int k, int c) : depth(depth), k(k) {
    standard(&board);  // dummy
    this->root = new Node(&board);
    this->root->C = c;
}

MinMaxEngine::~MinMaxEngine() {
    delete this->root;
}
