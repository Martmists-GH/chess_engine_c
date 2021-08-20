//
// Created by mart on 8/20/21.
//

#include <cstring>
#include <vector>
#include "board_evaluation.h"
#include "../game/find_moves.h"

static float PIECE_SCORE[] = {
        200.0,
        9.0,
        3.3,
        3.1,
        5.0,
        1.0
};

float evaluatePieces(Board* board, const int* whitePieces, const int* blackPieces) {
    float score = 0;

    for (int i = 0; i < 16; i++) {
        int widx = whitePieces[i];
        int bidx = blackPieces[i];

        if (widx != 0) {
            score += PIECE_SCORE[board->pieces[widx].type];
        }

        if (bidx != 0) {
            score -= PIECE_SCORE[board->pieces[widx].type];
        }
    }

    return score;
}

static Board copyForBlack;

float evaluateMovement(Board* board, const int* whitePieces, const int* blackPieces) {
    float moves = 0.f;
    Board* white;
    Board* black;

    if (board->blackToMove) {
        black = board;
        white = &copyForBlack;
    } else {
        white = board;
        black = &copyForBlack;
    }

    std::vector<Move> movesAvailable;
    for (int i = 0; i < 16; i++) {
        int widx = whitePieces[i];
        int bidx = blackPieces[i];

        if (widx != 0) {
            movesSmart(white, widx, &movesAvailable);
            moves += movesAvailable.size();
        }
        if (bidx != 0) {
            movesSmart(black, bidx, &movesAvailable);
            moves -= movesAvailable.size();
        }
    }

    return .5f * moves;
}

float evaluate(Board *board) {
    updateState(board);
    switch(board->state) {
        case CHECKMATE_BLACK:
            return -999999;
        case CHECKMATE_WHITE:
            return 999999;
        case STALEMATE:
        case DRAW:
            return 0;
        default:
            int wp[16], bp[16];
            int wi = 0, bi = 0;
            memcpy(&copyForBlack, board, sizeof(Board));
            move(&copyForBlack, DUMMY_MOVE);

            memset(&wp, 0, 16 * sizeof(int));
            memset(&bp, 0, 16 * sizeof(int));

            for (int i = 20; i < 100; i++) {
                auto p = board->pieces[i];
                if (p.type != PieceType::INVALID && p.type != PieceType::EMPTY) {
                    if (p.black) {
                        bp[bi] = i;
                        bi++;
                    } else {
                        wp[wi] = i;
                        wi++;
                    }
                }
            }

            return evaluatePieces(board, wp, bp);
    }
}
