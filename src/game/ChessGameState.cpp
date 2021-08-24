//
// Created by mart on 8/24/21.
//

#include <cstring>
#include <random>
#include "ChessGameState.h"
#include "../engine/board_evaluation.h"

ChessGameState::ChessGameState() {
    maxPlayers = 2;
    standard();
}

void ChessGameState::rotate() {
    for (int index = 20; index < 60; index++) {
        auto tmp = pieces[index];
        pieces[index] = pieces[119-index];
        pieces[119-index] = tmp;
    }
}

void ChessGameState::standard() {
    lastMove.dummy();
    lastMove2.dummy();

    for (int index = 0; index < 120; index++) {
        int row = index / 10;
        int column = index % 10;
        if (row < 2 || row > 9 || column == 0 || column == 9) {
            pieces[index].dummy();
            continue;
        }

        auto p = pieces[index];

        if (row < 4) {
            // white
            p.black = false;
        } else if (row > 7) {
            // black
            p.black = true;
        } else {
            // empty
            pieces[index].empty();
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

        pieces[index] = p;
    }
}

static PieceType setup960[] = {
    KING, QUEEN, ROOK, ROOK, BISHOP, BISHOP, KNIGHT, KNIGHT
};

void ChessGameState::s960() {
    std::shuffle(&setup960[0], &setup960[7], std::mt19937(std::random_device()()));

    for (int index = 0; index < 120; index++) {
        int row = index / 10;
        int column = index % 10;
        if (row < 2 || row > 9 || column == 0 || column == 9) {
            pieces[index].dummy();
            continue;
        }

        auto p = pieces[index];

        if (row < 4) {
            // white
            p.black = false;
        } else if (row > 7) {
            // black
            p.black = true;
        } else {
            // empty
            pieces[index].empty();
            continue;
        }

        p.moved = false;

        if (row == 3 || row == 8) {
            p.type = PieceType::PAWN;
        } else {
            p.type = setup960[column-1];
        }

        pieces[index] = p;
    }
}

int ChessGameState::getWinner() {
    switch (status) {
        case PLAYING:
            return -1;
        case CHECKMATE_WHITE:
            return 0;
        case CHECKMATE_BLACK:
            return 1;
        default:
            return 2;
    }
}

int ChessGameState::getWinningPlayer() {
    float socre = evaluate(*this);
    if (socre <= -1) {
        return 1;
    } else if (socre >= 1) {
        return 0;
    }
    return 2;
}

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

void ChessGameState::getPossibleMovesSimple(std::vector<ChessMove>& found, int index) {
    auto p = pieces[index];
    auto mvl = PIECE_MOVES[p.type];

    int direction, i;
    ChessPiece target {};

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
                    target = pieces[current];
                    if (target.type == PieceType::EMPTY) {
                        // we can move freely
                        found.emplace_back(ChessMove(
                            (char)index, (char)current, false, false, PieceType::INVALID
                        ));
                    } else {
                        if (target.type != PieceType::INVALID && target.black != p.black) {
                            // capture
                            found.emplace_back(ChessMove(
                                (char)index, (char)current, false, false, PieceType::INVALID
                            ));
                        }
                        break;
                    }
                }
            }

            break;

        case KING:
            // check castle, fall through to knight
            if (!p.moved) {
                if (pieces[index-1].type == PieceType::EMPTY &&
                pieces[index-2].type == PieceType::EMPTY &&
                pieces[index-3].type == PieceType::ROOK && !pieces[index-3].moved && pieces[index-3].black == p.black
                ) {
                    // short castle
                    found.emplace_back(ChessMove(
                        (char)index, (char)(index - 2), true, false, PieceType::INVALID
                    ));
                }

                if (pieces[index+1].type == PieceType::EMPTY &&
                pieces[index+2].type == PieceType::EMPTY &&
                pieces[index+3].type == PieceType::EMPTY &&
                pieces[index+4].type == PieceType::ROOK && !pieces[index+4].moved && pieces[index+4].black == p.black
                ) {
                    // long castle
                    found.emplace_back(ChessMove(
                        (char)index, (char)(index + 2), true, false, PieceType::INVALID
                        ));
                }
            }

        case KNIGHT:
            for (i = 0; i < 8; i++) {
                direction = mvl[i];
                target = pieces[index + direction];
                if (target.type == PieceType::EMPTY || (target.type != PieceType::INVALID && target.black != p.black)) {
                    found.emplace_back(ChessMove(
                        (char)index, (char)(index + direction), false, false, PieceType::INVALID
                        ));
                }
            }
            break;

        case PAWN:
            int coeff = ((int)!p.black) * 2 - 1;
            direction = coeff * 10;
            bool nextRankLast = pieces[index + 2 * direction].type == PieceType::INVALID;

            if (pieces[index + direction].type == PieceType::EMPTY) {
                if (nextRankLast) {
                    for (auto pr : PROMOTIONS) {
                        found.emplace_back(ChessMove(
                            (char)index, (char)(index + direction), false, false, pr
                            ));
                    }
                } else {
                    found.emplace_back(ChessMove(
                        (char)index, (char)(index + direction), false, false, PieceType::INVALID
                        ));

                    if (!p.moved && pieces[index + 2 * direction].type == PieceType::EMPTY) {
                        found.emplace_back(ChessMove(
                            (char)index, (char)(index + 2 * direction), false, false, PieceType::INVALID
                            ));
                    }

                    if (lastMove.fromIndex != -1 && std::abs(lastMove.toIndex - index) == 1 && std::abs(lastMove.toIndex - lastMove.fromIndex) == 20 && pieces[lastMove.toIndex].type == PieceType::PAWN) {
                        found.emplace_back(ChessMove(
                            (char)index, (char)(lastMove.toIndex + direction), false, true, PieceType::INVALID
                            ));
                    }
                }
            }

            for (int j = -1; j < 2; j += 2) {
                int idx = index + direction + j;
                auto tgt = pieces[idx];

                if (tgt.type != PieceType::INVALID && tgt.type != PieceType::EMPTY && tgt.black != p.black) {
                    if (nextRankLast) {
                        for (auto pr : PROMOTIONS) {
                            found.emplace_back(ChessMove(
                                (char)index, (char)(idx), false, false, pr
                                ));
                        }
                    } else {
                        found.emplace_back(ChessMove(
                            (char)index, (char)(idx), false, false, PieceType::INVALID
                            ));
                    }
                }
            }
            break;
    }
}

static std::vector<ChessMove> found(120);
static std::vector<ChessMove> replies(120);
static ChessGameState tempMoveState {};

void ChessGameState::getPossibleMoves(std::vector<ChessMove>& vector, int index) {
    found.clear();
    replies.clear();

    getPossibleMovesSimple(found, index);

    int otherPieces[16];
    memset(otherPieces, 0, sizeof(int) * 16);
    int idx = 0;
    for (int i = 20; i < 100; i++) {
        auto p = pieces[i];
        if (p.type != PieceType::INVALID && p.type != PieceType::EMPTY && p.black != blackToMove) {
            otherPieces[idx] = i;
            idx++;
        }
    }

    for (auto &mv : found) {
        tempMoveState = *this;
        tempMoveState.move(mv);

        bool goodMove = true;
        for (auto opponent : otherPieces) {
            if (opponent == mv.toIndex) continue;  // captured
            if (opponent == 0) break;  // no more enemy pieces

            tempMoveState.getPossibleMovesSimple(replies, opponent);

            for (auto &reply : replies) {
                if (tempMoveState.pieces[reply.toIndex].type == PieceType::KING) {
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
            vector.emplace_back(mv);
        }
    }
}

void ChessGameState::getPossibleMoves(std::vector<ChessMove> &vector) {
    for (int i = 20; i < 100; i++) {
        if (pieces[i].black == blackToMove) {
            getPossibleMoves(vector, i);
        }
    }
}

void ChessGameState::move(ChessMove &move) {
    blackToMove = !blackToMove;

    if (move.fromIndex != -1) {
        if (move.isEnPassant) {
            pieces[lastMove.toIndex].empty();
        }

        if (move.isCastle) {
            if (move.toIndex > move.fromIndex) {
                pieces[move.toIndex - 1] = pieces[move.toIndex + 2];
                pieces[move.toIndex + 2].empty();
            } else {
                pieces[move.toIndex + 1] = pieces[move.toIndex - 1];
                pieces[move.toIndex - 1].empty();
            }
        }

        if (move.promotion != PieceType::INVALID) {
            pieces[move.fromIndex].type = move.promotion;
        }

        pieces[move.toIndex] = pieces[move.fromIndex];
        pieces[move.toIndex].moved = true;
        pieces[move.fromIndex].empty();

        if (move.fromIndex == lastMove2.toIndex && move.toIndex == lastMove2.fromIndex) {
            repeatedMoves++;
        } else {
            repeatedMoves = 0;
        }
    }

    lastMove2 = lastMove;
    lastMove = move;
    currentPlayer = 1-currentPlayer;
}

static std::vector<ChessMove> blackMoves(20), whiteMoves(20), foundMoves(5);
void ChessGameState::update() {
    int whitePieces[16];
    int blackPieces[16];
    int wi, bi;
    wi = bi = 0;

    if (repeatedMoves >= 6) {
        status = Status::DRAW;
        return;
    }

    memset(&whitePieces, 0, 16 * sizeof(int));
    memset(&blackPieces, 0, 16 * sizeof(int));

    for (int i = 20; i < 100; i++) {
        auto p = pieces[i];
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

    // check black moves
    for (auto i : blackPieces) {
        getPossibleMoves(foundMoves, i);
        blackMoves.insert(blackMoves.end(), foundMoves.begin(), foundMoves.end());
    }
    // check white moves
    for (auto i : whitePieces) {
        getPossibleMoves(foundMoves, i);
        whiteMoves.insert(whiteMoves.end(), foundMoves.begin(), foundMoves.end());
    }

    bool isMate = false;
    if (blackToMove) {
        if (blackMoves.empty()) {
            for (auto mv : whiteMoves) {
                if (pieces[mv.toIndex].type == PieceType::KING) {
                    isMate = true;
                    break;
                }
            }

            if (isMate) {
                status = Status::CHECKMATE_WHITE;
            } else {
                status = Status::STALEMATE;
            }

            return;
        }
    } else {
        if (whiteMoves.empty()) {
            for (auto mv : blackMoves) {
                if (pieces[mv.toIndex].type == PieceType::KING) {
                    isMate = true;
                    break;
                }
            }

            if (isMate) {
                status = Status::CHECKMATE_BLACK;
            } else {
                status = Status::STALEMATE;
            }

            return;
        }
    }

    // check draw by insufficient material
    int whiteKB, blackKB, whiteOther, blackOther;
    whiteKB = blackKB = whiteOther = blackOther = 0;
    for (int i : whitePieces) {
        auto p = pieces[i];
        if (p.type == PieceType::KNIGHT || p.type == PieceType::BISHOP) {
            whiteKB++;
        } else if (p.type != PieceType::KING) {
            whiteOther++;
        }
    }
    for (int i : blackPieces) {
        auto p = pieces[i];
        if (p.type == PieceType::KNIGHT || p.type == PieceType::BISHOP) {
            blackKB++;
        } else if (p.type != PieceType::KING) {
            blackOther++;
        }
    }

    if (blackOther == 0 && whiteOther == 0 && blackKB <= 1 && whiteKB <= 1) {
        status = Status::DRAW;
    }
}

long ChessGameState::hash() {
    // TODO
    return 0;
}
