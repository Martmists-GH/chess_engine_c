//
// Created by mart on 8/20/21.
//

#include "minmax_engine.h"
#include "../game/find_moves.h"
#include "board_evaluation.h"
#include "../util/random.h"
#include <limits>
#include <cstring>
#include <cmath>

MinMaxEngine::MinMaxEngine(int depth, int k) : depth(depth), k(k) {
    boards = new Board[depth];
    moves = new std::vector<Move>[depth];
    for (int i = 0; i < depth; i++) {
        moves[i].reserve(120);
    }
}

MinMaxEngine::~MinMaxEngine() {
    delete boards;
}

void MinMaxEngine::monteCarlo(Board* board, int* won, int* total, int d) {
    float eval = evaluate(board);
    if (d == 0 || board->state != PLAYING) {
        *total += 1;
        if ((board->blackToMove && board->state == CHECKMATE_BLACK) || (!board->blackToMove && board->state == CHECKMATE_WHITE)) {
            *won += 1;
        } else if ((board->blackToMove && eval < -1) || (!board->blackToMove && eval > 1)) {
            *won += 1;
        }
        return;
    }

    auto mvs = &moves[d-1];
    mvs->clear();

    Board* ptr = &boards[d-1];
    for (int i = 20; i < 100; i++) {
        auto p = board->pieces[i];
        if (p.type != INVALID && p.type != EMPTY && p.black != board->blackToMove) {
            movesSmart(board, i, mvs);
        }
    }

    shuffleVector(mvs);
    int i = 0;
    for (auto mv : *mvs) {
        memcpy(ptr, board, sizeof(Board));
        move(ptr, mv);
        monteCarlo(ptr, won, total, d-1);

        i++;
        if (i == k) {
            break;
        }
    }
}

static Board compare;
static Board compareX;
static Board compareY;

bool compareMovesGreater(Move x, Move y) {
    memcpy(&compareX, &compare, sizeof(Board));
    memcpy(&compareY, &compare, sizeof(Board));
    move(&compareX, x);
    move(&compareY, y);
    return evaluate(&compareX) > evaluate(&compareY);
}

bool compareMovesSmaller(Move x, Move y) {
    memcpy(&compareX, &compare, sizeof(Board));
    memcpy(&compareY, &compare, sizeof(Board));
    move(&compareX, x);
    move(&compareY, y);
    return evaluate(&compareX) < evaluate(&compareY);
}

float MinMaxEngine::alphabeta(Board* board, float alpha, float beta, int d) {
    float eval = evaluate(board);
    if (d == 0 || board->state != PLAYING) {
        return eval;
    }

    float value;
    auto mvs = &moves[d-1];
    mvs->clear();

    Board* ptr = &boards[d-1];
    for (int i = 20; i < 100; i++) {
        auto p = board->pieces[i];
        if (p.type != INVALID && p.type != EMPTY && p.black != board->blackToMove) {
            movesSmart(board, i, mvs);
        }
    }

    memcpy(&compare, board, sizeof(Board));
    if (!board->blackToMove) {
        std::sort(mvs->begin(), mvs->end(), compareMovesGreater);
        value = -std::numeric_limits<float>::infinity();
        for (auto mv : *mvs) {
            memcpy(ptr, board, sizeof(Board));
            move(ptr, mv);
            value = fmaxf(value, alphabeta(ptr, alpha, beta, d - 1));
            if (value >= beta) {
                break;
            }
            alpha = fmaxf(alpha, value);
        }
    } else {
        std::sort(mvs->begin(), mvs->end(), compareMovesSmaller);
        value = std::numeric_limits<float>::infinity();
        for (auto mv : *mvs) {
            memcpy(ptr, board, sizeof(Board));
            move(ptr, mv);
            value = fminf(value, alphabeta(ptr, alpha, beta, d - 1));
            if (value <= alpha) {
                break;
            }
            beta = fminf(beta, value);
        }
    }

    return value;
}

static std::vector<Move> possibleMoves(120);
//Move MinMaxEngine::process(Board *state) {
//    possibleMoves.clear();
//
//    Board test;
//    float score;
//    Move best = DUMMY_MOVE;
//
//    for (int i = 20; i < 100; i++) {
//        auto p = state->pieces[i];
//        if (p.type != INVALID && p.type != EMPTY && p.black == state->blackToMove) {
//            movesSmart(state, i, &possibleMoves);
//        }
//    }
//
//    if (state->blackToMove) {
//        score = std::numeric_limits<float>::infinity();
//    } else {
//        score = -std::numeric_limits<float>::infinity();
//    }
//
//    for (auto mv : possibleMoves) {
//        memcpy(&test, state, sizeof(Board));
//        move(&test, mv);
//        float newScore = alphabeta(&test, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), depth);
//
//        if ((state->blackToMove && newScore < score) || (!state->blackToMove && newScore > score)) {
//            score = newScore;
//            best = mv;
//        }
//    }
//
//    return best;
//}

Move MinMaxEngine::process(Board *state) {
    possibleMoves.clear();

    Board test;
    int won, total;
    float score = 0;
    Move best = DUMMY_MOVE;

    for (int i = 20; i < 100; i++) {
        auto p = state->pieces[i];
        if (p.type != INVALID && p.type != EMPTY && p.black == state->blackToMove) {
            movesSmart(state, i, &possibleMoves);
        }
    }

    for (auto mv : possibleMoves) {
        won = total = 0;
        memcpy(&test, state, sizeof(Board));
        move(&test, mv);
        monteCarlo(&test, &won, &total, depth);

        float newScore = (float)won / (float)total;

        if (newScore > score) {
            score = newScore;
            best = mv;
        }
    }

    return best;
}
