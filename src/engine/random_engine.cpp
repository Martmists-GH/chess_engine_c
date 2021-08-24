//
// Created by mart on 8/20/21.
//

#include "random_engine.h"
#include "../util/random.h"


std::vector<ChessMove> moves (20);
ChessMove RandomEngine::process(ChessGameState &state) {
    moves.clear();
    state.getPossibleMoves(moves);
    return randomItem(moves);
}
