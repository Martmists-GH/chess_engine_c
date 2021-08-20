//
// Created by mart on 8/20/21.
//

#include "random_engine.h"
#include "../game/find_moves.h"
#include "../util/random.h"

Move RandomEngine::process(Board *state) {
    std::vector<Move> moves;
    for (int i = 20; i < 100; i++) {
        auto p = state->pieces[i];
        if (p.black == state->blackToMove && p.type != PieceType::EMPTY && p.type != PieceType::INVALID) {
            auto newMoves = movesSmart(state, i);
            moves.insert(moves.end(), newMoves->begin(),  newMoves->end());
            delete newMoves;
        }
    }

    shuffleVector(&moves);
    return moves.at(0);
}
