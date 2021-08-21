//
// Created by mart on 8/17/21.
//

#include <cstdlib>
#include <cstring>
#include <random>
#include "board.h"
#include "find_moves.h"

Board* copy(Board* source) {
    auto ptr = malloc(sizeof(Board));
    memcpy(ptr, source, sizeof(Board));
    return (Board*)ptr;
}

void rotate(Board* board) {
    board->lastMove.fromIndex = rotateIndex(board->lastMove.fromIndex);
    board->lastMove.toIndex = rotateIndex(board->lastMove.toIndex);

    for (int i = 20; i < 60; i++) {
        auto tmp = board->pieces[i];
        board->pieces[i] = board->pieces[rotateIndex(i)];
        board->pieces[rotateIndex(i)] = tmp;
    }
}

int rotateIndex(int index) {
    return 119 - index;
}

void move(Board* board, Move move) {
    board->blackToMove = !board->blackToMove;

    if (move.fromIndex != -1) {
        if (move.isEnPassant) {
            board->pieces[board->lastMove.toIndex] = EMPTY_PIECE;
        }

        if (move.isCastle) {
            if (move.toIndex > move.fromIndex) {
                board->pieces[move.toIndex - 1] = board->pieces[move.toIndex + 2];
                board->pieces[move.toIndex + 2] = EMPTY_PIECE;
            } else {
                board->pieces[move.toIndex + 1] = board->pieces[move.toIndex - 1];
                board->pieces[move.toIndex - 1] = EMPTY_PIECE;
            }
        }

        if (move.promotion != PieceType::INVALID) {
            board->pieces[move.fromIndex].type = move.promotion;
        }

        board->pieces[move.toIndex] = board->pieces[move.fromIndex];
        board->pieces[move.toIndex].moved = true;
        board->pieces[move.fromIndex] = EMPTY_PIECE;

        if (move.fromIndex == board->lastMove2.toIndex && move.toIndex == board->lastMove2.fromIndex) {
            board->repeatedMoves++;
        } else {
            board->repeatedMoves = 0;
        }
    }

    board->lastMove2 = board->lastMove;
    board->lastMove = move;
}

void updateState(Board* board) {
    int whitePieces[16];
    int blackPieces[16];
    int wi, bi;
    wi = bi = 0;

    if (board->repeatedMoves >= 6) {
        board->state = GameState::DRAW;
        return;
    }

    memset(&whitePieces, 0, 16 * sizeof(int));
    memset(&blackPieces, 0, 16 * sizeof(int));

    for (int i = 20; i < 100; i++) {
        auto p = board->pieces[i];
        if (p.type != PieceType::INVALID && p.type != PieceType::EMPTY) {
            if (p.black) {
                blackPieces[bi] = i;
                bi++;
            } else {
                whitePieces[wi] = i;
                wi++;
            }
        }
    }

    std::vector<Move> blackMoves, whiteMoves;

    // check black moves
    for (auto i : blackPieces) {
        auto found = movesSmart(board, i);
        blackMoves.insert(blackMoves.end(), found->begin(), found->end());
        delete found;
    }
    // check white moves
    for (auto i : whitePieces) {
        auto found = movesSmart(board, i);
        whiteMoves.insert(whiteMoves.end(), found->begin(), found->end());
        delete found;
    }

    bool isMate = false;
    if (board->blackToMove) {
        if (blackMoves.size() == 0) {
            for (auto mv : whiteMoves) {
                if (board->pieces[mv.toIndex].type == PieceType::KING) {
                    isMate = true;
                    break;
                }
            }

            if (isMate) {
                board->state = GameState::CHECKMATE_WHITE;
            } else {
                board->state = GameState::STALEMATE;
            }

            return;
        }
    } else {
        if (whiteMoves.size() == 0) {
            for (auto mv : blackMoves) {
                if (board->pieces[mv.toIndex].type == PieceType::KING) {
                    isMate = true;
                    break;
                }
            }

            if (isMate) {
                board->state = GameState::CHECKMATE_BLACK;
            } else {
                board->state = GameState::STALEMATE;
            }

            return;
        }
    }

    // check draw by insufficient material
    int whiteKB, blackKB, whiteOther, blackOther;
    whiteKB = blackKB = whiteOther = blackOther = 0;
    for (int i : whitePieces) {
        auto p = board->pieces[i];
        if (p.type == PieceType::KNIGHT || p.type == PieceType::BISHOP) {
            whiteKB++;
        } else if (p.type != PieceType::KING) {
            whiteOther++;
        }
    }
    for (int i : blackPieces) {
        auto p = board->pieces[i];
        if (p.type == PieceType::KNIGHT || p.type == PieceType::BISHOP) {
            blackKB++;
        } else if (p.type != PieceType::KING) {
            blackOther++;
        }
    }

    if (blackOther == 0 && whiteOther == 0 && blackKB <= 1 && whiteKB <= 1) {
        board->state = GameState::DRAW;
    }
}

Board* standard() {
    auto ptr = (Board*)malloc(sizeof(Board));

    ptr->state = GameState::PLAYING;
    ptr->blackToMove = false;
    ptr->lastMove = DUMMY_MOVE;

    for (int index = 0; index < 120; index++) {
        int row = index / 10;
        int column = index % 10;
        if (row < 2 || row > 9 || column == 0 || column == 9) {
            ptr->pieces[index] = DUMMY_PIECE;
            continue;
        }

        auto p = ptr->pieces[index];

        if (row < 4) {
            // white
            p.black = false;
        } else if (row > 7) {
            // black
            p.black = true;
        } else {
            // empty
            ptr->pieces[index] = EMPTY_PIECE;
            continue;
        }

        p.moved = false;

        if (row == 3 || row == 8) {
            p.type = PieceType::PAWN;
        } else {
            switch(column) {
                case 1:
                case 8:
                    p.type = PieceType::ROOK;
                    break;
                case 2:
                case 7:
                    p.type = PieceType::KNIGHT;
                    break;
                case 3:
                case 6:
                    p.type = PieceType::BISHOP;
                    break;
                case 4:
                    p.type = PieceType::KING;
                    break;
                default:
                    p.type = PieceType::QUEEN;
            }
        }

        ptr->pieces[index] = p;
    }

    return ptr;
}

static PieceType setup960[] = {
    KING, QUEEN, ROOK, ROOK, BISHOP, BISHOP, KNIGHT, KNIGHT
};

Board* s960() {
    auto ptr = (Board*)malloc(sizeof(Board));

    std::shuffle(&setup960[0], &setup960[7], std::mt19937(std::random_device()()));

    ptr->state = GameState::PLAYING;
    ptr->blackToMove = false;
    ptr->lastMove = DUMMY_MOVE;

    for (int index = 0; index < 120; index++) {
        int row = index / 10;
        int column = index % 10;
        if (row < 2 || row > 9 || column == 0 || column == 9) {
            ptr->pieces[index] = DUMMY_PIECE;
            continue;
        }

        auto p = ptr->pieces[index];

        if (row < 4) {
            // white
            p.black = false;
        } else if (row > 7) {
            // black
            p.black = true;
        } else {
            // empty
            ptr->pieces[index] = EMPTY_PIECE;
            continue;
        }

        p.moved = false;

        if (row == 3 || row == 8) {
            p.type = PieceType::PAWN;
        } else {
            p.type = setup960[column-1];
        }

        ptr->pieces[index] = p;
    }

    return ptr;
}
