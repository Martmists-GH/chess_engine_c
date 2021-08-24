//
// Created by mart on 8/24/21.
//

#include "ChessMove.h"

ChessMove::ChessMove() : ChessMove(-1, -1) { }

ChessMove::ChessMove(int fromIndex, int toIndex) : ChessMove(fromIndex, toIndex, false, false, INVALID) { }

ChessMove::ChessMove(int fromIndex, int toIndex, bool isCastle, bool isEnPassant, PieceType promotion) : fromIndex(fromIndex), toIndex(toIndex), isCastle(isCastle), isEnPassant(isEnPassant), promotion(promotion) { }

void ChessMove::dummy() {
    fromIndex = toIndex = -1;
    isCastle = isEnPassant = false;
    promotion = INVALID;
}

long ChessMove::hash() {
    return fromIndex + (toIndex << 8) + (isCastle << 16) + (isEnPassant << 17) + (promotion << 18);
}
