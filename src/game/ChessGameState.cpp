//
// Created by mart on 8/24/21.
//

#include <cstring>
#include <random>
#include "ChessGameState.h"
#include "../engine/board_evaluation.h"

void rotate(ChessGameState* this_) {
    for (int index = 20; index < 60; index++) {
        auto tmp = this_->pieces[index];
        this_->pieces[index] = this_->pieces[119-index];
        this_->pieces[119-index] = tmp;
    }
}

void standard(ChessGameState* this_) {
    this_->lastMove = DUMMY_MOVE;
    this_->lastMove2 = DUMMY_MOVE;

    for (int index = 0; index < 120; index++) {
        int row = index / 10;
        int column = index % 10;
        if (row < 2 || row > 9 || column == 0 || column == 9) {
            this_->pieces[index] = INVALID_PIECE;
            continue;
        }

        auto p = this_->pieces[index];

        if (row < 4) {
            // white
            p.black = false;
        } else if (row > 7) {
            // black
            p.black = true;
        } else {
            // empty
            this_->pieces[index] = EMPTY_PIECE;
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

        this_->pieces[index] = p;
    }
}

static PieceType setup960[] = {
    KING, QUEEN, ROOK, ROOK, BISHOP, BISHOP, KNIGHT, KNIGHT
};

void s960(ChessGameState* this_) {
    std::shuffle(&setup960[0], &setup960[7], std::mt19937(std::random_device()()));

    for (int index = 0; index < 120; index++) {
        int row = index / 10;
        int column = index % 10;
        if (row < 2 || row > 9 || column == 0 || column == 9) {
            this_->pieces[index] = INVALID_PIECE;
            continue;
        }

        auto p = this_->pieces[index];

        if (row < 4) {
            // white
            p.black = false;
        } else if (row > 7) {
            // black
            p.black = true;
        } else {
            // empty
            this_->pieces[index] = EMPTY_PIECE;
            continue;
        }

        p.moved = false;

        if (row == 3 || row == 8) {
            p.type = PieceType::PAWN;
        } else {
            p.type = setup960[column-1];
        }

        this_->pieces[index] = p;
    }
}

int getWinner(ChessGameState* this_) {
    switch (this_->status) {
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

int getWinningPlayer(ChessGameState* this_) {
    float socre = evaluate(this_);
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

void getPossibleMovesSimple(ChessGameState* this_, std::vector<ChessMove>& found, int index) {
    auto p = this_->pieces[index];
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
                    target = this_->pieces[current];
                    if (target.type == PieceType::EMPTY) {
                        // we can move freely
                        found.emplace_back(ChessMove{
                                (char) index, (char) current, false, false, PieceType::INVALID
                        });
                    } else {
                        if (target.type != PieceType::INVALID && target.black != p.black) {
                            // capture
                            found.emplace_back(ChessMove{
                                    (char) index, (char) current, false, false, PieceType::INVALID
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
                if (this_->pieces[index-1].type == PieceType::EMPTY &&
                        this_->pieces[index-2].type == PieceType::EMPTY &&
                        this_->pieces[index-3].type == PieceType::ROOK && !this_->pieces[index-3].moved && this_->pieces[index-3].black == p.black
                ) {
                    // short castle
                    found.emplace_back(ChessMove{
                            (char) index, (char) (index - 2), true, false, PieceType::INVALID
                    });
                }

                if (this_->pieces[index+1].type == PieceType::EMPTY &&
                        this_->pieces[index+2].type == PieceType::EMPTY &&
                        this_->pieces[index+3].type == PieceType::EMPTY &&
                        this_->pieces[index+4].type == PieceType::ROOK && !this_->pieces[index+4].moved && this_->pieces[index+4].black == p.black
                ) {
                    // long castle
                    found.emplace_back(ChessMove{
                            (char) index, (char) (index + 2), true, false, PieceType::INVALID
                    });
                }
            }

        case KNIGHT:
            for (i = 0; i < 8; i++) {
                direction = mvl[i];
                target = this_->pieces[index + direction];
                if (target.type == PieceType::EMPTY || (target.type != PieceType::INVALID && target.black != p.black)) {
                    found.emplace_back(ChessMove{
                            (char) index, (char) (index + direction), false, false, PieceType::INVALID
                    });
                }
            }
            break;

        case PAWN:
            int coeff = ((int)!p.black) * 2 - 1;
            direction = coeff * 10;
            bool nextRankLast = this_->pieces[index + 2 * direction].type == PieceType::INVALID;

            if (this_->pieces[index + direction].type == PieceType::EMPTY) {
                if (nextRankLast) {
                    for (auto pr : PROMOTIONS) {
                        found.emplace_back(ChessMove{
                                (char) index, (char) (index + direction), false, false, pr
                        });
                    }
                } else {
                    found.emplace_back(ChessMove{
                            (char) index, (char) (index + direction), false, false, PieceType::INVALID
                    });

                    if (!p.moved && this_->pieces[index + 2 * direction].type == PieceType::EMPTY) {
                        found.emplace_back(ChessMove{
                                (char) index, (char) (index + 2 * direction), false, false, PieceType::INVALID
                        });
                    }

                    if (this_->lastMove.fromIndex != -1 && std::abs(this_->lastMove.toIndex - index) == 1 && std::abs(this_->lastMove.toIndex - this_->lastMove.fromIndex) == 20 && this_->pieces[this_->lastMove.toIndex].type == PieceType::PAWN) {
                        found.emplace_back(ChessMove{
                                (char) index, (char) (this_->lastMove.toIndex + direction), false, true, PieceType::INVALID
                        });
                    }
                }
            }

            for (int j = -1; j < 2; j += 2) {
                int idx = index + direction + j;
                auto tgt = this_->pieces[idx];

                if (tgt.type != PieceType::INVALID && tgt.type != PieceType::EMPTY && tgt.black != p.black) {
                    if (nextRankLast) {
                        for (auto pr : PROMOTIONS) {
                            found.emplace_back(ChessMove{
                                    (char) index, (char) (idx), false, false, pr
                            });
                        }
                    } else {
                        found.emplace_back(ChessMove{
                                (char) index, (char) (idx), false, false, PieceType::INVALID
                        });
                    }
                }
            }
            break;
    }
}

static std::vector<ChessMove> found(120);
static std::vector<ChessMove> replies(120);
static ChessGameState tempMoveState {};

void getPossibleMoves(ChessGameState* this_, std::vector<ChessMove>& vector, int index) {
    found.clear();
    replies.clear();

    getPossibleMovesSimple(this_, found, index);

    int otherPieces[16];
    memset(otherPieces, 0, sizeof(int) * 16);
    int idx = 0;
    for (int i = 20; i < 100; i++) {
        auto p = this_->pieces[i];
        if (p.type != PieceType::INVALID && p.type != PieceType::EMPTY && p.black != this_->blackToMove) {
            otherPieces[idx] = i;
            idx++;
        }
    }

    for (ChessMove &mv : found) {
        memcpy(&tempMoveState, this_, sizeof(ChessGameState));
        move(&tempMoveState, mv);

        bool goodMove = true;
        for (auto opponent : otherPieces) {
            if (opponent == mv.toIndex) continue;  // captured
            if (opponent == 0) break;  // no more enemy pieces

            getPossibleMovesSimple(&tempMoveState, replies, opponent);

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

void getPossibleMoves(ChessGameState* this_, std::vector<ChessMove> &vector) {
    for (int i = 20; i < 100; i++) {
        if (this_->pieces[i].black == this_->blackToMove) {
            getPossibleMoves(this_, vector, i);
        }
    }
}

void move(ChessGameState* this_, ChessMove move) {
    this_->blackToMove = !this_->blackToMove;

    if (move.fromIndex != -1) {
        if (move.isEnPassant) {
            this_->pieces[this_->lastMove.toIndex] = EMPTY_PIECE;
        }

        if (move.isCastle) {
            if (move.toIndex > move.fromIndex) {
                this_->pieces[move.toIndex - 1] = this_->pieces[move.toIndex + 2];
                this_->pieces[move.toIndex + 2] = EMPTY_PIECE;
            } else {
                this_->pieces[move.toIndex + 1] = this_->pieces[move.toIndex - 1];
                this_->pieces[move.toIndex - 1] = EMPTY_PIECE;
            }
        }

        if (move.promotion != PieceType::INVALID) {
            this_->pieces[move.fromIndex].type = move.promotion;
        }

        this_->pieces[move.toIndex] = this_->pieces[move.fromIndex];
        this_->pieces[move.toIndex].moved = true;
        this_->pieces[move.fromIndex] = EMPTY_PIECE;

        if (move.fromIndex == this_->lastMove2.toIndex && move.toIndex == this_->lastMove2.fromIndex) {
            this_->repeatedMoves++;
        } else {
            this_->repeatedMoves = 0;
        }
    }

    this_->lastMove2 = this_->lastMove;
    this_->lastMove = move;
}

static std::vector<ChessMove> blackMoves(20), whiteMoves(20);
void update(ChessGameState* this_) {
    blackMoves.clear();
    whiteMoves.clear();

    int whitePieces[16];
    int blackPieces[16];
    int wi, bi;
    wi = bi = 0;

    if (this_->repeatedMoves >= 6) {
        this_->status = Status::DRAW;
        return;
    }

    memset(&whitePieces, 0, 16 * sizeof(int));
    memset(&blackPieces, 0, 16 * sizeof(int));

    for (int i = 20; i < 100; i++) {
        auto p = this_->pieces[i];
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
        getPossibleMoves(this_, blackMoves, i);
    }
    // check white moves
    for (auto i : whitePieces) {
        getPossibleMoves(this_, whiteMoves, i);
    }

    bool isMate = false;
    if (this_->blackToMove) {
        if (blackMoves.empty()) {
            for (auto mv : whiteMoves) {
                if (this_->pieces[mv.toIndex].type == PieceType::KING) {
                    isMate = true;
                    break;
                }
            }

            if (isMate) {
                this_->status = Status::CHECKMATE_WHITE;
            } else {
                this_->status = Status::STALEMATE;
            }

            return;
        }
    } else {
        if (whiteMoves.empty()) {
            for (auto mv : blackMoves) {
                if (this_->pieces[mv.toIndex].type == PieceType::KING) {
                    isMate = true;
                    break;
                }
            }

            if (isMate) {
                this_->status = Status::CHECKMATE_BLACK;
            } else {
                this_->status = Status::STALEMATE;
            }

            return;
        }
    }

    // check draw by insufficient material
    int whiteKB, blackKB, whiteOther, blackOther;
    whiteKB = blackKB = whiteOther = blackOther = 0;
    for (int i : whitePieces) {
        if (i == 0) break;
        auto p = this_->pieces[i];
        if (p.type == PieceType::KNIGHT || p.type == PieceType::BISHOP) {
            whiteKB++;
        } else if (p.type != PieceType::KING) {
            whiteOther++;
        }
    }
    for (int i : blackPieces) {
        if (i == 0) break;
        auto p = this_->pieces[i];
        if (p.type == PieceType::KNIGHT || p.type == PieceType::BISHOP) {
            blackKB++;
        } else if (p.type != PieceType::KING) {
            blackOther++;
        }
    }

    if (blackOther == 0 && whiteOther == 0 && blackKB <= 1 && whiteKB <= 1) {
        this_->status = Status::DRAW;
    }
}
