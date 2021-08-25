//
// Created by mart on 8/20/21.
//

#include "random_engine.h"
#include "../util/random.h"


static std::vector<ChessMove> moves (20);
ChessMove RandomEngine::process(ChessGameState* state) {
    moves.clear();
    getPossibleMoves(state, moves);
    return randomItem(moves);
}
