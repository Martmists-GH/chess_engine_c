//
// Created by mart on 8/19/21.
//

#include <cstring>
#include <cmath>
#include <cstdio>
#include "board.h"
#include "find_moves.h"

static int PIECE_MOVES[6][8] {
    { -11, -10, -9, -1, 1, 9, 10, 11 },
    { -11, -10, -9, -1, 1, 9, 10, 11 },
    { -11, -9, 9, 11, 0, 0, 0, 0 },
    { -21, -19, -12, -8, 8, 12, 19, 21 },
    { -10, -1, 1, 10, 0, 0, 0, 0 },
    { 10, 0, 0, 0, 0, 0, 0, 0 },
};

static PieceType PROMOTIONS[4] {
    KNIGHT, BISHOP, ROOK, QUEEN
};

void movesSimple(Board *board, int index, std::vector<Move>* found) {
    auto p = board->pieces[index];
    auto mvl = PIECE_MOVES[p.type];

    int direction, i;
    Piece target;

    switch (p.type) {
        case QUEEN:
        case BISHOP:
        case ROOK:
            for (i = 0; i < 8; i++) {
                direction = mvl[i];
                if (direction == 0) break;

                int current = index;
                while (true) {
                    current += direction;
                    target = board->pieces[current];
                    if (target.type == PieceType::EMPTY) {
                        // we can move freely
                        found->emplace_back(Move {
                            (char)index, (char)current, false, false, PieceType::INVALID
                        });
                    } else {
                        if (target.type != PieceType::INVALID && target.black != p.black) {
                            // capture
                            found->emplace_back(Move {
                                (char)index, (char)current, false, false, PieceType::INVALID
                            });
                        }
                        break;
                    }
                }
            }

            break;
        case KING:
            // check castle, fall through to knight
            if (!p.moved) {
                if (board->pieces[index-1].type == PieceType::EMPTY &&
                    board->pieces[index-2].type == PieceType::EMPTY &&
                    board->pieces[index-3].type == PieceType::ROOK && !board->pieces[index-3].moved && board->pieces[index-3].black == p.black
                ) {
                    // short castle
                    found->emplace_back(Move {
                        (char)index, (char)(index - 2), true, false, PieceType::INVALID
                    });
                }

                if (board->pieces[index+1].type == PieceType::EMPTY &&
                    board->pieces[index+2].type == PieceType::EMPTY &&
                    board->pieces[index+3].type == PieceType::EMPTY &&
                    board->pieces[index+4].type == PieceType::ROOK && !board->pieces[index+4].moved && board->pieces[index+4].black == p.black
                ) {
                    // long castle
                    found->emplace_back(Move {
                        (char)index, (char)(index + 2), true, false, PieceType::INVALID
                    });
                }
            }

        case KNIGHT:
            for (i = 0; i < 8; i++) {
                direction = mvl[i];
                target = board->pieces[index + direction];
                if (target.type == PieceType::EMPTY || (target.type != PieceType::INVALID && target.black != p.black)) {
                    found->emplace_back(Move {
                        (char)index, (char)(index + direction), false, false, PieceType::INVALID
                    });
                }
            }
            break;
        case PAWN:
            int coeff = ((int)!p.black) * 2 - 1;
            direction = coeff * 10;
            bool nextRankLast = board->pieces[index + 2 * direction].type == PieceType::INVALID;

            if (board->pieces[index + direction].type == PieceType::EMPTY) {
                if (nextRankLast) {
                    for (auto pr : PROMOTIONS) {
                        found->emplace_back(Move {
                            (char)index, (char)(index + direction), false, false, pr
                        });
                    }
                } else {
                    found->emplace_back(Move {
                        (char)index, (char)(index + direction), false, false, PieceType::INVALID
                    });

                    if (!p.moved && board->pieces[index + 2 * direction].type == PieceType::EMPTY) {
                        found->emplace_back(Move {
                            (char)index, (char)(index + 2 * direction), false, false, PieceType::INVALID
                        });
                    }

                    if (board->lastMove.fromIndex != -1 && std::abs(board->lastMove.toIndex - index) == 1 && std::abs(board->lastMove.toIndex - board->lastMove.fromIndex) == 20 && board->pieces[board->lastMove.toIndex].type == PieceType::PAWN) {
                        found->emplace_back(Move {
                            (char)index, (char)(board->lastMove.toIndex + direction), false, true, PieceType::INVALID
                        });
                    }
                }
            }

            for (int j = -1; j < 2; j += 2) {
                int idx = index + direction + j;
                auto tgt = board->pieces[idx];

                if (tgt.type != PieceType::INVALID && tgt.type != PieceType::EMPTY && tgt.black != p.black) {
                    if (nextRankLast) {
                        for (auto pr : PROMOTIONS) {
                            found->emplace_back(Move {
                                (char)index, (char)(idx), false, false, pr
                            });
                        }
                    } else {
                        found->emplace_back(Move {
                            (char)index, (char)(idx), false, false, PieceType::INVALID
                        });
                    }
                }
            }
            break;
    }
}

std::vector<Move> *movesSimple(Board *board, int index) {
    auto moves = new std::vector<Move>;
    movesSimple(board, index, moves);
    return moves;
}

static Board tempMoveBoard;

void movesSmart(Board *board, int index, std::vector<Move>* good) {
    std::vector<Move> found;
    movesSimple(board, index, &found);

    int otherPieces[16];
    memset(otherPieces, 0, sizeof(int) * 16);
    int idx = 0;
    for (int i = 20; i < 100; i++) {
        auto p = board->pieces[i];
        if (p.type != PieceType::INVALID && p.type != PieceType::EMPTY && p.black != board->blackToMove) {
            otherPieces[idx] = i;
            idx++;
        }
    }

    std::vector<Move> replies;
    for (auto &mv : found) {
        memcpy(&tempMoveBoard, board, sizeof(Board));
        move(&tempMoveBoard, mv);

        bool goodMove = true;
        for (auto opponent : otherPieces) {
            if (opponent == mv.toIndex) continue;  // captured
            if (opponent == 0) break;  // no more enemy pieces

            movesSimple(&tempMoveBoard, opponent, &replies);

            for (auto &reply : replies) {
                if (tempMoveBoard.pieces[reply.toIndex].type == PieceType::KING) {
                    // bad move
                    goodMove = false;
                    break;
                }

                if (mv.isCastle) {
                    // check if we weren't in check and didn't move through check
                    int castleIndex = mv.fromIndex + (mv.toIndex - mv.fromIndex) / 2;
                    if (reply.toIndex == mv.fromIndex || reply.toIndex == castleIndex) {
                        goodMove = false;
                        break;
                    }
                }
            }
            replies.clear();
        }

        if (goodMove) {
            good->emplace_back(mv);
        }
    }
}

std::vector<Move> *movesSmart(Board *board, int index) {
    auto moves = new std::vector<Move>;
    movesSmart(board, index, moves);
    return moves;
}
