//
// Created by mart on 8/20/21.
//

#include "minmax_engine.h"
#include "../game/find_moves.h"
#include "board_evaluation.h"
#include <limits>
#include <cstring>
#include <cmath>

MinMaxEngine::MinMaxEngine(int depth) : depth(depth) {
    boards = new Board[depth];
}

MinMaxEngine::~MinMaxEngine() {
    delete boards;
}

float MinMaxEngine::alphabeta(Board* board, float alpha, float beta, int depth) {
    float eval = evaluate(board);
    if (depth == 0 || board->state != PLAYING) {
        return eval;
    }

    float value;
    std::vector<Move> moves;
    Board* ptr = &boards[depth-1];
    for (int i = 20; i < 100; i++) {
        auto p = board->pieces[i];
        if (p.type != INVALID && p.type != EMPTY && p.black != board->blackToMove) {
            movesSmart(board, i, &moves);
        }
    }

    if (!board->blackToMove) {
        value = -std::numeric_limits<float>::infinity();
        for (auto mv : moves) {
            memcpy(ptr, board, sizeof(Board));
            move(ptr, mv);
            value = fmaxf(value, alphabeta(ptr, alpha, beta, depth - 1));
            if (value >= beta) {
                break;
            }
            alpha = fmaxf(alpha, value);
        }
    } else {
        value = std::numeric_limits<float>::infinity();
        for (auto mv : moves) {
            memcpy(ptr, board, sizeof(Board));
            move(ptr, mv);
            value = fminf(value, alphabeta(ptr, alpha, beta, depth - 1));
            if (value <= alpha) {
                break;
            }
            beta = fminf(beta, value);
        }
    }

    return value;
}

Move MinMaxEngine::process(Board *state) {
    std::vector<Move> possibleMoves;
    Board test;
    float score;
    Move best = DUMMY_MOVE;

    if (state->blackToMove) {
        score = std::numeric_limits<float>::infinity();
    } else {
        score = -std::numeric_limits<float>::infinity();
    }

    for (int i = 20; i < 100; i++) {
        auto p = state->pieces[i];
        if (p.type != INVALID && p.type != EMPTY && p.black == state->blackToMove) {
            movesSmart(state, i, &possibleMoves);
        }
    }

    for (auto mv : possibleMoves) {
        memcpy(&test, state, sizeof(Board));
        move(&test, mv);
        float newScore = alphabeta(&test, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), depth);

        if ((state->blackToMove && newScore < score) || (!state->blackToMove && newScore > score)) {
            score = newScore;
            best = mv;
        }
    }

    return best;
}
